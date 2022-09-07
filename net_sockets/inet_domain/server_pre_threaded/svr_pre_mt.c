/*
 * Original / Credits :
 * ZeroHttpd web servers from H Shuveb!
 * https://github.com/shuveb/zerohttpd
 * Article:
 * Linux Applications Performance: Introduction, H Shuveb, Apr 2019
 * https://unixism.net/2019/04/linux-applications-performance-introduction/
 *
 * Adapted and further simplified here...
 * Kaiwan NB, kaiwanTECH
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include <pthread.h>
#include <errno.h>

#define DEFAULT_SERVER_PORT         8000
#define QLEN_MAX					10
#define METHOD_HANDLED              0

static char prg_name[128];

#define THREADS_COUNT   10
pthread_t threads[THREADS_COUNT];

int server_socket;
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;


/*
 * --------------------- Utility Routines follow -----------------------------
 */
const char *unimplemented_content =
    "HTTP/1.0 400 Bad Request\r\n"
    "Content-type: text/html\r\n"
    "\r\n"
    "<html>"
    "<head>"
    "<title>ZeroHTTPd: Unimplemented</title>"
    "</head>"
    "<body>"
    "<h1>Bad Request (Unimplemented)</h1>"
    "<p>Your client sent a request ZeroHTTPd did not understand and it is probably not your fault.</p>"
    "</body>" "</html>";

const char *http_404_content =
    "HTTP/1.0 404 Not Found\r\n"
    "Content-type: text/html\r\n"
    "\r\n"
    "<html>"
    "<head>"
    "<title>ZeroHTTPd: Not Found</title>"
    "</head>"
    "<body>"
    "<h1>Not Found (404)</h1>"
    "<p>Your client is asking for an object that was not found on this server.</p>"
    "</body>" "</html>";

/*
 One function that prints the system call and the error details
 and then exits with error code 1. Non-zero meaning things didn't go well.
 */
void fatal_error(const char *syscall)
{
	perror(syscall);
	exit(errno);
}

/*
 * Reads the client socket character-by-character and once a line is read, the string
 * is NULL-terminated and the function returns the characters read.
 * This is not a very efficient way of doing things since we are making multiple calls
 * to read(), but it keeps things simple.
 * */

int get_line(int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';
	ssize_t n;

	while ((i < size - 1) && (c != '\n')) {
		n = recv(sock, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
					recv(sock, &c, 1, 0);
			} else {
				buf[i] = c;
				i++;
			}
		} else
			return 0;
	}
	buf[i] = '\0';

	return i;
}

/*
 * Utility function to convert a string to lower case.
 * */

void strtolower(char *str)
{
	for (; *str; ++str)
		*str = (char)tolower(*str);
}

/*
 * When ZeroHTTPd encounters any other HTTP method other than GET or POST, this function
 * is used to inform the client.
 * */

void handle_unimplemented_method(int client_socket)
{
	send(client_socket, unimplemented_content,
	     strlen(unimplemented_content), 0);
}

/*
 * This function is used to send a "HTTP Not Found" code and message to the client in
 * case the file requested is not found.
 * */

void handle_http_404(int client_socket)
{
	send(client_socket, http_404_content, strlen(http_404_content), 0);
}

/*
 * Simple function to get the file extension of the file that we are about to serve.
 * */

