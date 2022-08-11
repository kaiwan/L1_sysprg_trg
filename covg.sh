#!/bin/bash
# Part of our so-called single-prg-build 'better Makefile' support.
# Rationale:
# As a PUT - program under test - (like a server) can run continually, we will
# have to manually run the coverage utils.
# Attempting to abort the PUT with ^C (or [p]kill) causes the entire chain
# to abort. So, we need a separate script to run the stuff below; that's precisely
# the purpose behind the script covg.sh; run it when required...
#
# If your purpose is to build an app with multiple source files (across
# multiple dirs), then pl use the makefile_templ/hdr_var/Makefile template).
# (c) kaiwanTECH, 2022
name=$(basename $0)
[ $# -ne 1 ] && {
	echo "Usage: ${name} src_filename (without the .c)"
	exit 1
}
[[ "${1}" = *"."* ]] && {
	echo "Usage: ${name} src-filename ONLY (do NOT put any extension)."
	exit 1
}
[ ! -f Makefile ] && {
	echo "${name}: Makefile not present? aborting..."
	exit 1
}
fname_c=$1
sed -i "s/^FNAME_C.*=.*$/FNAME_C := ${fname_c}/" Makefile
[ $? -ne 0 ] && {
	echo "${name}: sed: find/replace failed(1)..."
	exit 1
}
grep -q "^FNAME_C.*= ${fname_c}" Makefile || {
	echo "${name}: sed: find/replace failed(2)..."
	exit 1
}

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
