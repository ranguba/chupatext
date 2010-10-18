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
#include <chupatext/chupa_memory_input_stream.h>

#include <ruby.h>
#include <ruby/encoding.h>

#include <rbgobject.h>

void  chupa_ruby_init             (void);
VALUE chupa_ruby_gsf_output_init  (VALUE mGsf);
VALUE chupa_ruby_feeder_init      (VALUE mChupa);
VALUE chupa_ruby_data_init        (VALUE mChupa);
VALUE chupa_ruby_decomposer_init  (VALUE mChupa);
VALUE chupa_ruby_metadata_init    (VALUE mChupa);

VALUE chupa_ruby_metadata_new     (ChupaMetadata *metadata,
                                   gboolean readonly);

#define rb_utf8_str_new(str, len) rb_enc_str_new(str, len, rb_utf8_encoding())
#define rb_utf8_str_new_cstr(str) rb_enc_str_new(str, (long)strlen(str), rb_utf8_encoding())

GType        chupa_ruby_decomposer_get_type       (void);

#endif
