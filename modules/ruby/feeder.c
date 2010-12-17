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

#define SELF(self) (CHUPA_FEEDER(RVAL2GOBJ(self)))

static VALUE rb_cGLibObject;

static VALUE
feed(VALUE self, VALUE input)
{
    ChupaFeeder *feeder;
    GError *error = NULL;
    gboolean success;

    feeder = SELF(self);
    success = chupa_feeder_feed(feeder, RVAL2GOBJ(input), &error);
    if (error) {
        RAISE_GERROR(error);
    }

    return CBOOL2RVAL(success);
}

static VALUE
accepted(VALUE self, VALUE data)
{
    ChupaFeeder *feeder;

    feeder = SELF(self);
    chupa_feeder_accepted(feeder, RVAL2GOBJ(data));

    return Qnil;
}

static VALUE
error_s_exception(int argc, VALUE *argv, VALUE klass)
{
    VALUE message;

    rb_scan_args(argc, argv, "01", &message);
    return rb_funcall(klass, rb_intern("new"),
                      2, rb_str_new2("unknown"), message);
}

static VALUE
error_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE code, message;

    rb_scan_args(argc, argv, "02", &code, &message);
    if (argc == 1) {
        message = code;
        code = Qnil;
    }

    rb_iv_set(self, "@domain", CSTR2RVAL(g_quark_to_string(CHUPA_FEEDER_ERROR)));
    if (!NIL_P(code)) {
        ChupaFeederError error_code;
        error_code = RVAL2GENUM(code, CHUPA_TYPE_FEEDER_ERROR);
        code = GENUM2RVAL(error_code, CHUPA_TYPE_FEEDER_ERROR);
    }
    rb_iv_set(self, "@code", code);

    rb_call_super(1, &message);

    return Qnil;
}

VALUE
chupa_ruby_feeder_init(VALUE mChupa, VALUE eChupaError)
{
    VALUE cFeeder, eFeederError;

    rb_cGLibObject = rb_const_get(rb_const_get(rb_cObject, rb_intern("GLib")),
                                  rb_intern("Object"));

    cFeeder = G_DEF_CLASS(CHUPA_TYPE_FEEDER, "Feeder", mChupa);

    rb_define_method(cFeeder, "feed", feed, 1);
    rb_define_method(cFeeder, "accepted", accepted, 1);

    eFeederError = G_DEF_ERROR(CHUPA_FEEDER_ERROR, "FeederError", mChupa,
                               eChupaError, Qnil);

    rb_define_singleton_method(eFeederError, "exception", error_s_exception, -1);
    rb_define_method(eFeederError, "initialize", error_initialize, -1);

    G_DEF_CLASS(CHUPA_TYPE_FEEDER_ERROR, "FeederErrorCode", mChupa);

    return cFeeder;
}
