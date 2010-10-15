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

extern void Init_chupa(void);
static VALUE chupa_ruby_make_metadata(VALUE self, chupa_ruby_input_t *input, gboolean readonly);
static VALUE chupa_ruby_target_metadata(VALUE self);
static VALUE chupa_ruby_source_metadata(VALUE self);
static VALUE chupa_ruby_gets(VALUE self);
static VALUE chupa_ruby_read(int argc, VALUE *argv, VALUE self);
static VALUE chupa_ruby_decompose(VALUE self);

static void
chupa_ruby_mark(void *ptr)
{
    if (ptr) {
        chupa_ruby_t *ch = ptr;
        rb_gc_mark(ch->target.metadata);
        rb_gc_mark(ch->source.metadata);
    }
}

static void
chupa_ruby_dispose(void *ptr)
{
    if (ptr) {
        chupa_ruby_t *ch = ptr;
        gpointer *obj;
#define DISPOSE(member) \
        (!(obj = (gpointer)ch->member) ? (void)0 : \
         (void)(ch->member = 0, g_object_unref(obj)))
        DISPOSE(source.input);
        DISPOSE(target.input);
        DISPOSE(stream);
        DISPOSE(sink);
#undef DISPOSE
    }
}

static size_t
chupa_ruby_memsize(const void *ptr)
{
    return ptr ? sizeof(chupa_ruby_t) : 0;
}

static const rb_data_type_t chupa_ruby_type = {
    "chupa",
#ifdef HAVE_RB_DATA_TYPE_T_FUNCTION
    {
#endif
        chupa_ruby_mark, chupa_ruby_dispose, chupa_ruby_memsize,
#ifdef HAVE_RB_DATA_TYPE_T_FUNCTION
    },
#endif
};

static VALUE
chupa_ruby_s_allocate(VALUE klass)
{
    chupa_ruby_t *ptr;
    return TypedData_Make_Struct(klass, chupa_ruby_t, &chupa_ruby_type, ptr);
}

static VALUE
chupa_ruby_allocate(VALUE arg)
{
    ID name = (ID)arg;
    VALUE klass = rb_const_get(rb_cObject, rb_intern("Chupa"));
    if (name) {
        klass = rb_const_get(klass, name);
    }
    return chupa_ruby_s_allocate(klass);
}

static VALUE
chupa_require(VALUE name)
{
    return rb_require((const char *)name);
}

VALUE
chupa_ruby_new(const gchar *klassname, ChupaFeeder *feeder, ChupaTextInput *input)
{
    VALUE receiver;
    const char *filename = chupa_text_input_get_filename(input);
    int state = 0;
    chupa_ruby_t *ptr;
    ID klassid = 0;

    if (klassname) {
        GString *libname = g_string_new("chupa/decomposer/");
        klassname = "HTML"; /* FIXME */
        g_string_append(libname, klassname);
        g_string_ascii_down(libname);
        rb_protect(chupa_require, (VALUE)libname->str, &state);
        g_string_free(libname, TRUE);
        klassid = rb_intern(klassname);
    }
    receiver = rb_protect(chupa_ruby_allocate, (VALUE)klassid, &state);
    if (NIL_P(receiver)) {
        return receiver;
    }
    ptr = rb_check_typeddata(receiver, &chupa_ruby_type);
    g_object_ref(input);
    rb_iv_set(receiver, "@feeder", GOBJ2RVAL(feeder));
    ptr->source.input = input;
    ptr->sink = GSF_OUTPUT_MEMORY(gsf_output_memory_new());
    ptr->stream = CHUPA_MEMORY_INPUT_STREAM(chupa_memory_input_stream_new(ptr->sink));
    ptr->target.input = chupa_text_input_new_from_stream(NULL, G_INPUT_STREAM(ptr->stream), filename);
    rb_iv_set(receiver, "@target_input", GOBJ2RVAL(ptr->target.input));
    chupa_text_input_set_mime_type(ptr->target.input, "text/plain");

    return receiver;
}

VALUE
chupa_ruby_make_metadata(VALUE self, chupa_ruby_input_t *input, gboolean readonly)
{
    if (!input->metadata) {
        ChupaMetadata *metadata = chupa_text_input_get_metadata(input->input);
        input->metadata = chupa_ruby_metadata_new(metadata, readonly);
    }
    return input->metadata;
}

