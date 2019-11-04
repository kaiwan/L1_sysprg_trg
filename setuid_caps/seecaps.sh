#!/bin/bash
# seecaps.sh
# 
# Quick Description:
# Show various files, looking for setuid-root and modern 'capabilities'.
# 
# Last Updated : 23jul17
# Created      : 23jul17  
# 
# Author:
# Kaiwan N Billimoria
# kaiwan -at- kaiwantech -dot- com
# kaiwanTECH
# 
# License:
# MIT License.
name=$(basename $0)
source ./common.sh || {
 echo "${name}: fatal: could not source common.sh , aborting..."
 exit 1
}
#[ $# -ne 1 ] && {
#  echo "Usage: ${name} "
#  exit 1
#}

########### Functions follow #######################

scanforcaps()
{
[ ! -d $1 ] && return 1
for fname in /$1/*
do
  getcap ${fname}
done
}

show_files_with_caps()
{
 ShowTitle "Scanning various folders for binaries with (modern) 'capabilities' embedded ..."
 aecho "Scanning /bin ..."
 scanforcaps /bin
 aecho "Scanning /usr/bin ..."
 scanforcaps /usr/bin
 aecho "Scanning /sbin ..."
 scanforcaps /sbin
 aecho "Scanning /usr/sbin ..."
 scanforcaps /usr/sbin
 aecho "Scanning /usr/local/bin ..."
 scanforcaps /usr/local/bin
 aecho "Scanning /usr/local/sbin ..."
 scanforcaps /usr/local/sbin
}

show_traditional_setuid_root()
{
 ShowTitle "Scanning various folders for traditional setuid-root binaries ..."
 aecho "Scanning /bin ..."
 ls -l /bin/  |grep "^-..s" |awk '$3=="root" {print $0}'
 aecho "Scanning /usr/bin ..."
 ls -l /usr/bin/  |grep "^-..s" |awk '$3=="root" {print $0}'
 aecho "Scanning /sbin ..."
 ls -l /sbin/  |grep "^-..s" |awk '$3=="root" {print $0}'
 aecho "Scanning /usr/sbin ..."
 ls -l /usr/sbin/  |grep "^-..s" |awk '$3=="root" {print $0}'
 aecho "Scanning /usr/local/bin ..."
 ls -l /usr/local/bin/  |grep "^-..s" |awk '$3=="root" {print $0}'
 aecho "Scanning /usr/local/sbin ..."
 ls -l /usr/local/sbin/  |grep "^-..s" |awk '$3=="root" {print $0}'
}

start()
{
show_traditional_setuid_root
show_files_with_caps
}

##### 'main' : execution starts here #####

echo -n "Distro: "
head -n1 /etc/issue
echo -n "kernel: "
uname -r
date

start
exit 0
