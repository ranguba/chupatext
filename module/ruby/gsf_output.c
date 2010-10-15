/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

#define SELF(self) (GSF_OUTPUT(RVAL2GOBJ(self)))

static VALUE
write(VALUE self, VALUE data)
{
    GsfOutput *output;
    gboolean success;

    output = SELF(self);
    StringValue(data);
    success = gsf_output_write(output,
                               RSTRING_LEN(data),
                               (const guint8 *)RSTRING_PTR(data));

    return CBOOL2RVAL(success);
}

VALUE
chupa_ruby_gsf_output_init(VALUE mGsf)
{
    VALUE cOutput;

    cOutput = G_DEF_CLASS(GSF_OUTPUT_TYPE, "Output", mGsf);

    rb_define_method(cOutput, "write", write, 1);

    G_DEF_CLASS(GSF_OUTPUT_MEMORY_TYPE, "OutputMemory", mGsf);

    return cOutput;
}
