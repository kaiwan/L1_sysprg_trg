/*
 * convenient.h
 *
 * A few convenience routines..
 *
 * Author: Kaiwan N Billimoria <kaiwan@designergraphix.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __CONVENIENT_H__
#define __CONVENIENT_H__

#include <asm/param.h>	/* HZ */
#include <linux/sched.h>

typedef int Bool;

#ifndef __KERNEL__
static void handleErr(char * fn, Bool use_errno, Bool fatal_error)
{
	fprintf(stderr, "%s: error!\n", fn);
	if (use_errno)
		perror("failed.");
	if (fatal_error)
		exit (EXIT_FAILURE);
}
#endif

/*------------------------ MSG, QP ------------------------------------*/

#define PRINT_IRQCTX() {        \
  if (printk_ratelimit()) \
      printk(" %s: in_interrupt:%3s in_irq:%3s in_softirq:%3s in_serving_softirq:%3s. cpu #%2d preempt_count=%x\n",  \
        __func__, (in_interrupt()?"yes":"no"), (in_irq()?"yes":"no"), (in_softirq()?"yes":"no"),        \
        (in_serving_softirq()?"yes":"no"), smp_processor_id(), preempt_count());        \
}

#ifdef DEBUG
    #ifdef __KERNEL__
  	#define MSG(string, args...) \
		printk(KERN_INFO "%s:%d : " string, __FUNCTION__, __LINE__, ##args)
	#else
  	#define MSG(string, args...) \
		fprintf(stderr, "%s:%d : " string, __FUNCTION__, __LINE__, ##args)
	#endif

    #ifdef __KERNEL__
  	#define MSG_SHORT(string, args...) \
			printk(KERN_INFO string, ##args)
	#else
  	#define MSG_SHORT(string, args...) \
			fprintf(stderr, string, ##args)
	#endif
    \
	#define QP MSG("\n")
    \
    #ifdef __KERNEL__
	#define QPDS do { \
		 MSG("\n"); \
		 dump_stack(); \
	  } while(0)
	#define PRCS_CTX do { \
		 if (!in_interrupt()) { \
			MSG("prcs ctx: %s(%d)\n", current->comm, current->pid); \
		 } \
		 else { \
			MSG("irq ctx\n"); \
			PRINT_IRQCTX();   \
		} \
	  } while(0)
    #endif
#else
	#define MSG(string, args...)
	#define MSG_SHORT(string, args...)
	#define QP
	#define QPDS
#endif



/*------------------------ assert ---------------------------------------*/
#ifdef __KERNEL__
#define assert(expr) \
if (!(expr)) { \
 printk("********** Assertion [%s] failed! : %s:%s:%d **********\n", \
  #expr, __FILE__, __func__, __LINE__); \
}
#endif

/*------------------------ DELAY_LOOP --------------------------------*/
static inline void beep(int what)
{
  #ifdef __KERNEL__
	(void)printk(KERN_INFO "%c", (char)what );
  #else
	(void)printf("%c", (char)what );
  #endif
}

/* 
 * DELAY_LOOP macro
 * @val : ASCII value to print
 * @loop_count : times to loop around
 */
#define DELAY_LOOP(val,loop_count) \
{ \
	int c=0, m;\
	unsigned int for_index,inner_index; \
	\
	for(for_index=0;for_index<loop_count;for_index++) { \
		beep((val)); \
		c++;\
			for(inner_index=0;inner_index<HZ*1000*8;inner_index++) \
				for(m=0;m<50;m++); \
		} \
	/*printf("c=%d\n",c);*/\
}
/*------------------------------------------------------------------------*/

#ifdef __KERNEL__
/*------------ DELAY_SEC -------------------------*
 * Delays execution for n seconds.
 * MUST be called from process context.
 *------------------------------------------------*/
#define DELAY_SEC(val) \
{ \
	if (!in_interrupt()) {	\
		set_current_state (TASK_INTERRUPTIBLE); \
		schedule_timeout (val * HZ); \
	}	\
}
#endif

/* Get time difference between two struct timeval's
 * Credits: Arkaitz Jimenez
 * http://stackoverflow.com/questions/1444428/time-stamp-in-the-c-programming-language
 */
static int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);
/* Subtract the `struct timeval' values X and Y,
    storing the result in RESULT.
    Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
   /* Perform the carry for the later subtraction by updating y. */
   if (x->tv_usec < y->tv_usec) {
     int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
     y->tv_usec -= 1000000 * nsec;
     y->tv_sec += nsec;
   }
   if (x->tv_usec - y->tv_usec > 1000000) {
     int nsec = (x->tv_usec - y->tv_usec) / 1000000;
     y->tv_usec += 1000000 * nsec;
     y->tv_sec -= nsec;
   }

   /* Compute the time remaining to wait.
      tv_usec is certainly positive. */
   result->tv_sec = x->tv_sec - y->tv_sec;
   result->tv_usec = x->tv_usec - y->tv_usec;

   /* Return 1 if result is negative. */
   return x->tv_sec < y->tv_sec;
}

#endif
