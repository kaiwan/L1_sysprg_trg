/* 
*  mt_sfingr.c
*  Internet domain streams socket server; 
*  multi-threaded server
*/
#include "inet.h"
#include <pthread.h>
#include <pwd.h>

#define	QLENGTH	5

int verbose = 0;
char *prg;

int thrd_err_exit(char *prg, char *err, int exitcode)
{
	char *err_str, exitstr[4];

	err_str = (char *)malloc(300);
	if (!err_str)
		printf("\n%s: malloc failed for err_str..", prg);

	snprintf(err_str, 300, "%s: %s", prg, err);
	perror(err_str);
	free(err_str);

	snprintf(exitstr, 4, "%d", exitcode);
	pthread_exit(exitstr);
}				// thrd_err_exit()

int err_exit(char *prg, char *err, int exitcode)
{
	char *err_str;

	err_str = (char *)malloc(strlen(prg) + strlen(err) + 2);
	if (!err_str)
		printf("\n%s: malloc failed for err_str..", prg);

	snprintf(err_str, strlen(prg) + strlen(err) + 2, "%s: %s", prg, err);
	perror(err_str);
	free(err_str);

	exit(exitcode);
}				// err_exit()

// Child thread p2 processes this function
void * process_client(void *arg)
{
	char *username, *reply;
	int sd, n;
	struct passwd *p;
#define PBUFLEN 8192
	char pbuf[PBUFLEN];

	sd = (long)arg;
	if (verbose)
		printf("%s: child thread (%d) on socket sd %d\n",
		       prg, getpid(), sd);

#ifdef DBG
	printf("_child thread: in process_client()..arg=%d\n", (int)arg);
	fflush(stdout);
#endif
	username = malloc(MAXBUF);
	if (!username) {
		printf("\n%s: no memory for username buffer", prg);
		pthread_exit("enomem");
	}
	// Read username from client
	if ((n = read(sd, username, MAXBUF)) == -1)
		thrd_err_exit(prg, "socket read error", 6);
	username[n] = '\0';
#ifdef DBG
	printf("_child thread: af read()..n=%d\n", n);
	fflush(stdout);
#endif

	reply = (char *)malloc(MAXBUF);
	if (!reply) {
		printf("\n%s: no memory for reply buffer", prg);
		free(username);
		pthread_exit("1");
	}
	// TODO !!! MT-Unsafe! replace with getpwnam_r() !
	//p = getpwnam(username);
	/*int getpwnam_r(const char *name, struct passwd *pwd,
	   char *buf, size_t buflen, struct passwd **result);
	 */
	p = malloc(sizeof(struct passwd));
	if (!p) {
		printf("\n%s: no memory for passwd structure", prg);
		free(username);
		free(reply);
		pthread_exit("1");
	}
	if (getpwnam_r(username, p, pbuf, PBUFLEN, &p) || (p == NULL)) {
		snprintf(reply, MAXBUF-1, "\n%s is unable to get info for %s\n", prg,
			username);
		if ((write(sd, reply, strlen(reply))) == -1)
			thrd_err_exit(prg, "socket write error", 6);
		close(sd);
		free(p);
		free(username);
		free(reply);
		pthread_exit("1");
	}
	// Got the info; send it to the client..
	snprintf(reply, MAXBUF-1,
	"\tUsername: %s\n\t(encrypted) password: %s\n\
\tUID: %d\n\tGID: %d\n\tComment/FullName: %s\n\tHome dir: %s\n\tShell: %s\n",
	p->pw_name, p->pw_passwd, p->pw_uid, p->pw_gid, p->pw_gecos, p->pw_dir, p->pw_shell);

	if ((write(sd, reply, strlen(reply))) == -1)
		thrd_err_exit(prg, "socket write error", 6);

	free(p);
	free(username);
	free(reply);

#ifdef TESTING
	sleep(5);
#endif

	/* This is a thread: so normally we don't close the socket!
	 * But the socket in question is a new socket allocated by the accept()
	 * each time a thread is created; so here it's better to close the socket
	 * as otherwise the file table will start getting filled v rapidly.
	 */
	close(sd);
	pthread_exit("0");
}				// process_client()


int main(int argc, char *argv[])
{
	int sd, newsd, r;
	unsigned int clilen;
	pthread_t p2;
	struct sockaddr_in svr_addr, cli_addr;

	if ((argc > 1) && (strcmp(argv[1], "-v") == 0))
		verbose = 1;
	prg = argv[0];

	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		err_exit(argv[0], "socket creation error", 1);
	if (verbose)
		printf("%s: tcp socket created\n", argv[0]);

	// Initialize server's address & bind it
	svr_addr.sin_family = PF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;
	//svr_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	svr_addr.sin_port = htons(SERV_PORT);

	if (bind(sd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1)
		err_exit(argv[0], "socket bind error", 2);
	if (verbose)
		printf("%s: bind done at IP %s port %d\n", argv[0], SERV_IP,
		       SERV_PORT);

	if (listen(sd, QLENGTH) == -1)
		err_exit(argv[0], "socket listen error", 3);
	if (verbose)
		printf("%s: listen q set up\n", argv[0]);

	while (1) {
		if (verbose)
			printf("%s: pid %d blocking on accept()..\n",
			       argv[0], getpid());
		fflush(stdout);

		clilen = sizeof(cli_addr);
		if ((newsd = accept(sd, (struct sockaddr *)&cli_addr, &clilen)) == -1)	// server blocks here..
			err_exit(argv[0], "socket accept error", 5);

		printf("\nServer %s: client connected now from IP %s \
port # %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		fflush(stdout);

		// Client connected; create child thread to handle
		// the client request
		r = pthread_create(&p2,	// thread id
				   NULL,	// thread attributes (use default)
				   process_client,	// function to execute
				   (void *)newsd);	// argument to function
		if (r)
			perror("pthread creation");

		if (pthread_detach(p2)) {
			perror("svr: pthread detach failed");
			break;
		}
		if (verbose)
			printf("%s: child thread created & detached\n",
			       argv[0]);

		// Parent thread
		/* close( newsd );      *//* this time do *not* close newsd as the threads
		 * really share the open files
		 */
		fflush(stdout);

	}			// while      

}				// main()

// end mt_sfingr.c
