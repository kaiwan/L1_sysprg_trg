#!/bin/sh
# sysvipc_lim.sh
#
# Print SysV IPC objects system limits from /proc fs
# 
SEP="-----------------------------------------------------------------------"
echo "Current System:: "
cat /etc/issue
echo "Current kernel:: "
uname -a

echo $SEP
echo "Message Queues: "
echo -n "MAX MQs that can exist system-wide: "
cat /proc/sys/kernel/msgmni

echo -n "MAX size of messages (bytes): "
cat /proc/sys/kernel/msgmax

echo -n "MAX size of a single MQ (bytes): "
cat /proc/sys/kernel/msgmnb

echo $SEP
echo "Semaphores: "
echo -n "MAX semaphores per array: "
cat /proc/sys/kernel/sem | cut -f1
echo -n "MAX semaphores systemwide: "
cat /proc/sys/kernel/sem | cut -f2
echo -n "MAX operations per semop call: "
cat /proc/sys/kernel/sem | cut -f3
echo -n "MAX number of semaphore arrays: "
cat /proc/sys/kernel/sem | cut -f4

echo $SEP
echo "Shared Memory: "
echo -n "MAX shmem segment size (bytes): "
cat /proc/sys/kernel/shmmax
echo -n "MAX number of shmem segments: "
cat /proc/sys/kernel/shmmni
echo $SEP

