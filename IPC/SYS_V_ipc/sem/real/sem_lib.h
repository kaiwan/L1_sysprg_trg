/*
 * sem_lib.h
 *
 * From "UNIX Systems Programming" by Robbins & Robbins.
 * All rights with original authours.
 *
 */
#ifndef __SEMLIB_H__
#define __SEMLIB_H__

#ifndef _ASSERT_H
#include <assert.h>
#endif
/*
 * function : i n i t e l e m e n t
 *
 * Initializes the value of the specified semaphore element.
 * 
 * Parameters:
 *	@semid : semaphore ID
 *  @semnum: particular semaphore in this set
 *  @semvalue : value to set @semnum to
 * 
 * Returns:
 *  0 on success, -1 on failure, and errno set.
 */
int initelement (int semid, int semnum, int semvalue)
{
	union semnum {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	} arg;

	arg.val = semvalue;
	return semctl (semid, semnum, SETVAL, arg);
}


/*
 * function : s e t s e m b u f
 *
 * Initialized the struct sembuf structure members in an implementation-
 * -independent manner.
 *
 * Parameters:
 *	@semid : semaphore ID
 *
 * Returns:
 *  Nothing.
 */
void setsembuf (struct sembuf *s, int num, int op, int flg)
{
	assert(s!=NULL);

	s->sem_num = (short)num;
	s->sem_op  = (short)op;
	s->sem_flg = (short)flg;
	return;
}


/*
 * function : r _ s e m o p
 *
 * Wrapper around semop to provide an auto-restartable semop after a signal.
 *
 * Note: this actually belongs in a "restart library" header..
 */
int r_semop (int semid, struct sembuf *sops, size_t nsops)
{
	while (semop (semid, sops, nsops) == -1) {
		if (errno != EINTR)
			return -1;
	}
	return 0;
}

/* function : r e m o v e s e m
 *
 * Deletes the semaphore specified.
 *
 * Parameters:
 *	@semid : semaphore ID
 *
 * Returns:
 *  0 on success, -1 on failure, and errno set.
 */
int removesem (int semid)
{
	return semctl (semid, 0, IPC_RMID);
}

#endif
