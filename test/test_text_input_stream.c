/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_text_input_stream.h>

#include <gcutter.h>

void test_new (void);

static ChupaTextInputStream *stream;

void
setup (void)
{
    stream = NULL;
}

void
teardown (void)
{
    if (stream)
        g_object_unref(stream);
}

void
test_new (void)
{
    ChupaMetadata *meta;
    stream = chupa_text_input_stream_new(NULL, NULL);
    meta = chupa_text_input_stream_get_metadata(stream);
    cut_assert_not_null(meta);
    cut_assert_equal_int(0, chupa_metadata_size(meta));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
