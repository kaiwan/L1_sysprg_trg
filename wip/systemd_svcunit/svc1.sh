#!/bin/bash
# Test : run svc1.service unit via systemd at boot
# this script gets invoked at boot...

# If all's running well, the echo (stdout/stderr) automatically goes to the syslog !
echo "$0: am running!"
/home/letsdebug/L1_sysprg_trg/systemd_svcunit/primegen/primegen 100
exit 0
