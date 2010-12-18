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
#include <chupatext/chupa_types.h>

static VALUE
ruby_value_from_g_time_val(const GValue *value)
{
    VALUE rb_cTime;
    GTimeVal *time_value;

    time_value = chupa_value_get_time_val(value);
    if (!time_value)
        return Qnil;

    rb_cTime = rb_const_get(rb_cObject, rb_intern("Time"));
    return rb_funcall(rb_cTime, rb_intern("at"), 2,
                      LONG2NUM(time_value->tv_sec),
                      LONG2NUM(time_value->tv_usec));
}

static void
ruby_value_to_g_time_val(VALUE time, GValue *value)
{
    GTimeVal time_value;

    time_value.tv_sec = NUM2LONG(rb_funcall(time, rb_intern("to_i"), 0));
    time_value.tv_usec = NUM2LONG(rb_funcall(time, rb_intern("usec"), 0));
    chupa_value_set_time_val(value, &time_value);
}

VALUE
chupa_ruby_types_init(VALUE mChupa, VALUE eChupaGError)
{
    rbgobj_register_g2r_func(CHUPA_TYPE_TIME_VAL, ruby_value_from_g_time_val);
    rbgobj_register_r2g_func(CHUPA_TYPE_TIME_VAL, ruby_value_to_g_time_val);

    return Qnil;
}
