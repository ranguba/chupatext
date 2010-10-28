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
void test_get_first_value (void);
void test_get_values (void);
void test_replace_value (void);
void test_update_value (void);
void test_remove_value (void);
void test_foreach (void);
void test_int (void);
void test_int_error (void);

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
    cut_assert_equal_string(NULL, chupa_metadata_get_first_value(metadata, "content-type"));
}

void
test_get_first_value (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "content-type", "text/plain");
    cut_assert_equal_string("text/plain", chupa_metadata_get_first_value(metadata, "content-type"));
}

void
test_get_values (void)
{
    GList *values;

    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "content-type", "text/plain");
    chupa_metadata_add_value(metadata, "content-type", "text/html");
    values = chupa_metadata_get_values(metadata, "content-type");
    cut_assert_equal_int(2, g_list_length(values));
    cut_assert_equal_string("text/plain", values->data);
    cut_assert_equal_string("text/html", values->next->data);
}

void
test_replace_value (void)
{
    const gchar *key = "content-type";
    GList *values;

    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, key, "text/plain");
    cut_assert_equal_string("text/plain", chupa_metadata_get_first_value(metadata, key));
    chupa_metadata_replace_value(metadata, key, "text/html");
    values = chupa_metadata_get_values(metadata, key);
    cut_assert_equal_int(1, g_list_length(values));
    cut_assert_equal_string("text/html", values->data);
}

void
test_update_value (void)
{
    const gchar *key1 = "content-type";
    const gchar *key2 = "content-length";

    metadata = chupa_metadata_new();
    metadata2 = chupa_metadata_new();
    chupa_metadata_add_value(metadata, key1, "text/plain");
    chupa_metadata_add_value(metadata2, key2, "100");
    cut_assert_null(chupa_metadata_get_first_value(metadata, key2));
    chupa_metadata_update(metadata, metadata2);
    cut_assert_equal_string("100", chupa_metadata_get_first_value(metadata, key2));
}

void
test_remove_value (void)
{
    const gchar *key = "content-type";

    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, key, "text/plain");
    cut_assert_equal_string("text/plain", chupa_metadata_get_first_value(metadata, key));
    chupa_metadata_replace_value(metadata, key, NULL);
    cut_assert_null(chupa_metadata_get_values(metadata, key));
}

struct foreach_test_args {
    GString *string;
    const gchar *key;
};

static void
test_foreach_push_values(gpointer value, gpointer user_data)
{
    struct foreach_test_args *args = user_data;
    g_string_append(args->string, args->key);
    g_string_append(args->string, ": ");
    g_string_append(args->string, value);
    g_string_append_c(args->string, '\n');
}

static void
test_foreach_push(gpointer key, gpointer value_list, gpointer user_data)
{
    struct foreach_test_args args;
    args.string = user_data;
    args.key = key;
    g_list_foreach(value_list, test_foreach_push_values, (gpointer)&args);
}

void
test_foreach (void)
{
    GString *result;

    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "foo", "bar");
    chupa_metadata_add_value(metadata, "bar", "qux");
    chupa_metadata_add_value(metadata, "foo", "zot");
    result = g_string_sized_new(32);
    chupa_metadata_foreach(metadata, test_foreach_push, result);
    cut_assert_equal_string("foo: bar\n"
                            "foo: zot\n"
                            "bar: qux\n",
                            result->str);
}

void
test_int (void)
{
    metadata = chupa_metadata_new();
    chupa_metadata_add_int(metadata, "foo", 1);
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
