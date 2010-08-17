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
void test_inspect (void);

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
    cut_assert_equal_string(NULL, chupa_metadata_get_first_value(metadata, "content-type"));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
