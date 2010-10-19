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

export BASE_DIR="$(dirname $0)"

if test -z "$MAKE"; then
    if which gmake > /dev/null; then
	MAKE="gmake"
    else
	MAKE="make"
    fi
fi
export MAKE

if test x"$NO_MAKE" != x"yes"; then
    $MAKE > /dev/null || exit 1
fi

if test -z "$TOP_BUILD_DIR"; then
    TOP_BUILD_DIR="$(${MAKE} -s echo-top-builddir)"
fi
TOP_BUILD_DIR="$(cd ${TOP_BUILD_DIR}; pwd)"

if test x"$NO_MAKE" != x"yes"; then
    $MAKE -C "$TOP_BUILD_DIR" > /dev/null || exit 1
fi

test_dir="$TOP_BUILD_DIR/test"
if test -z "$CUTTER"; then
    CUTTER="$(${MAKE} -s -C "$test_dir" echo-cutter)"
fi
export CUTTER

if test -z "$RUBY"; then
    RUBY="$(${MAKE} -s -C "$test_dir" echo-ruby)"
fi
export RUBY

if test -z "$TOP_SRC_DIR"; then
    TOP_SRC_DIR="$test_dir/$(${MAKE} -s -C "$test_dir" echo-top-srcdir)"
fi

: ${LIBTOOL:=${TOP_BUILD_DIR}/libtool}

CUTTER_ARGS=
CUTTER_WRAPPER=
if test x"$CUTTER_DEBUG" = x"yes"; then
    CUTTER_WRAPPER="${LIBTOOL} --mode=execute gdb --args"
    CUTTER_ARGS="--keep-opening-modules"
elif test x"$CUTTER_CHECK_LEAK" = x"yes"; then
    CUTTER_WRAPPER="${LIBTOOL} --mode=execute valgrind "
    CUTTER_WRAPPER="$CUTTER_WRAPPER --leak-check=full --show-reachable=yes -v"
    CUTTER_ARGS="--keep-opening-modules"
elif test x"$CUTTER_STRACE" = x"yes"; then
    CUTTER_WRAPPER="${LIBTOOL} --mode=execute strace -o strace.out "
fi

CUTTER_ARGS="$CUTTER_ARGS -s $BASE_DIR --exclude-directory fixtures"
if echo "$@" | grep -- --mode=analyze > /dev/null; then
    :
else
    CUTTER_ARGS="$CUTTER_ARGS --stream=xml --stream-log-directory $test_dir/log"
fi
if test x"$USE_GTK" = x"yes"; then
    CUTTER_ARGS="-u gtk $CUTTER_ARGS"
fi

ruby_src_dir=$TOP_SRC_DIR/module/ruby
export CHUPA_RUBY_LIBRARY_DIR=$CHUPATEXT_RUBYLIB:$ruby_src_dir/lib
export CHUPA_DECOMPOSER_DIR=$TOP_BUILD_DIR/module/.libs
export CHUPA_DESCRIPTIONS_DIR=$TOP_SRC_DIR/data/descriptions

no_test=1

cutter_result=0
if test "$NO_CUTTER" != "yes" -a -n "$CUTTER"; then
    $CUTTER_WRAPPER $CUTTER $CUTTER_ARGS "$@" "$test_dir"
    cutter_result=$?
    no_test=0
fi

ruby_result=0
if test "$NO_RUBY" != "yes" -a -n "$RUBY"; then
    : ${TEST_UNIT_MAX_DIFF_TARGET_STRING_SIZE:=30000}
    export TEST_UNIT_MAX_DIFF_TARGET_STRING_SIZE
    LD_LIBRARY_PATH="${TOP_BUILD_DIR}/chupatext/.libs:${LD_LIBRARY_PATH}"
    export LD_LIBRARY_PATH
    PKG_CONFIG_PATH="${TOP_BUILD_DIR}:${PKG_CONFIG_PATH}"
    export PKG_CONFIG_PATH
    RUBY_WRAPPER=
    if test x"$RUBY_DEBUG" = x"yes"; then
      RUBY_WRAPPER="${LIBTOOL} --mode=execute gdb --args"
    fi
    $RUBY_WRAPPER $RUBY $test_dir/run-test.rb $RUBY_TEST_ARGS "$@"
    ruby_result=$?
    no_test=0
fi

if [ $no_test = 0 -a $cutter_result = 0 -a $ruby_result = 0 ]; then
    exit 0
else
    exit 1
fi
