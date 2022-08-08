/*
** selectserver.c -- a cheezy multiperson chat server
 * Src: Beej's Guide to Network Programming
 * http://beej.us/guide/bgnet/examples/selectserver.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "convenient.h"   // UPDATE on your box


static void	hexdump(unsigned char *srcbuf, unsigned int len)
{
	int i;

	for (i=0; i<len; i++) {
		printf("%02x ", srcbuf[i]);
	}
	printf("\n");
}


#define PORT "9034"   // port we're listening on

static int VERBOSE=1; // make 0 to make non-verbose

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; //AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//hints.ai_flags = AI_CANONNAME; //AI_PASSIVE;

	VP(VERBOSE, "Issuing getaddrinfo() syscall now...\n");
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for(p = ai; p != NULL; p = p->ai_next) {
		VP(VERBOSE, "Issuing socket() syscall now...\n");
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		VP(VERBOSE, "Issuing bind() syscall now: ip addr %x port %s...\n", 
			(unsigned long)get_in_addr((struct sockaddr*)&remoteaddr), PORT);

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}
		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
		/* If you have a socket that is listen() ing, you can check to see 
		   if there is a new connection by putting that socket's file 
		   descriptor in the readfds set 
		 */

    // main loop
    for(;;) {
		VP(VERBOSE, "Issuing select() syscall now...\n");
		/*
          int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
		 */		
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

		VP(VERBOSE, "select() unblocked...\n");
        /* run through the existing connections looking for data to read.
         * Note that select() can only tell us that some fd(s) is ready; 
		 * it itself does not read/write any data; we have to do that..
		 */
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
				VP(VERBOSE, "Got a connection (on read fd %d), issue accept() ...\n", i);
				/* If you have a socket that is listen() ing, you can check to see 
				   if there is a new connection by putting that socket's file 
				   descriptor in the readfds set */
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
		    		newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

			       if (newfd == -1) {
                        perror("accept");
                   } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: +++ new connection from %s on "
                            "socket %d\n",
						inet_ntop(remoteaddr.ss_family,
						  get_in_addr((struct sockaddr*)&remoteaddr),
						  remoteIP, INET6_ADDRSTRLEN), newfd);
                    }
                } else {
                    // handle data from a client
					VP(VERBOSE, "Issue recv() (on read fd %d)...\n", i);
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
						VP(VERBOSE, "Got data, broadcast it to all ...\n");
						printf("Data (%d bytes):In char::\n", nbytes);
						for (int k=0; k<nbytes; k++)
							printf("%c ", buf[k]);
						printf("\nIn hex::\n");

						hexdump(buf, nbytes);

                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}
