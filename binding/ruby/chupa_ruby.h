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

#ifndef CHUPA_RUBY_H
#define CHUPA_RUBY_H

#include <chupatext/chupa_decomposer.h>
#include <chupatext/chupa_module.h>
#include <chupatext/chupa_memory_input_stream.h>
#include <glib.h>
#include <ruby.h>
#include <ruby/encoding.h>

/* ruby class */
typedef struct {
    ChupaTextInput *input;
    VALUE metadata;
} chupa_ruby_input_t;

typedef struct {
    ChupaText *chupar;
    ChupaMemoryInputStream *stream;
    GsfOutputMemory *sink;
    chupa_ruby_input_t target, source;
} chupa_ruby_t;

VALUE chupa_ruby_new(VALUE klass, ChupaText *chupar, ChupaTextInput *input);
VALUE chupa_ruby_protect(VALUE (*func)(VALUE), VALUE arg, int *state, GError **g_error);
VALUE chupa_ruby_funcall(VALUE receiver, ID mid, int argc, VALUE *argv, GError **g_error);
VALUE chupa_ruby_decomposed(VALUE self, VALUE data);
VALUE chupa_ruby_init(void);

VALUE chupa_ruby_metadata_init(VALUE cChupa);
VALUE chupa_ruby_metadata_new(VALUE klass, ChupaMetadata *metadata, gboolean readonly);

#define rb_utf8_str_new(str, len) rb_enc_str_new(str, len, rb_utf8_encoding())
#define rb_utf8_str_new_cstr(str) rb_enc_str_new(str, (long)strlen(str), rb_utf8_encoding())

/* decomposer */
GType chupa_ruby_decomposer_get_type(void);
#define CHUPA_RUBY_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_RUBY_DECOMPOSER, ChupaRubyDecomposer)
#define CHUPA_RUBY_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_RUBY_DECOMPOSER, ChupaRubyDecomposerClass)
#define CHUPA_IS_RUBY_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_RUBY_DECOMPOSER)
#define CHUPA_IS_RUBY_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_RUBY_DECOMPOSER)
#define CHUPA_RUBY_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_RUBY_DECOMPOSER, ChupaRubyDecomposerClass)

#ifndef CHUPA_TYPE_RUBY_DECOMPOSER
#define CHUPA_TYPE_RUBY_DECOMPOSER chupa_ruby_decomposer_get_type()
#endif

typedef struct _ChupaRubyDecomposer ChupaRubyDecomposer;
typedef struct _ChupaRubyDecomposerClass ChupaRubyDecomposerClass;

struct _ChupaRubyDecomposer
{
    ChupaDecomposer object;

    VALUE self;
};

struct _ChupaRubyDecomposerClass
{
    ChupaDecomposerClass parent_class;

    VALUE klass;
};

#endif
