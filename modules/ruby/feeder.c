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

CHUPA_RUBY_DEF_EXCEPTION_METHODS(Feeder, FEEDER);

VALUE
chupa_ruby_feeder_init(VALUE mChupa, VALUE eChupaGError)
{
    VALUE cFeeder;

    rb_cGLibObject = rb_const_get(rb_const_get(rb_cObject, rb_intern("GLib")),
                                  rb_intern("Object"));

    cFeeder = G_DEF_CLASS(CHUPA_TYPE_FEEDER, "Feeder", mChupa);

    rb_define_method(cFeeder, "feed", feed, 1);
    rb_define_method(cFeeder, "accepted", accepted, 1);

    CHUPA_RUBY_DEF_EXCEPTION_CLASS(Feeder, FEEDER);

    return cFeeder;
}
