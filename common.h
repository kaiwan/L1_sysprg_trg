/*
 * common.h
 * Brief Description:
 * This is the 'common' code that gets compiled into
 * all binary executables.
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/*--- 'Regular' headers ---*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
/*---*/

#define NON_FATAL    0
int handle_err(int fatal, const char *fmt, ...);
void Dprint(const char *fmt, ...);

#define WARN(warnmsg, args...) do {                   \
	handle_err(NON_FATAL, "!WARNING! %s:%s:%d: " warnmsg, \
	   __FILE__, __FUNCTION__, __LINE__, ##args); \
} while(0)
#define FATAL(errmsg, args...) do {                   \
	handle_err(EXIT_FAILURE, "FATAL:%s:%s:%d: " errmsg, \
	   __FILE__, __FUNCTION__, __LINE__, ##args); \
} while(0)

/*------------------------ DELAY_LOOP --------------------------------*/
static inline void beep(int what)
{
	char buf[1];
	buf[0] = what;
	(void)write(STDOUT_FILENO, buf, 1);
}

/* 
 * DELAY_LOOP macro
 * @val        : ASCII value to print
 * @loop_count : times to loop around
 */
#define HZ  250
#define DELAY_LOOP(val,loop_count)                                             \
{                                                                              \
	int c=0, m;                                                            \
	unsigned int for_index,inner_index;                                    \
	                                                                       \
	for(for_index=0;for_index<loop_count;for_index++) {                    \
		beep((val));                                                   \
		c++;                                                           \
		for(inner_index=0;inner_index<HZ*1000;inner_index++)           \
			for(m=0;m<50;m++);                                     \
		}                                                              \
	/*printf("c=%d\n",c);*/                                                \
}
/*------------------------------------------------------------------------*/

#endif     /* #ifndef _COMMON_H_ */
