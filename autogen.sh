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

run ${AUTORECONF:-autoreconf} --install --force
