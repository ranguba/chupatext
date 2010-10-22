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
data_read(int argc, VALUE *argv, VALUE self)
{
    ChupaData *data;
    GInputStream *input_stream;
    VALUE rb_length, rb_data = Qnil;
    gsize rest;
    GError *error = NULL;

    data = SELF(self);
    input_stream = chupa_data_get_stream(data);

    rb_scan_args(argc, argv, "01", &rb_length);
    if (NIL_P(rb_length)) {
        rest = -1;
    } else {
        rest = NUM2UINT(rb_length);
    }

    while (rest > 0) {
        gchar buffer[4096];
        VALUE rb_buffer;
        gsize read_bytes, request_bytes;

        request_bytes = sizeof(buffer);
        if (rest > 0 && request_bytes > rest)
            request_bytes = rest;
        read_bytes = g_input_stream_read(input_stream, buffer, request_bytes,
                                         NULL, &error);
        if (error)
            RAISE_GERROR(error);
        if (read_bytes == 0)
            break;
        rb_buffer = rb_external_str_new_with_enc(buffer, read_bytes,
                                                 rb_ascii8bit_encoding());
        if (NIL_P(rb_data)) {
            rb_data = rb_buffer;
        } else {
            rb_str_concat(rb_data, rb_buffer);
        }
        rest -= read_bytes;
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

static VALUE
data_is_text(VALUE self)
{
    return CBOOL2RVAL(chupa_data_is_text(SELF(self)));
}

static VALUE
data_is_finished(VALUE self)
{
    return CBOOL2RVAL(chupa_data_is_finished(SELF(self)));
}

static VALUE
data_is_succeeded(VALUE self)
{
    return CBOOL2RVAL(chupa_data_is_succeeded(SELF(self)));
}

VALUE
chupa_ruby_data_init(VALUE mChupa)
{
    VALUE cData;

    cData = G_DEF_CLASS(CHUPA_TYPE_DATA, "Data", mChupa);

    rb_define_method(cData, "read", data_read, -1);
    rb_define_method(cData, "metadata", data_get_metadata, 0);

    rb_define_method(cData, "text?", data_is_text, 0);
    rb_define_method(cData, "finished?", data_is_finished, 0);
    rb_define_method(cData, "succeeded?", data_is_succeeded, 0);

    return cData;
}
