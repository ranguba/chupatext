/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#include "chupa_test_util.h"

void test_95(void);
void test_95_ja(void);
void test_xp(void);
void test_xp_ja(void);
void test_2007(void);
void test_2007_ja(void);

void
setup(void)
{
    chupa_test_setup("excel");
}

void
teardown(void)
{
    chupa_test_teardown();
}

void
test_95(void)
{
    cut_assert_equal_string("Sample\tof\n"
                            "Excel\t95\n"
                            "Sheet\t2\n",
                            decompose("95.xls"));
}

void
test_95_ja(void)
{
    cut_omit("Excel 95 file with Japanese content is needed.");
    cut_assert_equal_string("Sample\tof\n"
                            "Excel\t95\n"
                            "日本語\tサンプル\n"
                            "Sheet\t2\n"
                            "シート\t2\n",
                            decompose("95_ja.xls"));
}

void
test_xp(void)
{
    cut_assert_equal_string("Sample\tof\n"
                            "Excel\tXP\n"
                            "Sheet\t2\n",
                            decompose("xp.xls"));
}

void
test_xp_ja(void)
{
    cut_assert_equal_string("Sample\tof\n"
                            "Excel\tXP\n"
                            "日本語\tサンプル\n"
                            "Sheet\t2\n"
                            "シート\t2\n",
                            decompose("xp_ja.xls"));
}

void
test_2007(void)
{
    cut_assert_equal_string("Sample\tof\n"
                            "Excel\t2007\n"
                            "Sheet\t2\n",
                            decompose("2007.xlsx"));
}

void
test_2007_ja(void)
{
    cut_assert_equal_string("Sample\tof\n"
                            "Excel\t2007\n"
                            "日本語\tサンプル\n"
                            "Sheet\t2\n"
                            "シート\t2\n",
                            decompose("2007_ja.xlsx"));
}
