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

static VALUE chupa_metadata_get(VALUE self, VALUE name);
static VALUE chupa_metadata_set(VALUE self, VALUE name, VALUE value);
static VALUE chupa_metadata_add(VALUE self, VALUE name, VALUE value);

static size_t
chupa_metadata_memsize(const void *ptr)
{
    return ptr ? sizeof(ChupaMetadata) : 0;
}

static const rb_data_type_t chupa_metadata_type = {
    "chupa",
#ifdef HAVE_RB_DATA_TYPE_T_FUNCTION
    {
#endif
        0, g_object_unref, chupa_metadata_memsize,
#ifdef HAVE_RB_DATA_TYPE_T_FUNCTION
    },
#endif
};

static VALUE
chupa_metadata_s_allocate(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &chupa_metadata_type, NULL);
}

#define CHUPA_RUBY_METADATA_READONLY FL_USER1

VALUE
chupa_ruby_metadata_new(VALUE klass, ChupaMetadata *meta, gboolean readonly)
{
    VALUE obj;
    g_object_ref(meta);
    obj = TypedData_Wrap_Struct(klass, &chupa_metadata_type, meta);
    if (readonly) {
        FL_SET(obj, CHUPA_RUBY_METADATA_READONLY);
    }
    return obj;
}

static void
chupa_metadata_writable(VALUE obj)
{
    rb_check_frozen(obj);
    if (FL_TEST(obj, CHUPA_RUBY_METADATA_READONLY)) {
        rb_raise(rb_eRuntimeError, "can't modify readonly %s", rb_obj_classname(obj));
    }
}

VALUE
chupa_metadata_get(VALUE self, VALUE name)
{
    ChupaMetadata *metadata = rb_check_typeddata(self, &chupa_metadata_type);
    const char *namestr = StringValueCStr(name);
    const char *valuestr = chupa_metadata_get_first_value(metadata, namestr);

    if (!valuestr) {
        return Qnil;
    }
    return rb_utf8_str_new_cstr(valuestr);
}

VALUE
chupa_metadata_set(VALUE self, VALUE name, VALUE value)
{
    ChupaMetadata *metadata = rb_check_typeddata(self, &chupa_metadata_type);
    const char *namestr;
    VALUE aryvalue;

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

VALUE
chupa_metadata_add(VALUE self, VALUE name, VALUE value)
{
    ChupaMetadata *metadata = rb_check_typeddata(self, &chupa_metadata_type);
    const char *namestr;
    VALUE aryvalue;

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
chupa_ruby_metadata_init(VALUE cChupa)
{
    VALUE cMeta = rb_define_class_under(cChupa, "Metadata", rb_cObject);
    rb_define_alloc_func(cMeta, chupa_metadata_s_allocate);
    rb_define_method(cMeta, "[]", chupa_metadata_get, 1);
    rb_define_method(cMeta, "[]=", chupa_metadata_set, 2);
    rb_define_method(cMeta, "set", chupa_metadata_set, 2);
    rb_define_method(cMeta, "add", chupa_metadata_add, 2);
    return cMeta;
}
