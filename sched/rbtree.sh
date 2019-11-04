#!/bin/bash

[ ! -f task.sh ] && {
  echo "task.sh missing?"
  exit 1
}

trap 'pkill task.sh; exit 1' INT QUIT EXIT

./task.sh a > /dev/null &
./task.sh b > /dev/null &
./task.sh c > /dev/null &

while [ true ]
do
  grep -A9 'runnable tasks' /proc/sched_debug
  #cat /proc/sched_debug
  sleep 1
done

