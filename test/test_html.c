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

void test_body_ascii(void);
void test_body_utf8(void);
void test_body_euc_jp(void);
void test_body_utf8_without_content_type(void);
void test_body_euc_jp_without_content_type(void);
void test_title_ascii(void);
void test_title_utf8(void);
void test_title_euc_jp(void);
void test_charset_none(void);
void test_charset_utf8(void);
void test_charset_euc_jp(void);
void test_non_ascii_text_before_charset(void);

void
setup(void)
{
    chupa_test_setup("html");
}

void
teardown(void)
{
    chupa_test_teardown();
}

void
test_body_ascii(void)
{
    cut_assert_equal_string("This is a sample.",
                            decompose("ascii_only.html"));
}

void
test_body_utf8(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose("utf8.html"));
}

void
test_body_euc_jp(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose("euc_jp.html"));
}

void
test_body_utf8_without_content_type(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose("utf8_without_content_type.html"));
}

void
test_body_euc_jp_without_content_type(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose("euc_jp_without_content_type.html"));
}

void
test_title_ascii(void)
{
    cut_assert_equal_string("Sample HTML File",
                            title("ascii_only.html"));
}

void
test_title_utf8(void)
{
    cut_assert_equal_string("サンプルHTMLファイル",
                            title("utf8.html"));
}

void
test_title_euc_jp(void)
{
    cut_assert_equal_string("サンプルHTMLファイル",
                            title("euc_jp.html"));
}

void
test_charset_none(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("ascii_only.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string(NULL,
                            chupa_metadata_get_original_encoding(metadata));
}

void
test_charset_utf8(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("utf8.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("utf-8",
                            chupa_metadata_get_original_encoding(metadata));
}

void
test_charset_euc_jp(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("euc_jp.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("euc-jp",
                            chupa_metadata_get_original_encoding(metadata));
}

void
test_non_ascii_text_before_charset(void)
{
    ChupaMetadata *metadata;

    metadata = chupa_test_metadata_fixture("gnu_philosophy.ru.html");
    cut_assert_not_null(metadata);
    cut_assert_equal_string("Философия Проекта GNU - "
                            "Фонд Свободного ПО (FSF)",
                            chupa_metadata_get_string(metadata, "title", NULL));
}
