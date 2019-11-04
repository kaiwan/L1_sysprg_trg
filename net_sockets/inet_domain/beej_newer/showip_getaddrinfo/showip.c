/*
** showip.c -- show IP addresses for a host given on the command line
*/
#include <stdio.h>
#include <string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	if (argc != 2) {
		fprintf(stderr, "usage: showip hostname\n");
		return 1;
	}

	/*
	From the man page on getaddrinfo(3):
	...
	Given node and service, which identify an Internet host and a service, 
	getaddrinfo() returns one or more addrinfo structures, each of which
        contains an Internet address that can be specified in a call to bind(2) or 
	connect(2). The getaddrinfo() function combines the functionality provided 
	by the getservbyname(3) and getservbyport(3) functions into a single interface, 
	but unlike the latter functions, getaddrinfo() is reentrant and allows programs 
	to eliminate IPv4-versus-IPv6 dependencies.
	
	...

	 The hints argument points to an addrinfo structure that specifies criteria for 
	 selecting the socket address structures returned in the list pointed  to by res.  
	 If hints is not NULL it points to an addrinfo structure whose ai_family, 
	 ai_socktype, and ai_protocol specify criteria that limit the set of socket 
	 addresses returned by getaddrinfo(),
	...
	*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;	// AF_INET or AF_INET6 to force version
	hints.ai_socktype = 0; //SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

	/*
	...
 	The getaddrinfo() function allocates and initializes a linked list of addrinfo 
	structures, one for each network address that  matches  node and  service, 
	subject to any restrictions imposed by hints, and returns a pointer to the start 
	of the list in res.  The items in the linked list are linked by the ai_next field.
	...	
	*/
	printf("IP addresses for %s:\n\n", argv[1]);

	for (p = res; p != NULL; p = p->ai_next) {
		void *addr;
		in_port_t port;
		char *ipver;
		char *proto;

// get the pointer to the address itself,
// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) {	// IPv4
			struct sockaddr_in *ipv4 =
			    (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			port = ipv4->sin_port;
			ipver = "IPv4";
		} else {	// IPv6
			struct sockaddr_in6 *ipv6 =
			    (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			port = ipv6->sin6_port;
			ipver = "IPv6";
		}

// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		if (p->ai_protocol == 6)
			proto="tcp";
		else if (p->ai_protocol == 17)
			proto="udp";

		printf(" %s: %s %s", ipver, ipstr, proto);
		/* port always seems to be 0, don't show */
		if (port)
		  printf(" port %d\n", port);
		else
		  printf("\n");
		if (p->ai_canonname)
			printf(" canonname: %s\n", p->ai_canonname);
	}

	freeaddrinfo(res);	// free the linked list
	return 0;
}
