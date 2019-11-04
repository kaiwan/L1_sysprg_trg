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
#include "convenient.h"

#define KEY_VAL		154326

struct msgbuf {
	long int mtype;		/* type of received/sent message */
    char mtext[80];		/* text of the message */
};

