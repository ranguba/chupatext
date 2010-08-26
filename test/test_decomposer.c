/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>

static ChupaDecomposer *decomp;
static GInputStream *source;

void
setup(void)
{
    chupa_decomposer_load_modules();
    decomp = NULL;
}

void
teardown(void)
{
    if (decomp)
        g_object_unref(decomp);
    if (source)
        g_object_unref(source);
}

void
test_search(void)
{
    decomp = chupa_decomposer_search("text/plain");
    cut_assert_not_null(decomp);
    cut_assert_not_null(CHUPA_TEXT_DECOMPOSER(decomp));
}
