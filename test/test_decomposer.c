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

#include <chupatext/chupa_dispatcher.h>
#include <gio/gio.h>

#include <gcutter.h>

void test_search(void);
void test_register(void);

static ChupaDecomposer *decomposer;
static ChupaDispatcher *dispatcher;
static GInputStream *source;

void
setup(void)
{
    decomposer = NULL;
    dispatcher = chupa_dispatcher_new();
}

void
teardown(void)
{
    if (dispatcher)
        g_object_unref(dispatcher);
    if (decomposer)
        g_object_unref(decomposer);
    if (source)
        g_object_unref(source);
}

void
test_search(void)
{
    decomposer = chupa_dispatcher_dispatch(dispatcher, "text/plain");
    cut_assert_not_null(decomposer);
}
