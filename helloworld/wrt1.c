#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main() {
	// ssize_t write(int fd, const void buf[.count], size_t count);

	ssize_t ret;

        ret = write(21, "HELLO\n", 6);
	if (ret < 0) {
		fprintf(stderr, "write() failed\n");
		perror("write() failed");
		exit(1);
	}
        exit(0);
}
