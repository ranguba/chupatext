#! /bin/sh
#
# Copyright (C) 2008-2010  Kouhei Sutou <kou@claer-code.com>
#
# This library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <http://www.gnu.org/licenses/>.

export BASE_DIR="`dirname $0 | sed 's|\/.$||'`"
case "$BASE_DIR" in
    */..) top_srcdir="$BASE_DIR/..";;
    */test) top_srcdir="`dirname "$BASE_DIR"`";;
esac
testdir=${BASE_DIR}
test $# = 0 || eval testdir='"${'$#'-.}"'
case "$testdir" in
    -* ) testdir=.; builddir="`pwd`";;
    . | .. | */. | */.. ) builddir="$testdir/..";;
    */* ) builddir=`expr "$testdir" : '\(.*\)/.*'`;;
    *) builddir="`cd $testdir && cd .. && pwd`";;
esac

if test x"$NO_MAKE" != x"yes"; then
    if which gmake > /dev/null; then
	MAKE=${MAKE:-"gmake"}
    else
	MAKE=${MAKE:-"make"}
    fi
    $MAKE -C "$builddir" > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="`${MAKE} -s -C "$testdir" echo-cutter`"
fi

CUTTER_ARGS=
CUTTER_WRAPPER=
if test x"$CUTTER_DEBUG" = x"yes"; then
    CUTTER_WRAPPER="${LIBTOOL-$builddir/libtool} --mode=execute gdb --args"
    CUTTER_ARGS="--keep-opening-modules"
elif test x"$CUTTER_CHECK_LEAK" = x"yes"; then
    CUTTER_WRAPPER="${LIBTOOL-$builddir/libtool} --mode=execute valgrind "
    CUTTER_WRAPPER="$CUTTER_WRAPPER --leak-check=full --show-reachable=yes -v"
    CUTTER_ARGS="--keep-opening-modules"
elif test x"$CUTTER_CHECK_STRACE" = x"yes"; then
    CUTTER_WRAPPER="${LIBTOOL-$builddir/libtool} --mode=execute strace -o strace.out "
fi

export CUTTER

CUTTER_ARGS="$CUTTER_ARGS -s $BASE_DIR --exclude-directory fixtures"
if echo "$@" | grep -- --mode=analyze > /dev/null; then
    :
else
    CUTTER_ARGS="$CUTTER_ARGS --stream=xml --stream-log-directory $testdir/log"
fi
if test x"$USE_GTK" = x"yes"; then
    CUTTER_ARGS="-u gtk $CUTTER_ARGS"
fi

ruby_dir=$top_srcdir/binding/ruby
CHUPATEXT_RUBYLIB=$CHUPATEXT_RUBYLIB:$ruby_dir/lib
CHUPATEXT_RUBYLIB=$CHUPATEXT_RUBYLIB:$ruby_dir/src/toolkit/.libs
CHUPATEXT_RUBYLIB=$CHUPATEXT_RUBYLIB:$ruby_dir/src/manager/.libs
ruby_glib2_dir=
for dir in $(for dir in $ruby_dir/glib-*; do echo $dir; done | sort -r); do
    if [ -f $dir/src/glib2.so ]; then
	ruby_glib2_dir=$dir
	break
    fi
done
if [ "$ruby_glib2_dir" != "" ]; then
    CHUPATEXT_RUBYLIB=$CHUPATEXT_RUBYLIB:$ruby_glib2_dir/src/lib
    CHUPATEXT_RUBYLIB=$CHUPATEXT_RUBYLIB:$ruby_glib2_dir/src
fi
RUBYLIB=$CHUPATEXT_RUBYLIB:$RUBYLIB
export CHUPATEXT_RUBYLIB
export RUBYLIB
export CHUPATEXT_CONFIGURATION_MODULE_DIR=$builddir/module/configuration/ruby/.libs
export CHUPATEXT_CONFIG_DIR=$top_srcdir/test/fixtures/configuration
export CHUPA_MODULE_DIR=$builddir/module/.libs

$CUTTER_WRAPPER $CUTTER $CUTTER_ARGS "$@" "$testdir"
