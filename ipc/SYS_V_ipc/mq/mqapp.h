/*
 * mqapp.h
 * Common header for simple MQ demo app - msg1_r.c & msg1_w.c
 *
 * GNU GPL v2
 * kaiwan.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define KEY_VAL		154326
#define MSGSZ_MAX		   180

/* Node on the list */
struct msgbuf {
    long int mtype;		/* Mandatory: type of received/sent message */
	/* ... you're free to put ANYTHING here ... */
    char mtext[MSGSZ_MAX];		/* text of the message */
};

