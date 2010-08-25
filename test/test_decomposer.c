/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>

static ChupaDecomposerClass *decomp_class;
static GInputStream *source;

void
setup(void)
{
    chupa_decomposer_load_modules();
    decomp_class = NULL;
}

void
teardown(void)
{
    if (decomp_class)
        g_object_unref(decomp_class);
    if (source)
        g_object_unref(source);
}

void
test_new(void)
{
    static const char plain_text[] = "plain text\n";
    GInputStream *mem = g_memory_input_stream_new_from_data(plain_text, strlen(plain_text), NULL);

    source = mem;
    decomp_class = chupa_decomposer_search(mem);
    cut_assert_not_null(decomp_class);
    cut_assert_equal_uint(CHUPA_TYPE_TEXT_DECOMPOSER, G_TYPE_FROM_CLASS(decomp_class));
}
