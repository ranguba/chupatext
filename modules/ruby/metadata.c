/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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
    value = chupa_metadata_get_string(metadata, RVAL2CSTR(name), NULL);
    if (!value) {
        return Qnil;
    }
    return CSTR2RVAL(value);
}

static VALUE
chupa_metadata_set(VALUE self, VALUE name, VALUE value)
{
    ChupaMetadata *metadata;
    const char *namestr;

    metadata = SELF(self);
    chupa_metadata_writable(self);
    StringValueCStr(name);
    name = rb_str_new_frozen(name);
    namestr = RSTRING_PTR(name);
    if (NIL_P(value)) {
        chupa_metadata_remove(metadata, namestr);
    } else {
        chupa_metadata_set_string(metadata, namestr, StringValueCStr(value));
    }

    return value;
}

static VALUE
metadata_remove(VALUE self, VALUE name)
{
    ChupaMetadata *metadata;

    metadata = SELF(self);
    chupa_metadata_remove(metadata, RVAL2CSTR(name));

    return Qnil;
}

static void
chupa_metadata_keys_push(ChupaMetadataField *field, gpointer user_data)
{
    VALUE *keys = user_data;
    rb_ary_push(*keys, CSTR2RVAL(chupa_metadata_field_name(field)));
}

static VALUE
chupa_metadata_keys(VALUE self)
{
    ChupaMetadata *metadata;
    VALUE keys = rb_ary_new();

    metadata = SELF(self);
    chupa_metadata_foreach(metadata, chupa_metadata_keys_push, &keys);
    return keys;
}

static void
chupa_metadata_value_push(ChupaMetadataField *field, gpointer user_data)
{
    VALUE *values = user_data;
    rb_ary_push(*values, CSTR2RVAL(chupa_metadata_field_value_string(field)));
}

static VALUE
chupa_metadata_values(VALUE self)
{
    ChupaMetadata *metadata;
    VALUE values = rb_ary_new();

    metadata = SELF(self);
    chupa_metadata_foreach(metadata, chupa_metadata_value_push, &values);
    return values;
}

static void
chupa_metadata_each_yield(ChupaMetadataField *field, gpointer user_data)
{
    rb_yield_values(1, GOBJ2RVAL(field));
}

static VALUE
chupa_metadata_each(VALUE self)
{
    ChupaMetadata *metadata;

    metadata = SELF(self);
    chupa_metadata_foreach(metadata, chupa_metadata_each_yield, NULL);
    return Qnil;
}

static VALUE
metadata_merge_original_metadata(VALUE self, VALUE original)
{
    ChupaMetadata *metadata, *original_metadata;

    metadata = SELF(self);
    original_metadata = SELF(original);
    chupa_metadata_merge_original_metadata(metadata, original_metadata);
    return Qnil;
}

VALUE
chupa_ruby_metadata_init(VALUE mChupa, VALUE eChupaGError)
{
    VALUE cMetadata;

    cMetadata = G_DEF_CLASS(CHUPA_TYPE_METADATA, "Metadata", mChupa);
    rb_define_method(cMetadata, "[]", chupa_metadata_get, 1);
    rb_define_method(cMetadata, "[]=", chupa_metadata_set, 2);
    rb_define_method(cMetadata, "set", chupa_metadata_set, 2);
    rb_define_method(cMetadata, "remove", metadata_remove, 1);
    rb_define_method(cMetadata, "keys", chupa_metadata_keys, 0);
    rb_define_method(cMetadata, "values", chupa_metadata_values, 0);
    rb_define_method(cMetadata, "each", chupa_metadata_each, 0);
    rb_define_method(cMetadata, "merge_original_metadata",
                     metadata_merge_original_metadata, 1);
    rb_include_module(cMetadata, rb_mEnumerable);
    return cMetadata;
}
