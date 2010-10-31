/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

void test_decompose_html(void);
void test_decompose_html_title(void);
void test_decompose_html_utf8(void);
void test_decompose_html_euc_jp(void);
void test_decompose_html_charset_none(void);
void test_decompose_html_charset_utf8(void);
void test_decompose_html_charset_euc_jp(void);

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

#define decompose_fixture chupa_test_decompose_fixture

void
test_decompose_html(void)
{
    cut_assert_equal_string("This is a sample.",
                            decompose_fixture("sample.html"));
}

void
test_decompose_html_title(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("sample.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("Sample HTML File",
                            chupa_metadata_get_first_value(metadata, "title"));
}

void
test_decompose_html_utf8(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose_fixture("sample_utf8.html"));
}

void
test_decompose_html_euc_jp(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose_fixture("sample_euc_jp.html"));
}

void
test_decompose_html_charset_none(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("sample.html");
    cut_assert_not_null(metadata);
    cut_assert_null(chupa_metadata_get_first_value(metadata, "charset"));
}

void
test_decompose_html_charset_utf8(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("sample_utf8.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("utf-8",
                            chupa_metadata_get_first_value(metadata, "charset"));
}

void
test_decompose_html_charset_euc_jp(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("sample_euc_jp.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("euc-jp",
                            chupa_metadata_get_first_value(metadata, "charset"));
}
