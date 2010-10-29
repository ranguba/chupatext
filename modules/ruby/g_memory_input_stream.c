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

#define SELF(self) (G_MEMORY_INPUT_STREAM(RVAL2GOBJ(self)))

static VALUE
initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE data;
    GInputStream *stream;

    rb_scan_args(argc, argv, "01", &data);
    if (NIL_P(data)) {
        stream = g_memory_input_stream_new();
    } else {
        StringValue(data);
        G_RELATIVE(self, data);
        stream = g_memory_input_stream_new_from_data(RSTRING_PTR(data),
                                                     RSTRING_LEN(data),
                                                     NULL);
    }
    G_INITIALIZE(self, stream);

    return Qnil;
}

static VALUE
add_data(VALUE self, VALUE data)
{
    StringValue(data);
    G_RELATIVE(self, data);
    g_memory_input_stream_add_data(SELF(self),
                                   RSTRING_PTR(data),
                                   RSTRING_LEN(data),
                                   NULL);
    return self;
}

VALUE
chupa_ruby_g_memory_input_stream_init(VALUE mGLib)
{
    VALUE cMemoryInputStream;

    cMemoryInputStream = G_DEF_CLASS(G_TYPE_MEMORY_INPUT_STREAM,
                                     "MemoryInputStream", mGLib);

    rb_define_method(cMemoryInputStream, "initialize", initialize, -1);
    rb_define_method(cMemoryInputStream, "add_data", add_data, 1);

    return cMemoryInputStream;
}
