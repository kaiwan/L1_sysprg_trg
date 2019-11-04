/*
* libpk.h
*
* Small collection of common routines for pthread programs;
* 
* Routines/macros:
*
*  1. DELAY_LOOP
*  2. MSG
* 
* Last updated: Oct 2006
* kaiwan
*
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <asm/param.h>		// HZ

static inline void beep(int what)
{
        char buf[2];
        buf[0]=(char)(what);
        buf[1]='\0';
        write(STDOUT_FILENO, buf, 1 );
}

/* 
 * @val : ASCII value to print
 * @loop_count : times to loop around
 */
#define DELAY_LOOP(val,loop_count) \
{ \
        int c=0;\
        unsigned int for_index,inner_index; \
        double x; \
        for(for_index=0;for_index<loop_count;for_index++) { \
                beep((val)); \
                c++;\
                for(inner_index=0;inner_index<HZ*100000;inner_index++) \
                        x=(inner_index%2)*((22/7)%3); \
        } \
        /*printf("c=%d\n",c);*/\
}

/* The "MSG" macro, for printing when in debugging mode */
#ifdef DEBUG
  	#define MSG(string, args...) fprintf(stderr, "%s:%s : " string, __FILE__, __FUNCTION__, ##args)
#else
	#define MSG(string, args...)
#endif


#if 0
static inline void beep(int how, unsigned char * what)
{
	if( how )	// beep!
		write(STDOUT_FILENO, "\007", 1 );
	else		// write a character *"what"
		write(STDOUT_FILENO, what, 1 );
}

#define DELAY_STEP		   20240/HZ
#define DELAY_LOOP(how,val) \
{ \
	unsigned int i,k; \
	double x; \
	for(i=0;i<(DELAY_STEP);i++) { \
		beep((how), (val)); \
		for(k=0;k<HZ*100000;k++) \
			x=(k%2)*((22/7)%3); \
	} \
}	
#endif

