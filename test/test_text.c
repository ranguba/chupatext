/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <chupatext/chupa_text_input.h>
#include <gio/gio.h>

#include <gcutter.h>

void
setup(void)
{
    chupa_decomposer_load_modules();
}

void
teardown(void)
{
}

#define TAKE_OBJECT(obj) gcut_take_object(G_OBJECT(obj))
#define TAKE_STRING(str) cut_take_string(str)

static const char *
decompose_text(const char *text, gsize size)
{
    ChupaText *chupar;
    ChupaTextInput *text_input;
    GInputStream *mem;

    TAKE_OBJECT(chupar = chupa_text_new());
    TAKE_OBJECT(mem = g_memory_input_stream_new_from_data(text, size, NULL));
    TAKE_OBJECT(text_input = chupa_text_input_new_from_stream(NULL, mem, NULL));
    return TAKE_STRING(chupa_text_decompose_all(chupar, text_input));
}

void
test_decompose_text_plain (void)
{
    static const char plain_text[] = "plain text\n";

    cut_assert_equal_string(plain_text, decompose_text(plain_text, sizeof(plain_text) - 1));
}
