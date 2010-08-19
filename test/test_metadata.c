/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <chupatext/chupa_metadata.h>

#include <gcutter.h>

void test_new (void);
void test_get_first_value (void);
void test_get_values (void);

static ChupaMetadata *metadata, *metadata2;

void
setup (void)
{
    metadata = NULL;
    metadata2 = NULL;
}

void
teardown (void)
{
    if (metadata)
        g_object_unref(metadata);
    if (metadata2)
        g_object_unref(metadata2);
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