const char *get_filename_ext(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/*
 * HTML URLs and other data like data sent over the POST method encoded using a simple
 * scheme. This function takes the encoded data and returns a regular, decoded string.
 * e.g:
 * Encoded: Nothing+is+better+than+bread+%26+butter%21
 * Decoded: Nothing is better than bread & butter!
 * */
char *urlencoding_decode(char *str)
{
	char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
	while (*pstr) {
		if (*pstr == '%') {
			if (pstr[1] && pstr[2]) {
				*pbuf++ =
				    from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		} else if (*pstr == '+') {
			*pbuf++ = ' ';
		} else {
			*pbuf++ = *pstr;
		}
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}
/* ------------------------------------------------------------------------- */


/*
 * Once a static file is identified to be served, this function is used to read the file
 * and write it over the client socket using Linux's sendfile() system call. This saves us
 * the hassle of transferring file buffers from kernel to user space and back - in effect,
 * a zero-copy technique!
 * */

int transfer_file_contents(char *file_path, int client_socket)
{
	int fd, ret;
	struct stat statb;
	off_t filesz = 0;

	//--- Serve only regular files with size > 0
	ret = stat(file_path, &statb);
	if (ret < 0) {
		perror("stat failed");
		return 1;
	}
	if (!(statb.st_mode & S_IFREG))
		return 2;
	filesz = statb.st_size;
	if (filesz <= 0)
		return 3;

	fd = open(file_path, O_RDONLY);
	if (fd < 0) {
		perror("open failed");
		return 4;
	}
	ret = sendfile(client_socket, fd, NULL, filesz);
	if (ret < 0) {
		perror("sendfile failed");
		return 5;
	}
	close(fd);

	return 0;
}

/*
 * This function is called per client request. By default, it transfers a file
 * - specified by the macro FILE_TO_TRANSFER - to the client.
 * */

// Replace this with the file you'd like to transfer :)
#define FILE_TO_TRANSFER	"/etc/passwd"

void handle_client(int client_socket, long thrdnum)
{
	char msg[512];
	int ret;

	// Send a simple message -or- send a file to the client
#if 0
	snprintf(msg, 127,
		 "Hello, from pre-threaded concurrent server [thread# %ld]\n",
		 thrdnum);
	ret = send(client_socket, msg, strlen(msg), 0);
	if (ret < 0)
		fatal_error("send(2) failed");
#else
	ret = transfer_file_contents(FILE_TO_TRANSFER, client_socket);
	switch (ret) {
	case 0:
		return; // all ok
	// Error cases ...
	case 1:
		snprintf(msg, 512,
		 "%s:%s():thread# %ld: transferring file %s failed; reason: stat(2) failed\n",
		prg_name, __func__, thrdnum, FILE_TO_TRANSFER);
		break;
	case 2:
		snprintf(msg, 512,
		 "%s:%s():thread# %ld: transferring file %s failed; reason: file isn't a regular file\n",
		prg_name, __func__, thrdnum, FILE_TO_TRANSFER);
		break;
	case 3:
		snprintf(msg, 512,
		 "%s:%s():thread# %ld: transferring file %s failed; reason: empty file\n",
		prg_name, __func__, thrdnum, FILE_TO_TRANSFER);
		break;
	case 4:
		snprintf(msg, 512,
		 "%s:%s():thread# %ld: transferring file %s failed; reason: open failed (permissions?)\n",
		prg_name, __func__, thrdnum, FILE_TO_TRANSFER);
		break;
	case 5:
		snprintf(msg, 512,
		 "%s:%s():thread# %ld: transferring file %s failed; reason: sendfile(2) failed\n",
		prg_name, __func__, thrdnum, FILE_TO_TRANSFER);
		break;
	default:
		snprintf(msg, 512,
		 "%s:%s():thread# %ld: transferring file %s: unknown issue (%d)\n",
		prg_name, __func__, thrdnum, FILE_TO_TRANSFER, ret);
		break;
	}
	strncat(msg, "Lookup server log for details.\n", 511);
	ret = send(client_socket, msg, strlen(msg), 0);
	if (ret < 0)
		fatal_error("send(2) failed");
#endif
}

/*
 * This function is the main server loop. It never returns. In a loop, it accepts client
 * connections and calls handle_client() to serve the request. Once the request is served,
 * it closes the client connection and goes back to waiting for a new client connection,
 * calling accept() again.
 * Here, the call to accept() is protected with a mutex so that only one thread in the
 * thread pool can actually be blocked in accept(). This is to avoid the thundering herd
 * problem that supposedly affects some versions of Unix operating systems.
 * */

void *enter_server_loop(void *targ)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	long thrdnum = (long)targ;

	while (1) {
#if 1
		pthread_mutex_lock(&mlock);
#endif
		long client_socket = accept(server_socket,
					    (struct sockaddr *)&client_addr,
					    &client_addr_len);
		if (client_socket == -1)
			fatal_error("accept()");
#if 1
		pthread_mutex_unlock(&mlock);
#endif

		handle_client(client_socket, thrdnum);
	}
}

#define PER_THRD_STACKSIZE		(8*1024*1024)
void create_thread(long index)
{
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, PER_THRD_STACKSIZE);

	if (pthread_create
	    (&threads[index], &attr, &enter_server_loop, (void *)index))
		fprintf(stderr,
			"%s(): !WARNING! pthread_create() failed [index %ld]; errno=%d\n",
			__func__, index, errno);
}

/*
 * This function is responsible for setting up the main listening socket used by the
 * web server.
 * */

int setup_listening_socket(int port)
{
	int sock;
	struct sockaddr_in srv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		fatal_error("socket()");

	/*
	 * The (in)famous 'Address already in use' issue can crop up!
	 * Explanation:
	 * http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/unix-socket-faq-4.html#ss4.2
	 * One typical solution: use the SO_REUSEADDR socket option
	 */
	int enable = 1;
	if (setsockopt(sock,
		       SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		fatal_error("setsockopt(SO_REUSEADDR)");

	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* We bind to a port and turn this socket into a listening
	 * socket.
	 * */
	if (bind(sock,
		 (const struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
		fatal_error("bind()");

	if (listen(sock, QLEN_MAX) < 0)
		fatal_error("listen()");

	return sock;
}

/*
 * When Ctrl-C is pressed on the terminal, the shell sends our process SIGINT. This is the
 * handler for SIGINT, like we setup in main().
 *
 * We use the getrusage() call to get resource usage in terms of user and system times and
 * we print those. This helps us see how our server is performing.
 * */

void print_stats(int signo)
{
	double user, sys;
	struct rusage myusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0)
		fatal_error("getrusage()");

	user = (double)myusage.ru_utime.tv_sec +
	    myusage.ru_utime.tv_usec / 1000000.0;
	sys = (double)myusage.ru_stime.tv_sec +
	    myusage.ru_stime.tv_usec / 1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
	exit(0);
}

/*
 * Our main function is fairly simple. It sets up the listening socket and then
 * creates THREAD_COUNT number of threads to handle incoming requests. The main
 * thread does nothing, just calling pause() in an infinite loop.
 * */

int main(int argc, char *argv[])
{
	int server_port;

	signal(SIGINT, print_stats);

	if (argc > 1)
		server_port = atoi(argv[1]);
	else
		server_port = DEFAULT_SERVER_PORT;

	signal(SIGPIPE, SIG_IGN);
	strncpy(prg_name, argv[0], 127);

	server_socket = setup_listening_socket(server_port);
	printf("Pre-MT server listening on port %d\n", server_port);

	for (long i = 0; i < THREADS_COUNT; ++i) {
		create_thread(i);
	}

	for (;;)
		pause();
}
