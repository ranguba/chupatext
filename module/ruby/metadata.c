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
#include <chupatext/chupa_metadata.h>

#define SELF(self) (CHUPA_METADATA(RVAL2GOBJ(self)))

#define CHUPA_RUBY_METADATA_READONLY FL_USER1

VALUE
chupa_ruby_metadata_new(ChupaMetadata *metadata, gboolean readonly)
{
    VALUE self;

    self = GOBJ2RVAL(metadata);
    if (readonly) {
        FL_SET(self, CHUPA_RUBY_METADATA_READONLY);
    }

    return self;
}

static void
chupa_metadata_writable(VALUE self)
{
    rb_check_frozen(self);
    if (FL_TEST(self, CHUPA_RUBY_METADATA_READONLY)) {
        rb_raise(rb_eRuntimeError, "can't modify readonly %s", rb_obj_classname(self));
    }
}

static VALUE
chupa_metadata_get(VALUE self, VALUE name)
{
    ChupaMetadata *metadata;
    const char *value;

    metadata = SELF(self);
    value = chupa_metadata_get_first_value(metadata, StringValueCStr(name));
    if (!value) {
        return Qnil;
    }
    return rb_utf8_str_new_cstr(value);
}

static VALUE
chupa_metadata_set(VALUE self, VALUE name, VALUE value)
{
    ChupaMetadata *metadata;
    const char *namestr;
    VALUE aryvalue;

    metadata = SELF(self);
    chupa_metadata_writable(self);
    StringValueCStr(name);
    name = rb_str_new_frozen(name);
    namestr = RSTRING_PTR(name);
    if (NIL_P(value)) {
        chupa_metadata_replace_value(metadata, namestr, NULL);
    }
    else if (!NIL_P(aryvalue = rb_check_array_type(value))) {
        long i;
        chupa_metadata_replace_value(metadata, namestr, NULL);
        for (i = 0; i < RARRAY_LEN(aryvalue); ++i) {
            value = RARRAY_PTR(aryvalue)[i];
            chupa_metadata_add_value(metadata, namestr, StringValueCStr(value));
        }
    }
    else {
        chupa_metadata_replace_value(metadata, namestr, StringValueCStr(value));
    }

    return value;
}

static VALUE
chupa_metadata_add(VALUE self, VALUE name, VALUE value)
{
    ChupaMetadata *metadata;
    const char *namestr;
    VALUE aryvalue;

    metadata = SELF(self);
    chupa_metadata_writable(self);
    StringValueCStr(name);
    name = rb_str_new_frozen(name);
    namestr = RSTRING_PTR(name);
    if (!NIL_P(aryvalue = rb_check_array_type(value))) {
        long i;
        for (i = 0; i < RARRAY_LEN(aryvalue); ++i) {
            value = RARRAY_PTR(aryvalue)[i];
            chupa_metadata_add_value(metadata, namestr, StringValueCStr(value));
        }
    }
    else {
        chupa_metadata_add_value(metadata, namestr, StringValueCStr(value));
    }

    return value;
}

VALUE
chupa_ruby_metadata_init(VALUE mChupa)
{
    VALUE cMetadata;

    cMetadata = G_DEF_CLASS(CHUPA_TYPE_METADATA, "Metadata", mChupa);
    rb_define_method(cMetadata, "[]", chupa_metadata_get, 1);
    rb_define_method(cMetadata, "[]=", chupa_metadata_set, 2);
    rb_define_method(cMetadata, "set", chupa_metadata_set, 2);
    rb_define_method(cMetadata, "add", chupa_metadata_add, 2);
    return cMetadata;
}
