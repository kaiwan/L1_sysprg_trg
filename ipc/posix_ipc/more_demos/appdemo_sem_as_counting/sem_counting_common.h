#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>		/* For O_* constants */
#include <sys/mman.h>		// mmap()
#include <sys/stat.h>		/* For mode constants */
#include <mqueue.h>
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MQ_NAME "/mq_client_request"		// under /dev/mqueue
#define SHM_NAME "/myshm_counting_demo"		// under /dev/shm
#define SHM_MODE 0640
#define MEMSIZE 128
#define SEM_NAME "/appdemo_sem_counting"	// under /dev/shm

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
