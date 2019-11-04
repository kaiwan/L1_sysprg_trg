/* 
 * msg1_r.c
 * Kaiwan NB, kaiwanTECH.
 *
 * Simple producer-consumer MQ demo app.
 * Consumer: reads messages from an MQ and exits.
 */

#include "mqapp.h"
int READMAX=80;

static ssize_t msgrcv_r(int msqid, void *msgp, size_t msgsz, long msgtyp,
                      int msgflg)
{
	ssize_t n=0;
	while ( ((n=(msgrcv(msqid, msgp, msgsz, msgtyp, msgflg))) == -1) && (errno == EINTR)) ;
   /* !Bug! 
	* With this (earlier) code:
   while ( (n=(msgrcv(msqid, pMsg, msgsz, msgtyp, msgflg)) == -1) && (errno == EINTR)) ;

	* n was getting the value 0..
	* The bug turned out to be not nesting the inititalization of n in brackets properly!
	* (an additional pair of brackets were necessary!)  :-p
	*/
   //MSG("n=%d\n", n);
   return n;
}

int main( int argc, char **argv )
{
	int id, n=0;
	pid_t pid=getpid();
	struct msgbuf msg;

	/* We expect the MQ object to exist. */
	id=msgget(KEY_VAL, 0);
	if( -1 == id )
		perror("msgget"), exit(1);
	printf("%s [%d]: id=%d\n", argv[0], pid, id);

	/* Read first msg on the Q.
	 ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
                      int msgflg);
	*/
	if( (n=(msgrcv_r (id, &msg, READMAX, 0, 0)))== -1 ) 
		perror("msgrcv"), exit(1);
	printf("%s [%d]: Type 0 msg read of %d bytes; Type %ld: Msg: \"%.*s\"\n",
				 argv[0], pid, n, msg.mtype, n, msg.mtext);

	// Read msg type 5 on the Q..; msgtype, flags are last 2 params..
	if( (n=(msgrcv_r(id, &msg, READMAX, 5, 0)))== -1 ) 
		perror("msgrcv"), exit(1);
	printf("%s [%d]: Type 5 msg read of %d bytes; Type %ld: Msg: \"%.*s\"\n",
				 argv[0], pid, n, msg.mtype, n, msg.mtext);

	/* Read msg type 2 on the Q, but only for 10 bytes; type, flags are last
	 * 2 params..; note the MSG_NOERROR flag is used to get a truncated msg
	 * and avoid an error return
	 */
	printf("%s [%d]: Read Type 2 msg for 10 bytes\n", argv[0], pid);
	n=msgrcv_r(id, &msg, 10, 2, MSG_NOERROR);
	if( -1 == n )
		perror("msgrcv"), exit(1);
	printf("%s [%d]: Type 2 msg read for 10 bytes; %d bytes actually read; \
Type %ld: Msg: \"%.*s\"\n",
			 argv[0], pid, n, msg.mtype, n, msg.mtext);

	exit(0);
}

