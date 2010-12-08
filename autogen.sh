#!/bin/sh

run()
{
    $@
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

cutter_svn_repository=https://cutter.svn.sourceforge.net/svnroot/cutter
run svn export --force $cutter_svn_repository/cutter/trunk/misc

mkdir -p m4
run intltoolize --copy --force
run aclocal --output acinclude.m4 ${ACLOCAL_ARGS}
run ${AUTORECONF:-autoreconf} --install --force
