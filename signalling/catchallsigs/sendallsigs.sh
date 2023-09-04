#!/bin/bash
# (c) kaiwanTECH
# MIT.

# Send all signals to a given process
# Meant to be run in conjunction with the 'catchall' program

[[ $# -ne 1 ]] && {
	echo "Usage: ${name} PID-of-process-to-send-all-signals-to"
	exit 1
}

PID=$1
grep "^Sig" /proc/${PID}/status
echo

sig=1
while [[ ${sig} -le 64 ]]
do
	# Send all signals EXCEPT for signals SIGKILL, SIGSTOP, 32 and 33 (reserved by Pthreads)
	if [[ ${sig} -ne 9 ]] && [[ ${sig} -ne 19 ]] && \
	   [[ ${sig} -ne 32 ]] && [[ ${sig} -ne 33 ]] ; then
 		  kill -${sig} ${PID} || echo "Sending signal # ${sig} failed" #&& echo -n "."
	fi
	sig=$((sig+1))
done
