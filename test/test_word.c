/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; encoding: utf-8 -*- */
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

void test_decompose_msword(void);
void test_decompose_msword_ja(void);
void test_decompose_msword_ja97(void);
void test_decompose_msword_ja95(void);

void
setup(void)
{
    chupa_test_setup();
}

void
teardown(void)
{
    chupa_test_teardown();
}

#define decompose_msword chupa_test_decompose_fixture

void
test_decompose_msword(void)
{
    cut_assert_equal_string("Sample\n\n", decompose_msword("sample.doc", NULL));
}

void
test_decompose_msword_ja(void)
{
    cut_assert_equal_string("Sample\nあいうえお\n", decompose_msword("sample_ja.doc", NULL));
}

void
test_decompose_msword_ja97(void)
{
    cut_assert_equal_string("Sample of Word97\n日本語サンプルWord97\n", decompose_msword("ja97.doc", NULL));
}

void
test_decompose_msword_ja95(void)
{
    cut_assert_equal_string("Sample of Word95\n日本語サンプルWord95\n", decompose_msword("ja95.doc", NULL));
}
