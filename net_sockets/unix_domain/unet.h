/* unet.h
 * Common header file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define	SOCKET_NAME    "/run/sockname.sk"
