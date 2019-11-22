#!/bin/bash
# Kaiwan NB, kaiwanTECH
# License: MIT.
source ./common.sh || {
 echo "${name}: fatal: could not source common.sh , aborting..."
 exit 1
}
PRG_SR=pr_ids_setuidroot # SR = setuid root
PRG_CAP=pr_ids_capdumb

[ $(id -u) -eq 0 ] && {
  fecho "$0: do Not run as root!"
  exit 1
}
rm ${PRG_CAP} 2>/dev/null # rm stale instance
make  # build binary

aecho "1. Traditional setuid-root binary:"
ls -l ${PRG_SR}
echo " check with getcap:"
getcap -v ./${PRG_SR}
echo " run it:"
./${PRG_SR}

echo
aecho "2. Make a copy, strip it of setuid-root:"
cp ${PRG_SR} ${PRG_CAP}
chmod u-s ${PRG_CAP}
ls -l ${PRG_CAP}
echo " check with getcap:"
getcap -v ./${PRG_CAP}

echo
aecho "3. Apply caps with setcap:"
CAPS=cap_net_raw
setcap cap_net_raw+eip ./${PRG_CAP} || {
  echo " setcap fails as CAP_SETFCAP required; so we just sudo it now..."
  sudo setcap ${CAPS}+eip ./${PRG_CAP} || { 
    fecho "setcap: fatal error, abort.." 
    exit 1 
  }
  echo " setcap done: check with getcap:"
  getcap ./${PRG_CAP}
  echo " run it:"
  ./${PRG_CAP}
}
aecho "Done."
exit 0
