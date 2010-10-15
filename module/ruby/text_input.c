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

#define SELF(self) (CHUPA_TEXT_INPUT(RVAL2GOBJ(self)))

static VALUE
text_input_gets(VALUE self)
{
    ChupaTextInput *input;
    GInputStream *input_stream;
    GDataInputStream *data_input_stream;
    VALUE rb_data;
    gchar *data;
    gsize length;
    GCancellable *cancellable = NULL;
    GError *error = NULL;

    input = SELF(self);
    input_stream = chupa_text_input_get_stream(input);
    data_input_stream = G_DATA_INPUT_STREAM(input_stream);

    data = g_data_input_stream_read_line(data_input_stream, &length,
                                         cancellable, &error);
    if (error)
        RAISE_GERROR(error);

    if (!data)
        return Qnil;

    rb_data = rb_external_str_new_with_enc(data, (long)length,
                                           rb_ascii8bit_encoding());
    g_free(data);
    return rb_data;
}

static VALUE
text_input_read(int argc, VALUE *argv, VALUE self)
{
    ChupaTextInput *input;
    GInputStream *input_stream;
    GDataInputStream *data_input_stream;
    VALUE rb_length, rb_data;
    gchar *data;
    gsize length;
    GCancellable *cancellable = NULL;
    GError *error = NULL;

    input = SELF(self);
    input_stream = chupa_text_input_get_stream(input);
    data_input_stream = G_DATA_INPUT_STREAM(input_stream);

    rb_scan_args(argc, argv, "01", &rb_length);
    if (NIL_P(rb_length)) {
        data = g_data_input_stream_read_until(data_input_stream, "",
                                              &length, cancellable, &error);
        if (error)
            RAISE_GERROR(error);
        rb_data = rb_external_str_new_with_enc(data, (long)length,
                                               rb_ascii8bit_encoding());
        g_free(data);
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
text_input_get_metadata(VALUE self)
{
    ChupaTextInput *input;

    input = SELF(self);
    return GOBJ2RVAL(chupa_text_input_get_metadata(input));
}

VALUE
chupa_ruby_text_input_init(VALUE mChupa)
{
    VALUE cTextInput;

    cTextInput = G_DEF_CLASS(CHUPA_TYPE_TEXT_INPUT, "TextInput", mChupa);

    rb_define_method(cTextInput, "gets", text_input_gets, 0);
    rb_define_method(cTextInput, "read", text_input_read, -1);
    rb_define_method(cTextInput, "metadata", text_input_get_metadata, 0);

    return cTextInput;
}
