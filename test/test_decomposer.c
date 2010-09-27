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

static ChupaDecomposer *decomp;
static ChupaDispatcher *dispatcher;
static GInputStream *source;

void
setup(void)
{
    decomp = NULL;
    dispatcher = chupa_dispatcher_new();
}

void
teardown(void)
{
    if (dispatcher)
        g_object_unref(dispatcher);
    if (decomp)
        g_object_unref(decomp);
    if (source)
        g_object_unref(source);
}

void
test_search(void)
{
    decomp = chupa_dispatcher_dispatch(dispatcher, "text/plain");
    cut_assert_not_null(decomp);
}

#define CHUPA_TEST_TYPE_DUMMY_DECOMPOSER chupa_test_dummy_decomposer_get_type()
#define CHUPA_TEST_DUMMY_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER, ChupaTestDummyDecomposer)
#define CHUPA_TEST_DUMMY_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER, ChupaTestDummyDecomposerClass)
#define CHUPA_TEST_IS_DUMMY_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER)
#define CHUPA_TEST_IS_DUMMY_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER)
#define CHUPA_TEST_DUMMY_DECOMPOSER_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER, ChupaTestDummyDecomposerClass)

typedef struct _ChupaTestDummyDecomposer ChupaTestDummyDecomposer;
typedef struct _ChupaTestDummyDecomposerClass ChupaTestDummyDecomposerClass;
typedef struct _ChupaTestDummyDecomposerPrivate ChupaTestDummyDecomposerPrivate;

struct _ChupaTestDummyDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaTestDummyDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_test_dummy_decomposer_get_type(void);

G_DEFINE_TYPE(ChupaTestDummyDecomposer, chupa_test_dummy_decomposer, CHUPA_TYPE_DECOMPOSER)

static void
chupa_test_dummy_decomposer_init(ChupaTestDummyDecomposer *dec)
{
}

static void
chupa_test_dummy_decomposer_class_init(ChupaTestDummyDecomposerClass *klass)
{
}

void
test_register(void)
{
    static const char test_dummy_type[] = "application/x-chupa-test";
    return;
    chupa_decomposer_register(test_dummy_type, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER);
    chupa_decomposer_unregister(test_dummy_type, CHUPA_TEST_TYPE_DUMMY_DECOMPOSER);
}
