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

static VALUE
gerror_s_exception(int argc, VALUE *argv, VALUE klass)
{
    VALUE message;

    rb_scan_args(argc, argv, "01", &message);
    return rb_funcall(klass, rb_intern("new"),
                      2, rb_str_new2("unknown"), message);
}

static VALUE
gerror_initialize(int argc, VALUE *argv, VALUE self)
{
    ID id_domain, id_normalize_code;
    VALUE code, message;

    rb_scan_args(argc, argv, "02", &code, &message);
    if (argc == 1) {
        message = code;
        code = Qnil;
    }

    CONST_ID(id_domain, "domain");
    rb_iv_set(self, "@domain", rb_funcall(self, id_domain, 0));

    CONST_ID(id_normalize_code, "normalize_code");
    rb_iv_set(self, "@code", rb_funcall(self, id_normalize_code, 1, code));

    rb_call_super(1, &message);

    return Qnil;
}

VALUE
chupa_ruby_error_init(VALUE mChupa)
{
    VALUE eChupaError, eChupaGError;

    eChupaError = rb_define_class_under(mChupa, "Error", rb_eStandardError);
    eChupaGError = rb_define_class_under(mChupa, "GError", eChupaError);

    rb_define_singleton_method(eChupaGError, "exception",
                               gerror_s_exception, -1);

    rb_define_method(eChupaGError, "initialize", gerror_initialize, -1);

    return eChupaGError;
}
