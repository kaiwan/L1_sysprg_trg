#!/bin/bash
# Script to modify the (better-single-prg) Makefile; it replaces the key
# FNAME_C variable, allowing the Makefile to build that program (along with
# all the usual fancy targets).
#
# If your purpose is to build an app with multiple source files (across
# multiple dirs), then pl use the makefile_templ/hdr_var/Makefile template).
name=$(basename $0)
[ $# -ne 1 ] && {
	echo "Usage: ${name} src_filename (without the .c)"
	exit 1
}
[[ "${1}" = *"."* ]] && {
	echo "Usage: ${name} src-filename ONLY (do NOT put any extension)."
	exit 1
}
[[ ! -f Makefile ]] && {
	echo "${name}: current Makefile not present? aborting..."
	exit 1
}
grep "^FNAME_C " Makefile || {
	echo "${name}: no FNAME_C in Makefile?"
	exit 1
}
fname_c=$1
sed -i "s/^FNAME_C :=.*$/FNAME_C := ${fname_c}/" Makefile
[[ $? -ne 0 ]] && {
	echo "${name}: sed: find/replace failed: $?"
	exit 1
}
grep -q "^FNAME_C := ${fname_c}" Makefile || {
	echo "${name}: grep failed: $?"
	exit 1
}
#make
