/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

#include "chupa_ruby.h"

#define SELF(self) (CHUPA_DATA(RVAL2GOBJ(self)))

static VALUE
data_gets(VALUE self)
{
    ChupaData *data;
    GInputStream *input_stream;
    GDataInputStream *data_input_stream;
    VALUE rb_data;
    gchar *line;
    gsize length;
    GCancellable *cancellable = NULL;
    GError *error = NULL;

    data = SELF(self);
    input_stream = chupa_data_get_stream(data);
    data_input_stream = G_DATA_INPUT_STREAM(input_stream);

    line = g_data_input_stream_read_line(data_input_stream, &length,
                                         cancellable, &error);
    if (error)
        RAISE_GERROR(error);

    if (!line)
        return Qnil;

    rb_data = rb_external_str_new_with_enc(line, (long)length,
                                           rb_ascii8bit_encoding());
    g_free(line);
    return rb_data;
}

static VALUE
data_read(int argc, VALUE *argv, VALUE self)
{
    ChupaData *data;
    GInputStream *input_stream;
    GDataInputStream *data_input_stream;
    VALUE rb_length, rb_data;
    gchar *read_data;
    gsize length;
    GCancellable *cancellable = NULL;
    GError *error = NULL;

    data = SELF(self);
    input_stream = chupa_data_get_stream(data);
    data_input_stream = G_DATA_INPUT_STREAM(input_stream);

    rb_scan_args(argc, argv, "01", &rb_length);
    if (NIL_P(rb_length)) {
        read_data = g_data_input_stream_read_until(data_input_stream, "",
                                              &length, cancellable, &error);
        if (error)
            RAISE_GERROR(error);
        rb_data = rb_external_str_new_with_enc(read_data, (long)length,
                                               rb_ascii8bit_encoding());
        g_free(read_data);
    } else {
        length = NUM2UINT(rb_length);
        rb_data = rb_external_str_new_with_enc(NULL, (long)length,
                                               rb_ascii8bit_encoding());
        g_input_stream_read_all(input_stream, RSTRING_PTR(rb_data),
                                length, &length, cancellable, &error);
        if (error)
            RAISE_GERROR(error);
        rb_str_set_len(rb_data, (long)length);
    }

    return rb_data;
}

static VALUE
data_get_metadata(VALUE self)
{
    ChupaData *data;

    data = SELF(self);
    return GOBJ2RVAL(chupa_data_get_metadata(data));
}

VALUE
chupa_ruby_data_init(VALUE mChupa)
{
    VALUE cData;

    cData = G_DEF_CLASS(CHUPA_TYPE_DATA, "Data", mChupa);

    rb_define_method(cData, "gets", data_gets, 0);
    rb_define_method(cData, "read", data_read, -1);
    rb_define_method(cData, "metadata", data_get_metadata, 0);

    return cData;
}
