/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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
    chupa_test_setup("word");
}

void
teardown(void)
{
    chupa_test_teardown();
}

void
test_95(void)
{
    cut_assert_equal_string("Sample of Word 95", decompose("95.doc"));
}

void
test_95_ja(void)
{
    cut_omit("Word 95 file with Japanese content is needed.");
    cut_assert_equal_string("Sample of Word 95\n"
                            "Word 95の日本語サンプル\n",
                            decompose("95_ja.doc"));
}

void
test_xp(void)
{
    cut_assert_equal_string("Sample of Word XP", decompose("xp.doc"));
}

void
test_xp_ja(void)
{
    cut_assert_equal_string("Sample of Word XP\n"
                            "Word XP の日本語サンプル\n",
                            decompose("xp_ja.doc"));
}

void
test_2007(void)
{
    cut_assert_equal_string("Sample of Word 2007", decompose("2007.docx"));
}

void
test_2007_ja(void)
{
    cut_assert_equal_string("Sample of Word 2007\n"
                            "Word 2007 の日本語サンプル\n",
                            decompose("2007_ja.docx"));
}
