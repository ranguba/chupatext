/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext/chupa_restrict_input_stream.h>

#include <gcutter.h>

void test_read (void);
void test_skip_to_end (void);

static GInputStream *stream;

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
test_read (void)
{
    static const char data[] = "foobarzot";
    char buf[sizeof(data)];
    GInputStream *source = g_memory_input_stream_new_from_data(data, strlen(data), NULL);
    GInputStream *region;
    gsize count;

    stream = source;
    count = g_input_stream_read(source, buf, 3, NULL, NULL);
    gcut_assert_equal_int64(3, count);
    buf[count] = '\0';
    cut_assert_equal_string("foo", buf);
    region = chupa_restrict_input_stream_new(source, 3);
    stream = G_INPUT_STREAM(region);
    count = g_input_stream_read(region, buf, sizeof(buf), NULL, NULL);
    gcut_assert_equal_int64(3, count);
    buf[count] = '\0';
    cut_assert_equal_string("bar", buf);
    count = g_input_stream_read(source, buf, sizeof(buf), NULL, NULL);
    gcut_assert_equal_int64(3, count);
    buf[count] = '\0';
    cut_assert_equal_string("zot", buf);
}

void
test_skip_to_end (void)
{
    static const char data[] = "foobarxyzzot";
    char buf[sizeof(data)];
    GInputStream *source = g_memory_input_stream_new_from_data(data, strlen(data), NULL);
    GInputStream *region;
    gsize count;

    stream = source;
    count = g_input_stream_read(source, buf, 3, NULL, NULL);
    gcut_assert_equal_int64(3, count);
    buf[count] = '\0';
    cut_assert_equal_string("foo", buf);
    region = chupa_restrict_input_stream_new(source, 6);
    stream = G_INPUT_STREAM(region);
    count = g_input_stream_read(region, buf, 3, NULL, NULL);
    gcut_assert_equal_int64(3, count);
    buf[count] = '\0';
    cut_assert_equal_string("bar", buf);
    count = chupa_restrict_input_stream_skip_to_end(region);
    gcut_assert_equal_int64(3, count);
    count = g_input_stream_read(source, buf, sizeof(buf), NULL, NULL);
    gcut_assert_equal_int64(3, count);
    buf[count] = '\0';
    cut_assert_equal_string("zot", buf);
}

void
test_read_line (void)
{
    static const char data[] = "foo\nbar\nzot\n";
    char buf[sizeof(data)];
    GInputStream *source = g_memory_input_stream_new_from_data(data, strlen(data), NULL);
    GDataInputStream *reader;
    gsize length;
    char *ptr;

    stream = source;
    stream = G_INPUT_STREAM(chupa_restrict_input_stream_new(source, 5));
    reader = g_data_input_stream_new(stream);
    stream = G_INPUT_STREAM(reader);
    ptr = g_data_input_stream_read_line(reader, &length, NULL, NULL);
    gcut_assert_equal_int64(3, length);
    cut_assert_equal_string("foo", ptr);
    g_free(ptr);
    ptr = g_data_input_stream_read_line(reader, &length, NULL, NULL);
    gcut_assert_equal_int64(1, length);
    cut_assert_equal_string("b", ptr);
    g_free(ptr);
    length = g_input_stream_read(source, buf, sizeof(buf), NULL, NULL);
    gcut_assert_equal_int64(7, length);
    buf[length] = '\0';
    cut_assert_equal_string("ar\nzot\n", buf);
    length = g_input_stream_read(source, buf, sizeof(buf), NULL, NULL);
    gcut_assert_equal_int64(0, length);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

