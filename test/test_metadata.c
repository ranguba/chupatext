/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2010  Kouhei Sutou <kou@clear-code.com>
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

#include <chupatext/chupa_metadata.h>

#include <gcutter.h>

void test_new (void);
void test_set_value (void);
void test_remove (void);
void test_foreach (void);
void test_string (void);
void test_string_error (void);
void test_int (void);
void test_int_error (void);
void test_time_val (void);
void test_size (void);
void test_size_error (void);
void test_boolean (void);
void test_boolean_error (void);
void test_content_length (void);
void test_encoding (void);
void test_original_encoding (void);
void test_meta_ignore_time (void);

static ChupaMetadata *metadata, *metadata2;
static GString *result;
static GError *expected_error;
static GError *actual_error;

void
setup (void)
{
    metadata = NULL;
    metadata2 = NULL;
    result = NULL;
    expected_error = NULL;
    actual_error = NULL;
}

void
teardown (void)
{
    if (metadata)
        g_object_unref(metadata);
    if (metadata2)
        g_object_unref(metadata2);
    if (result)
        g_string_free(result, TRUE);
    if (expected_error)
        g_error_free(expected_error);
    if (actual_error)
        g_error_free(actual_error);
}

void
test_new (void)
{
    metadata = chupa_metadata_new();
    cut_assert_equal_int(0, chupa_metadata_size(metadata));
    cut_assert_equal_string(NULL,
                            chupa_metadata_get_string(metadata, "content-type",
                                                      NULL));
}

void
test_set_value (void)
{
    const gchar *key = "content-type";

    metadata = chupa_metadata_new();
    chupa_metadata_set_string(metadata, key, "text/plain");
    cut_assert_equal_string("text/plain",
                            chupa_metadata_get_string(metadata, key, NULL));
    chupa_metadata_set_string(metadata, key, "text/html");
    cut_assert_equal_string("text/html",
                            chupa_metadata_get_string(metadata, key, NULL));
}

void
test_remove (void)
{
    const gchar *key = "content-type";

    metadata = chupa_metadata_new();
    chupa_metadata_set_string(metadata, key, "text/plain");
    cut_assert_equal_string("text/plain",
                            chupa_metadata_get_string(metadata, key, NULL));
    chupa_metadata_remove(metadata, key);
    cut_assert_equal_string(NULL,
                            chupa_metadata_get_string(metadata, key, NULL));
}

static void
test_foreach_push(ChupaMetadataField *field, gpointer user_data)
{
    GString *string = user_data;

    g_string_append(string, chupa_metadata_field_name(field));
    g_string_append(string, ": ");
    g_string_append(string, chupa_metadata_field_value_string(field));
    g_string_append(string, "\n");
}

void
test_foreach (void)
{
    GString *result;

    metadata = chupa_metadata_new();
    chupa_metadata_set_string(metadata, "author", "Joe");
    chupa_metadata_set_string(metadata, "creator", "Bob");
    result = g_string_sized_new(32);
    chupa_metadata_foreach(metadata, test_foreach_push, result);
    cut_assert_equal_string("author: Joe\n"
                            "creator: Bob\n",
                            result->str);
}

void
test_string (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_set_string(metadata, "author", "Joe");
    cut_assert_equal_string("Joe",
                            chupa_metadata_get_string(metadata, "author", NULL));
}

void
test_string_error (void)
{
    const gchar *key;

    key = "author";
    metadata = chupa_metadata_new();
    expected_error = g_error_new(CHUPA_METADATA_ERROR,
                                 CHUPA_METADATA_ERROR_NOT_EXIST,
                                 "requested key doesn't exist in metadata: <%s>",
                                 key);

    cut_assert_equal_string(NULL,
                            chupa_metadata_get_string(metadata, key,
                                                      &actual_error));
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_int (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_set_int(metadata, "foo", 1);
    cut_assert_equal_int(1, chupa_metadata_get_int(metadata, "foo", NULL));
}

void
test_int_error (void)
{
    const gchar *key;

    key = "foobarbaz";
    metadata = chupa_metadata_new();
    expected_error = g_error_new(CHUPA_METADATA_ERROR,
                                 CHUPA_METADATA_ERROR_NOT_EXIST,
                                 "requested key doesn't exist in metadata: <%s>",
                                 "foobarbaz");

    cut_assert_equal_int(0, chupa_metadata_get_int(metadata, key, &actual_error));
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_time_val (void)
{
    GTimeVal expected_time_val, *actual_time_val;

    metadata = chupa_metadata_new();
    g_get_current_time(&expected_time_val);
    chupa_metadata_set_time_val(metadata, "last-modified", &expected_time_val);
    actual_time_val = chupa_metadata_get_time_val(metadata, "last-modified", NULL);

    cut_assert_not_null(actual_time_val);
    cut_assert_equal_int(expected_time_val.tv_sec, actual_time_val->tv_sec);
    cut_assert_equal_int(expected_time_val.tv_usec, actual_time_val->tv_usec);
}

void
test_size (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_set_size(metadata, "hoge", 1);
    cut_assert_equal_size(1, chupa_metadata_get_size(metadata, "hoge", NULL));
}

void
test_size_error (void)
{
    const gchar *key;

    key = "piyo";
    metadata = chupa_metadata_new();
    expected_error = g_error_new(CHUPA_METADATA_ERROR,
                                 CHUPA_METADATA_ERROR_NOT_EXIST,
                                 "requested key doesn't exist in metadata: <%s>",
                                 "piyo");

    cut_assert_equal_size(0, chupa_metadata_get_size(metadata, key, &actual_error));
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_boolean (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_set_boolean(metadata, "hungry-p", TRUE);
    cut_assert_true(chupa_metadata_get_boolean(metadata, "hungry-p", NULL));
}

void
test_boolean_error (void)
{
    const gchar *key = "hungry-p";

    metadata = chupa_metadata_new();
    expected_error = g_error_new(CHUPA_METADATA_ERROR,
                                 CHUPA_METADATA_ERROR_NOT_EXIST,
                                 "requested key doesn't exist in metadata: <%s>",
                                 key);

    cut_assert_false(chupa_metadata_get_boolean(metadata, key, &actual_error));
    gcut_assert_equal_error(expected_error, actual_error);
}

void
test_content_length (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_set_content_length(metadata, 29);
    cut_assert_equal_int(29, chupa_metadata_get_content_length(metadata));
}

void
test_encoding (void)
{
    metadata = chupa_metadata_new();
    cut_assert_equal_string(NULL, chupa_metadata_get_encoding(metadata));
    chupa_metadata_set_encoding(metadata, "UTF-8");
    cut_assert_equal_string("UTF-8", chupa_metadata_get_encoding(metadata));
}

void
test_original_encoding (void)
{
    metadata = chupa_metadata_new();
    cut_assert_equal_string(NULL,
                            chupa_metadata_get_original_encoding(metadata));
    chupa_metadata_set_original_encoding(metadata, "UTF-8");
    cut_assert_equal_string("UTF-8",
                            chupa_metadata_get_original_encoding(metadata));
}

void
test_meta_ignore_time (void)
{
    metadata = chupa_metadata_new();
    cut_assert_false(chupa_metadata_get_meta_ignore_time(metadata));
    chupa_metadata_set_meta_ignore_time(metadata, TRUE);
    cut_assert_true(chupa_metadata_get_meta_ignore_time(metadata));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
