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
void test_body_x_sjis(void);
void test_body_utf8_without_content_type(void);
void test_body_euc_jp_without_content_type(void);
void test_body_x_sjis_without_content_type(void);
void test_title_ascii(void);
void test_title_utf8(void);
void test_title_euc_jp(void);
void test_title_utf8_without_content_type(void);
void test_title_euc_jp_without_content_type(void);
void test_title_koi8_r_between_xml_encoding_and_content_type(void);
void test_original_encoding_none(void);
void test_original_encoding_utf8(void);
void test_original_encoding_euc_jp(void);
void test_original_encoding_utf8_without_content_type(void);
void test_original_encoding_euc_jp_without_content_type(void);
void test_original_encoding_koi8_r_between_xml_encoding_and_content_type(void);

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
test_body_x_sjis(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose("x_sjis.html"));
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
test_body_x_sjis_without_content_type(void)
{
    cut_assert_equal_string("This is a sample.\nサンプル",
                            decompose("x_sjis_without_content_type.html"));
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
test_title_utf8_without_content_type(void)
{
    cut_assert_equal_string("サンプルHTMLファイル",
                            title("utf8_without_content_type.html"));
}

void
test_title_euc_jp_without_content_type(void)
{
    cut_assert_equal_string("サンプルHTMLファイル",
                            title("euc_jp_without_content_type.html"));
}

void
test_title_koi8_r_between_xml_encoding_and_content_type(void)
{
    cut_assert_equal_string("Философия Проекта GNU - "
                            "Фонд Свободного ПО (FSF)",
                            title("gnu_philosophy.ru.html"));
}

void
test_original_encoding_none(void)
{
    cut_assert_equal_string(NULL,
                            original_encoding("ascii_only.html"));
}

void
test_original_encoding_utf8(void)
{
    cut_assert_equal_string("utf-8",
                            original_encoding("utf8.html"));
}

void
test_original_encoding_euc_jp(void)
{
    cut_assert_equal_string("euc-jp",
                            original_encoding("euc_jp.html"));
}

void
test_original_encoding_utf8_without_content_type(void)
{
    cut_assert_equal_string("utf-8",
                            original_encoding("utf8_without_content_type.html"));
}

void
test_original_encoding_euc_jp_without_content_type(void)
{
    cut_assert_equal_string(
        "euc-jp",
        original_encoding("euc_jp_without_content_type.html"));
}

void
test_original_encoding_koi8_r_between_xml_encoding_and_content_type(void)
{
    cut_assert_equal_string("koi8-r",
                            original_encoding("gnu_philosophy.ru.html"));
}
