#!/bin/bash

name=$(basename $0)
die()
{
echo >&2 "FATAL: $@"
exit 1
}
# runcmd
# Parameters
#   $1 ... : params are the command to run
runcmd()
{
	[ $# -eq 0 ] && return
	echo "$@"
	eval "$@"
}


#-- 'main' --
PRG=showip
[[ ! -f ./${PRG} ]] && die "Program ${PRG} not found (not built?)"
which nc >/dev/null || die "Program nc (netcat) is required, not found; pl install & retry"

[[ $# -ne 1 ]] && die "Usage: ${name} website
F.e. ${name} google.com"

RUN_PORTSCAN=1
RUN_BANNERGRAB=1
SITE=$1
ports=(ftp ssh telnet smtp dns tftp sftp ntp imap finger http snmp ptp)
len=${#ports[@]}
i=1

for p in ${ports[@]}
do
	echo "
---------------------------------------------------------------
$i of ${len} : $p
---------------------------------------------------------------"
	runcmd "./${PRG} ${SITE} ${p}"
	[[ $? -ne 0 ]] && {
		let i=i+1
		continue
	}

	# Port scan? with netcat:
	#  -v : verbose
	#  -w n : timeout after n sec
	# -z : Only scan for listening daemons, without sending any data to them.  Cannot be used together with -l
	echo ">>> attempting port scan for $p"
	[[ ${RUN_PORTSCAN} -eq 1 ]] && nc -v -w 2 -z ${SITE} ${p}
	[[ $? -ne 0 ]] && echo "-failed-"

	# Banner grab attempt
	echo ">>> attempting banner grab for $p"
	[[ ${RUN_BANNERGRAB} -eq 1 ]] && echo "QUIT" | nc -w 2 ${SITE} ${p} > .tmp
	[[ $? -ne 0 ]] && echo "-failed-" || head .tmp
	let i=i+1
done
exit 0
