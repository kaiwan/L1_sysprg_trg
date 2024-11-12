/*
 * inet.h
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <pwd.h>

#include "convenient.h"

#define	SERV_IP		"127.0.0.1"	//INADDR_ANY
#define SERV_PORT		61008
#define MAXBUF		512
#define RE_MAXLEN	100+256+48+8+8+1024+4096+128
