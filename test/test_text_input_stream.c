/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_text_input_stream.h>

#include <gcutter.h>

void test_new (void);
void test_new_with_metadata (void);

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
    GInputStream *memstream = g_memory_input_stream_new();
    stream = chupa_text_input_stream_new(NULL, memstream);
    meta = chupa_text_input_stream_get_metadata(stream);
    cut_assert_not_null(meta);
    cut_assert_equal_int(1, chupa_metadata_size(meta));
    cut_assert_not_null(chupa_metadata_get_first_value(meta, "mime-type"));
}

void
test_new_with_metadata (void)
{
    ChupaMetadata *meta = chupa_metadata_new();
    GInputStream *memstream = g_memory_input_stream_new();
    stream = chupa_text_input_stream_new(meta, memstream);
    gcut_assert_equal_object(meta, chupa_text_input_stream_get_metadata(stream));
    cut_assert_equal_uint(1, chupa_metadata_size(meta));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
