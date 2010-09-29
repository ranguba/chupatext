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

static void
chupa_ruby_dispose(void *ptr)
{
    if (ptr) {
        chupa_ruby_t *ch = ptr;
        gpointer *obj;
#define DISPOSE(member) \
        (!(obj = (gpointer)ch->member) ? (void)0 : \
         (void)(ch->member = 0, g_object_unref(obj)))
        DISPOSE(chupar);
        DISPOSE(feed);
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
        0, chupa_ruby_dispose, chupa_ruby_memsize,
#ifdef HAVE_RB_DATA_TYPE_T_FUNCTION
    },
#endif
};

typedef struct {
    GError **g_error;
    VALUE error;
} make_error_arguments;

static VALUE
make_error_message(VALUE arg)
{
    make_error_arguments *ptr = (make_error_arguments *)arg;
    GError **g_error = ptr->g_error;
    VALUE error = ptr->error;
    GString *error_message;
    VALUE message, class_name, backtrace;
    long i;

    error_message = g_string_new(NULL);
    message = rb_funcall(error, rb_intern("message"), 0);
    class_name = rb_funcall(CLASS_OF(error), rb_intern("to_s"), 0);
    g_string_append_printf(error_message, "%s (%s)\n",
                           StringValueCStr(message),
                           StringValueCStr(class_name));
    backtrace = rb_funcall(error, rb_intern("backtrace"), 0);
    for (i = 0; i < RARRAY_LEN(backtrace); i++) {
        VALUE line = RARRAY_PTR(backtrace)[i];
        g_string_append_printf(error_message, "%s\n", StringValueCStr(line));
    }
    g_set_error(g_error,
                CHUPA_TEXT_ERROR, CHUPA_TEXT_ERROR_UNKNOWN,
                "unknown error is occurred: <%s>", error_message->str);
    g_string_free(error_message, TRUE);

    return Qnil;
}

static VALUE
chupa_ruby_s_allocate(VALUE klass)
{
    chupa_ruby_t *ptr;
    return TypedData_Make_Struct(klass, chupa_ruby_t, &chupa_ruby_type, ptr);
}

VALUE
chupa_ruby_new(VALUE klass, ChupaText *chupar, ChupaTextInput *input)
{
    VALUE receiver;
    const char *filename = chupa_text_input_get_filename(input);
    int state = 0;
    chupa_ruby_t *ptr;

    receiver = rb_protect(chupa_ruby_s_allocate, klass, &state);
    ptr = DATA_PTR(receiver);
    ptr->chupar = chupar;
    ptr->sink = GSF_OUTPUT_MEMORY(gsf_output_memory_new());
    ptr->stream = CHUPA_MEMORY_INPUT_STREAM(chupa_memory_input_stream_new(ptr->sink));
    ptr->feed = chupa_text_input_new_from_stream(NULL, G_INPUT_STREAM(ptr->stream), filename);

    return receiver;
}

typedef struct {
    VALUE receiver;
    ID name;
    int argc;
    VALUE *argv;
} funcall_arguments;

static VALUE
invoke_rb_funcall2(VALUE data)
{
    funcall_arguments *arguments = (funcall_arguments *)data;

    return rb_funcall2(arguments->receiver, arguments->name,
                       arguments->argc, arguments->argv);
}

VALUE
chupa_ruby_funcall(VALUE receiver, ID mid, int argc, VALUE *argv, GError **g_error)
{
    VALUE result, error;
    int state = 0;
    funcall_arguments call_args;

    call_args.receiver = receiver;
    call_args.name = mid;
    call_args.argc = argc;
    call_args.argv = argv;
    result = rb_protect(invoke_rb_funcall2, (VALUE)&call_args, &state);
    if (state && !NIL_P(error = rb_errinfo())) {
	make_error_arguments error_args;
	error_args.g_error = g_error;
	error_args.error = error;
	rb_protect(make_error_message, (VALUE)&error_args, &state);
    }

    return result;
}

VALUE
chupa_ruby_decomposed(VALUE self, VALUE data)
{
    chupa_ruby_t *ptr = rb_check_typeddata(self, &chupa_ruby_type);

    StringValue(data);
    gsf_output_write(GSF_OUTPUT(ptr->sink), RSTRING_LEN(data), (const guchar *)RSTRING_PTR(data));
    return data;
}

VALUE
chupa_ruby_init(void)
{
    extern void *chupa_stack_base;
    const VALUE *outer_klass = &rb_cObject;
    VALUE cChupa;

    if (!outer_klass || !*outer_klass) {
        int argc;
        const char *args[6];
        char **argv;
        gchar *rubydir, *rubyarchdir;

        argv = (char **)args;
        argc = 0;
        args[argc++] = "chupa";
        args[argc] = NULL;
        ruby_sysinit(&argc, &argv);
        ruby_init_stack(chupa_stack_base);
        ruby_init();
        rubydir = g_build_path("/", chupa_module_dir(), "ruby", NULL);
        rubyarchdir = g_build_path("/", rubydir, RUBY_ARCH, NULL);
        ruby_incpush(rubyarchdir);
        ruby_incpush(rubydir);
        g_free(rubyarchdir);
        g_free(rubydir);
        argc = 1;
        args[argc++] = "-e;";
        args[argc] = NULL;
        (void)ruby_process_options(argc, argv); /* ignore the insns which does nothing */
    }
    if (!rb_const_defined(*outer_klass, rb_intern("Chupa"))) {
        cChupa = rb_define_class_under(*outer_klass, "Chupa", rb_cObject);
        rb_define_method(cChupa, "decomposed", chupa_ruby_decomposed, 1);
    }
    return cChupa;
}
