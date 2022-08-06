#!/bin/bash
# Part of our so-called 'better Makefile' support.
# Rationale:
# As a PUT - program under test - (like a server) can run continually, we will
# have to manually run the coverage utils.
# Attempting to abort the PUT with ^C (or [p]kill) causes the entire chain
# to abort. So, we need a separate script to run the stuff below; that's precisely
# the purpose behind the script covg.sh; run it when required...
#
# (c) kaiwanTECH, 2022

#------UPDATE--------
FNAME_C=cpudtl_tstsvr
#---as in the Makefile

GCOV=gcov
# USER CHECK: change as required
SRC_FILES=*.[ch]
GENINFO=geninfo
GENHTML=genhtml

${GCOV} ${SRC_FILES}
# generate .info from the .gcno and .gcda file(s) in .
${GENINFO} ./ -o ./${FNAME_C}.info
# generate HTML report in output directory lcov_html/
${GENHTML} ./${FNAME_C}.info --output-directory lcov_html/
echo "Display lcov html report: google-chrome lcov_html/index.html"
