#!/bin/bash
# Ref:
# https://docs.kernel.org/admin-guide/sysctl/fs.html#proc-sys-fs-mqueue-posix-message-queues-filesystem

# Turn on unofficial Bash 'strict mode'! V useful
# "Convert many kinds of hidden, intermittent, or subtle bugs into immediate, glaringly obvious errors"
# ref: http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail

name=$(basename $0)

die()
{
echo >&2 "FATAL: $*"
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


#--- 'main'
echo "Maximum number of message queues allowed on the system"
runcmd "cat /proc/sys/fs/mqueue/queues_max"

echo "
Maximum number of messages in a queue on the system"
runcmd "cat /proc/sys/fs/mqueue/msg_max"

echo "
Maximum message size value (it is an attribute of every message queue, set during its creation)"
runcmd "cat /proc/sys/fs/mqueue/msgsize_max"

echo "
Default number of messages in a queue (when attr is set to NULL)"
runcmd "cat /proc/sys/fs/mqueue/msg_default"

echo "
Default message size value in a queue (when attr is set to NULL)"
runcmd "cat /proc/sys/fs/mqueue/msgsize_default"
