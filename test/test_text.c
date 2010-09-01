/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/text_decomposer.h>
#include <gio/gio.h>

#include <gcutter.h>

static ChupaText *chupar;
static GInputStream *source;
static gchar *read_data;

void
setup(void)
{
    chupa_decomposer_load_modules();
    chupar = NULL;
    source = NULL;
    read_data = NULL;
}

void
teardown(void)
{
    if (chupar)
        g_object_unref(chupar);
    if (source)
        g_object_unref(source);
    if (read_data)
        g_free(read_data);
}

static void
text_decomposed(gpointer obj, gpointer arg, gpointer udata)
{
    GDataInputStream *data = G_DATA_INPUT_STREAM(arg);
    gsize length;

    read_data = g_data_input_stream_read_until(data, "", &length, NULL, NULL);
}

void
test_decompose_text_plain (void)
{
    static const char plain_text[] = "plain text\n";
    GInputStream *mem = g_memory_input_stream_new_from_data(plain_text, strlen(plain_text), NULL);

    source = mem;
    chupar = chupa_text_new();
    g_signal_connect(chupar, chupa_text_signal_decomposed, (GCallback)text_decomposed, NULL);
    chupa_text_feed(chupar, source);
    cut_assert_equal_string(plain_text, read_data);
}
