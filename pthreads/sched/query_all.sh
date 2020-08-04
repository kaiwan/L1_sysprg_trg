#!/bin/sh
# Query the scheduling attributes (policy and RT (static) priority) of 
# all processes currently alive on the system.
# Just a simple wrapper around chrt.
# Also show the CPU affinity mask using taskset.
#
# Tip: Pipe this o/p to grep for FIFO / RR tasks..
# Also note that a multithreaded process shows up as several same PIDs
#  (resolve these using ps -eLf - to see actual PIDs of threads).

for p in `ps -A -To pid`
do
	chrt -p $p 2>/dev/null
	taskset -p $p
done

