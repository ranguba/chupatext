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

void test_decompose_pdf(void);
void test_decompose_pdf_multi_pages(void);

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

#define decompose_pdf chupa_test_decompose_fixture

void
test_decompose_pdf(void)
{
    cut_assert_equal_string("sample\n", decompose_pdf("sample.pdf", NULL));
}

void
test_decompose_pdf_multi_pages(void)
{
    cut_assert_equal_string("page1\n"
                            "2ページ目\n"
                            "page3\n",
                            decompose_pdf("sample_multi_pages.pdf", NULL));
}
