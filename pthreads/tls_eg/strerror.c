/**********************************************************************\
*                Copyright (C) Michael Kerrisk, 2010.                  *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty. See the file COPYING.agpl-v3 for details.              *
\**********************************************************************/

/* strerror.c

   An implementation of strerror() that is not thread-safe.
*/
#define _GNU_SOURCE                 /* Get '_sys_nerr' and '_sys_errlist'
                                       declarations from <stdio.h> */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>                 /* Get declaration of strerror() */

#define MAX_ERROR_LEN 256           /* Maximum length of string
                                       returned by strerror() */

static char buf[MAX_ERROR_LEN];     /* Statically allocated return buffer */

char *
strerror_unsafe(int err)
{
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';          /* Ensure null termination */
    }

    return buf;
}

void * thrd_func(void * arg)
{
    char *str = strerror_unsafe(ENOMEM);
    printf("%s: &buf = %p ; str (%p) = %s\n", "thrd_func", buf, str, str);
    pthread_exit(0);
}

int main()
{
    pthread_t t1;
    char *str;
    int rc;

    str = strerror_unsafe(EPERM);
    printf("main:      &buf = %p ; str (%p) = %s\n", buf, str, str);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (pthread_create(&t1, &attr, thrd_func, NULL)) {
        perror("p_c failed"); exit(1);
    }

	rc = pthread_join(t1, NULL);
	if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(1);
	}
    return 0;
}
