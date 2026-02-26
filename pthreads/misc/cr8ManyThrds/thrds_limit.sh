#!/bin/bash

cgroup_pid_limit()
{
    MY_CGROUP=$(cat /proc/self/cgroup) # | grep "cpu:" | cut -d: -f3)
    # rm chars preceding the frst '/' and print the rest of the string
    MY_CGROUP=$(echo "${MY_CGROUP}" | sed 's|^[^/]*||')
    grep . /sys/fs/cgroup/${MY_CGROUP}/pids.*
}

echo "ALL pid.* files in cgroup:"
cgroup_pid_limit
echo "
pids.max in cgroup:"
cgroup_pid_limit |grep "pids.max"