#include ../Makefile.inc

#GEN_EXE = detached_attrib one_time_init prod_condvar prod_no_condvar \
	simple_thread strerror_test strerror_test_tsd \
	thread_cancel thread_cleanup thread_incr thread_incr_mutex \
	thread_multijoin

#LINUX_EXE = strerror_test_tls

EXE = thread_cancel thread_cleanup

all : ${EXE}

#allgen : ${GEN_EXE}

#CFLAGS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}
#LDLIBS = ${IMPL_LDLIBS} ${IMPL_THREAD_FLAGS}
CFLAGS = -Wall
CFLAGS_DBG = -Wall -DDEBUG -g -ggdb -O0
LDLIBS = -lpthread

thread_cancel: thread_cancel.o
	${CC} -o $@ thread_cancel.o \
		${CFLAGS} ${LDLIBS}

thread_cancel_dbg: thread_cancel.c
	${CC} -o thread_cancel_dbg thread_cancel.c ${CFLAGS_DBG} ${LDLIBS}

thread_cleanup: thread_cleanup.o
	${CC} -o $@ thread_cleanup.o \
		${CFLAGS} ${LDLIBS}

clean : 
	${RM} -f ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
