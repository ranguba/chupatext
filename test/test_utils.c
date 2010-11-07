/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <chupatext/chupa_utils.h>

#include <gcutter.h>

void test_format_rfc2822 (void);

void
setup (void)
{
}

void
teardown (void)
{
}

void
test_format_rfc2822 (void)
{
    GTimeVal time;
    const gchar *rfc2822;

    time.tv_sec = 1289133579;
    time.tv_usec = 0;
    rfc2822 = cut_take_string(chupa_utils_format_rfc2822(&time));
    cut_assert_equal_string("Sun, 07 Nov 2010 12:39:39 +0000",
                            rfc2822);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
