#!/bin/bash
# Displays all SysV IPC Shared Memory segments, who created them and who used them last.
# 
# (c) Kaiwan NB
#
export IFS=$'\n'
first=0
i=1
for shmrec in $(cat /proc/sysvipc/shm)
do
	#echo $shmrec
	[ $first -eq 0 ] && {
		first=1
		continue
	}

	sz=$(echo $shmrec | awk '{print $4}')
	nattch=$(echo $shmrec | awk '{print $7}')
	echo "Shmem Segment ${i} :"
	echo "size: $sz bytes ($((${sz}/1024)) KB) nattch: ${nattch}"

	cpid=$(echo $shmrec | awk '{print $5}')
	echo -n "Creator PID: $cpid: "
	# Display, for that process, the owner and command-line
	ps -A -To pid,user,cmd | grep -w $cpid | uniq | head -n1 | awk '{printf("%s, %s\n", $2, $3)}'

	lpid=$(echo $shmrec | awk '{print $6}')
	echo -n "Last PID: $lpid:     "
	# Display, for that process, the owner and command-line
	ps -A -To pid,user,cmd | grep -w $lpid | uniq | head -n1 | awk '{printf("%s, %s\n", $2, $3)}'
	echo "
"
	let i=i+1
done