VALUE
chupa_ruby_target_metadata(VALUE self)
{
    chupa_ruby_t *ptr = rb_check_typeddata(self, &chupa_ruby_type);
    return chupa_ruby_make_metadata(self, &ptr->target, FALSE);
}

VALUE
chupa_ruby_source_metadata(VALUE self)
{
    chupa_ruby_t *ptr = rb_check_typeddata(self, &chupa_ruby_type);
    return chupa_ruby_make_metadata(self, &ptr->source, TRUE);
}

VALUE
chupa_ruby_gets(VALUE self)
{
    chupa_ruby_t *ptr = rb_check_typeddata(self, &chupa_ruby_type);
    GDataInputStream *input_stream = G_DATA_INPUT_STREAM(chupa_text_input_get_stream(ptr->source.input));
    gsize length;
    GCancellable *cancellable = NULL;
    GError **error = NULL;
    char *str = g_data_input_stream_read_line(input_stream, &length, cancellable, error);

    if (!str) {
        return Qnil;
    }
    return rb_utf8_str_new(str, (long)length);
}

VALUE
chupa_ruby_read(int argc, VALUE *argv, VALUE self)
{
    chupa_ruby_t *ptr = rb_check_typeddata(self, &chupa_ruby_type);
    GInputStream *input_stream = chupa_text_input_get_stream(ptr->source.input);
    GDataInputStream *data_input_stream = G_DATA_INPUT_STREAM(input_stream);
    gsize length;
    char *str;
    VALUE buffer;
    GCancellable *cancellable = NULL;
    GError **error = NULL;

    switch (argc) {
      case 0:
        str = g_data_input_stream_read_until(data_input_stream, "", &length, cancellable, error);
        return rb_utf8_str_new(str, (long)length);
        break;
      case 1:
        length = NUM2UINT(argv[0]);
        buffer = rb_utf8_str_new(NULL, (long)length);
        g_input_stream_read_all(input_stream, RSTRING_PTR(buffer), length, &length, cancellable, error);
        rb_str_set_len(buffer, (long)length);
        return buffer;
      default:
        rb_raise(rb_eArgError, "wrong number of arguments (%d for 0..1)", argc);
        return Qnil;            /* not reached */
    }
}

VALUE
chupa_ruby_decomposed(VALUE self, VALUE data)
{
    chupa_ruby_t *ptr = rb_check_typeddata(self, &chupa_ruby_type);

    StringValue(data);
    gsf_output_write(GSF_OUTPUT(ptr->sink), RSTRING_LEN(data), (const guchar *)RSTRING_PTR(data));
    return data;
}

#ifdef ABSTRACT_CLASS_RUBY
#define chupa_ruby_decompose rb_f_notimplement
#else
/* provisional implementation */
VALUE
chupa_ruby_decompose(VALUE self)
{
    return chupa_ruby_decomposed(self, chupa_ruby_read(0, NULL, self));
}
#endif

void
Init_chupa(void)
{
    VALUE mChupa, cBaseDecomposer;

    rb_require("chupa/pre_init");

    mChupa = rb_define_module("Chupa");
    cBaseDecomposer = rb_define_class_under(mChupa, "BaseDecomposer",
                                            rb_cObject);
    rb_define_method(cBaseDecomposer, "decomposed", chupa_ruby_decomposed, 1);
    rb_define_method(cBaseDecomposer, "gets", chupa_ruby_gets, 0);
    rb_define_method(cBaseDecomposer, "read", chupa_ruby_read, -1);
    rb_define_method(cBaseDecomposer, "decompose", chupa_ruby_decompose, 0);
    rb_define_method(cBaseDecomposer, "metadata", chupa_ruby_target_metadata, 0);
    rb_define_method(cBaseDecomposer, "target_metadata",
                     chupa_ruby_target_metadata, 0);
    rb_define_method(cBaseDecomposer, "source_metadata",
                     chupa_ruby_source_metadata, 0);
    chupa_ruby_feeder_init(mChupa);
    chupa_ruby_metadata_init(mChupa);

    rb_require("chupa/post_init");
}
