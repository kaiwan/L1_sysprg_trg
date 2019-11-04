/*
* msg1_w.c
*
* Simple producer-consumer MQ demo app.
* Producer: writes messages into a Message Queue and exits.
*
*/

#include "mqapp.h"

int main( int argc, char **argv )
{
	int id=0,n=0;
	pid_t pid=getpid();
	struct msgbuf msg;

	/*
	   The presence in msgflg of the
       fields IPC_CREAT and IPC_EXCL plays the same role, with respect  to
       the  existence of the message queue, as the presence of O_CREAT and
       O_EXCL in the mode argument of the open(2) system  call:  i.e.  the
       msgget function fails if msgflg asserts both IPC_CREAT and IPC_EXCL
       and a message queue already exists for key.
	 */  
	if( (id=msgget( KEY_VAL, IPC_CREAT|IPC_EXCL|0600 ))== -1 ) {
		if (EEXIST == errno) {
			printf("MQ exists, try to fetch it...\n");
			/* Retry without attempting to create the MQ */
			id=msgget(KEY_VAL,0);
			if( -1 == id )
				perror("msgget failure"),exit(1);
		}
		else {
			perror("msgget failure"),exit(1);
		}
	}
	printf("%s [%d] : MQ id=%d\n", argv[0], pid, id);

	/* Send a message of Type 2 */
	msg.mtype=2;
	strcpy(msg.mtext, "Message of type 2");
	n=msgsnd(id, &msg, strlen(msg.mtext), 0);
	if( -1 == n )
		perror("msgsnd"), exit(1);
	printf("%s [%d] : type 2 msg sent\n", argv[0], pid);

	/* Send a message of Type 5 */
	msg.mtype=5;
	strcpy(msg.mtext, "Message of type 5!");
	n=msgsnd(id, &msg, strlen(msg.mtext), 0);
	if( -1 == n )
		perror("msgsnd"), exit(1);
	printf("%s [%d] : type 5 msg sent\n", argv[0], pid);

	exit(0);
}

