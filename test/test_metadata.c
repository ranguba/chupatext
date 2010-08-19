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

static ChupaMetadata *metadata;

void
setup (void)
{
    metadata = NULL;
}

void
teardown (void)
{
    if (metadata)
        g_object_unref(metadata);
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

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
