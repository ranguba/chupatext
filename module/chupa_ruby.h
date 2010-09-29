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

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_memory_input_stream.h>
#include <glib.h>
#include <ruby.h>

/* ruby class */
typedef struct {
    ChupaText *chupar;
    ChupaTextInput *feed;
    ChupaMemoryInputStream *stream;
    GsfOutputMemory *sink;
} chupa_ruby_t;

VALUE chupa_ruby_new(VALUE klass, ChupaText *chupar, ChupaTextInput *input);
VALUE chupa_ruby_funcall(VALUE receiver, ID mid, int argc, VALUE *argv, GError **g_error);
VALUE chupa_ruby_decomposed(VALUE self, VALUE data);
VALUE chupa_ruby_init(void);

