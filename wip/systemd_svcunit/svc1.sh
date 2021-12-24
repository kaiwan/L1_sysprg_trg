#!/bin/bash
# Test : run svc1.service unit via systemd at boot
# this script gets invoked at boot...

# If all's running well, the echo (stdout/stderr) automatically goes to the syslog !
echo "$0: am running!"
/home/kaiwan/gitL1/wip/systemd_svcunit/primegen/primegen 100000
exit 0
