#!/bin/bash
# Frontend script to perf runs on the sched_pthrd (soft)-RT multithreaded app

source ./common.sh || {
 echo "$0: could not source ./common.sh , aborting..."
 exit 1
}

perf_sched()
{
ShowTitle "'perf sched' output follows:"
echo "Workload: $@"

# record data of workload
perf sched record $@

ShowTitle "'perf sched latency' output follows:"
perf sched latency
ShowTitle "'perf sched script' output follows:"
perf sched script
ShowTitle "'perf sched map' output follows:"
perf sched map
perf timechart # renders a output.svg
}

check_root_AIA

#---
# Fetch total # cpu cores on this system
nc=0
nc=$(getconf -a| grep _NPROCESSORS_CONF |awk '{print $2}')
[ ${nc} -eq 0 ] && nc=2
echo "# cpu cores: ${nc}"

# Turn off all cores except 0 (so that we don't see scheduling on them; also, the svg is then simpler..)
i=1
while [ ${i} -lt ${nc} ]
do
  echo 0 > /sys/devices/system/cpu/cpu${i}/online
  i=$((i+1))
done

#perf_sched taskset 8 ./sched_pthrd 5
perf_sched ./sched_pthrd 5

sync
# Turn cores back on
i=1
while [ ${i} -lt ${nc} ]
do
  echo 1 > /sys/devices/system/cpu/cpu${i}/online
  i=$((i+1))
done

