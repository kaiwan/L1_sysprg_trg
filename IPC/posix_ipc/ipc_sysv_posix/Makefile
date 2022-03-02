all: sysv posix
clean: sysv_clean posix_clean

sysv: \
sysv_shared_memory_private \
shmfile \
sysv_shared_memory_file_parent \
sysv_shared_memory_file_child \
semfile \
sysv_semaphore_file_parent \
sysv_semaphore_file_child \
msgfile \
sysv_message_queue_file_parent \
sysv_message_queue_file_child

sysv_shared_memory_private: sysv_shared_memory_private.c
	gcc -o sysv_shared_memory_private sysv_shared_memory_private.c -Wall

shmfile:
	touch shmfile

sysv_shared_memory_file_parent: sysv_shared_memory_file_parent.c
	gcc -o sysv_shared_memory_file_parent sysv_shared_memory_file_parent.c -Wall

sysv_shared_memory_file_child: sysv_shared_memory_file_child.c
	gcc -o sysv_shared_memory_file_child sysv_shared_memory_file_child.c -Wall

semfile:
	touch semfile

sysv_semaphore_file_parent: sysv_semaphore_file_parent.c
	gcc -o sysv_semaphore_file_parent sysv_semaphore_file_parent.c -Wall

sysv_semaphore_file_child: sysv_semaphore_file_child.c
	gcc -o sysv_semaphore_file_child sysv_semaphore_file_child.c -Wall

msgfile:
	touch msgfile

sysv_message_queue_file_parent: sysv_message_queue_file_parent.c
	gcc -o sysv_message_queue_file_parent sysv_message_queue_file_parent.c -Wall

sysv_message_queue_file_child: sysv_message_queue_file_child.c
	gcc -o sysv_message_queue_file_child sysv_message_queue_file_child.c -Wall

sysv_clean:
	rm -f sysv_shared_memory_private
	rm -f shmfile
	rm -f sysv_shared_memory_file_parent
	rm -f sysv_shared_memory_file_child
	rm -f semfile
	rm -f sysv_semaphore_file_parent
	rm -f sysv_semaphore_file_child
	rm -f msgfile
	rm -f sysv_message_queue_file_parent
	rm -f sysv_message_queue_file_child

posix: \
posix_shared_memory_parent \
posix_shared_memory_child \
posix_semaphore_parent \
posix_semaphore_child \
posix_message_queue_parent \
posix_message_queue_child

posix_shared_memory_parent: posix_shared_memory_parent.c
	gcc -o posix_shared_memory_parent posix_shared_memory_parent.c -Wall -lrt

posix_shared_memory_child: posix_shared_memory_child.c
	gcc -o posix_shared_memory_child posix_shared_memory_child.c -Wall -lrt

posix_semaphore_parent: posix_semaphore_parent.c
	gcc -pthread -o posix_semaphore_parent posix_semaphore_parent.c -Wall -lrt

posix_semaphore_child: posix_semaphore_child.c
	gcc -pthread -o posix_semaphore_child posix_semaphore_child.c -Wall -lrt

posix_message_queue_parent: posix_message_queue_parent.c
	gcc -o posix_message_queue_parent posix_message_queue_parent.c -Wall -lrt

posix_message_queue_child: posix_message_queue_child.c
	gcc -o posix_message_queue_child posix_message_queue_child.c -Wall -lrt

posix_clean:
	rm -f posix_shared_memory_parent
	rm -f posix_shared_memory_child
	rm -f posix_semaphore_parent
	rm -f posix_semaphore_child
	rm -f posix_message_queue_parent
	rm -f posix_message_queue_child
