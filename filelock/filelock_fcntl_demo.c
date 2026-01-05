#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define FILE_NAME "locked_file.txt"

void lock_file(int fd) {
    struct flock lock;
    
    lock.l_type = F_WRLCK; // Write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // Lock the entire file
    lock.l_pid = getpid();
    
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error locking file");
        exit(EXIT_FAILURE);
    }
    printf("File locked by process %d\n", getpid());
}

void unlock_file(int fd) {
    struct flock lock;
    
    lock.l_type = F_UNLCK; // Unlock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // Unlock the entire file
    lock.l_pid = getpid();
    
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error unlocking file");
        exit(EXIT_FAILURE);
    }
    printf("File unlocked by process %d\n", getpid());
}

void write_to_file(int fd, const char *message) {
    if (write(fd, message, strlen(message)) == -1) {
        perror("Error writing to file");
        exit(EXIT_FAILURE);
    }
    printf("Data written to file: %s\n", message);
}

void read_from_file(int fd) {
    char buffer[256];
    lseek(fd, 0, SEEK_SET);  // Go to the start of the file
    
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error reading from file");
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';  // Null terminate the string
    printf("Data read from file: %s\n", buffer);
}

int main() {
    // Open the file in read/write mode, create it if it doesn't exist
    int fd = open(FILE_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Demonstrate file locking and writing
    lock_file(fd);
    write_to_file(fd, "Hello, this is a test of file locking!\n");

    // Simulate some other process by waiting
    printf("Sleeping for 5 seconds (simulate another process locking)...\n");
    sleep(5);

    unlock_file(fd);
    close(fd);

    // Reopen the file to demonstrate reading after unlocking
    fd = open(FILE_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    read_from_file(fd);
    close(fd);

    return 0;
}
