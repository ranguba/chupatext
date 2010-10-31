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

#include <chupa_ruby.h>

#include <chupatext/chupa_decomposer_module.h>

/* ChupaRubyDecomposer */
#define CHUPA_TYPE_RUBY_DECOMPOSER            (chupa_ruby_decomposer_get_type ())
GType chupa_ruby_decomposer_get_type(void);
#define CHUPA_RUBY_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_RUBY_DECOMPOSER, ChupaRubyDecomposer)
#define CHUPA_RUBY_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_RUBY_DECOMPOSER, ChupaRubyDecomposerClass)
#define CHUPA_IS_RUBY_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_RUBY_DECOMPOSER)
#define CHUPA_IS_RUBY_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_RUBY_DECOMPOSER)
#define CHUPA_RUBY_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_RUBY_DECOMPOSER, ChupaRubyDecomposerClass)

typedef struct _ChupaRubyDecomposer ChupaRubyDecomposer;
typedef struct _ChupaRubyDecomposerClass ChupaRubyDecomposerClass;

struct _ChupaRubyDecomposer
{
    ChupaDecomposer object;

    VALUE decomposer;
};

struct _ChupaRubyDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

static GType chupa_type_ruby_decomposer = 0;
static ChupaRubyDecomposerClass *decomposer_parent_class;

GType
chupa_ruby_decomposer_get_type(void)
{
    return chupa_type_ruby_decomposer;
}

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
                CHUPA_FEEDER_ERROR, CHUPA_FEEDER_ERROR_UNKNOWN,
                "unknown error is occurred: <%s>", error_message->str);
    g_string_free(error_message, TRUE);

    return Qnil;
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

static VALUE
chupa_ruby_protect(VALUE (*func)(VALUE), VALUE arg, int *statep, GError **g_error)
{
    VALUE result, error;
    int state = 0;

    result = rb_protect(func, arg, &state);
    if (statep) {
        *statep = state;
    }
    if (state && !NIL_P(error = rb_errinfo())) {
	make_error_arguments error_args;
	error_args.g_error = g_error;
	error_args.error = error;
	rb_protect(make_error_message, (VALUE)&error_args, &state);
    }

    return result;
}

static VALUE
chupa_ruby_funcall(VALUE receiver, ID mid, int argc, VALUE *argv, GError **g_error)
{
    funcall_arguments call_args;

    call_args.receiver = receiver;
    call_args.name = mid;
    call_args.argc = argc;
    call_args.argv = argv;
    return chupa_ruby_protect(invoke_rb_funcall2, (VALUE)&call_args, NULL, g_error);
}

static gboolean
feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
     ChupaData *data, GError **error)
{
    ID id_feed;
    ChupaRubyDecomposer *ruby_decomposer;
    VALUE result;
    VALUE rb_decomposer;
    ChupaData *target_data;
    GInputStream *sink;
    ChupaMetadata *metadata;

    CONST_ID(id_feed, "feed");
    ruby_decomposer = CHUPA_RUBY_DECOMPOSER(decomposer);
    rb_decomposer = rb_funcall(ruby_decomposer->decomposer, rb_intern("new"), 0);
    rb_iv_set(rb_decomposer, "@feeder", GOBJ2RVAL(feeder));
    rb_iv_set(rb_decomposer, "@source", GOBJ2RVAL(data));
    sink = g_memory_input_stream_new();
    rb_iv_set(rb_decomposer, "@sink", GOBJ2RVAL(sink));
    metadata = chupa_metadata_new();
    chupa_metadata_add_value(metadata, "filename", chupa_data_get_filename(data));
    target_data = chupa_data_new(sink, metadata);
    g_object_unref(sink);
    g_object_unref(metadata);
    rb_iv_set(rb_decomposer, "@target", GOBJ2RVAL(target_data));
    chupa_data_set_mime_type(target_data, "text/plain");

    result = chupa_ruby_funcall(rb_decomposer, id_feed, 0, 0, error);

    return RTEST(result);
}

static void
decomposer_init(ChupaRubyDecomposer *decomposer)
{
    decomposer->decomposer = Qnil;
}

static void
decomposer_dispose(GObject *object)
{
    ChupaRubyDecomposer *decomposer;

    decomposer = CHUPA_RUBY_DECOMPOSER(object);
    if (!NIL_P(decomposer->decomposer)) {
        rb_gc_unregister_address(&(decomposer->decomposer));
        decomposer->decomposer = Qnil;
    }

    G_OBJECT_CLASS(decomposer_parent_class)->dispose(object);
}

static void
decomposer_class_init(ChupaRubyDecomposerClass *klass)
{
    GObjectClass *gobject_class;
    ChupaDecomposerClass *decomposer_class;

    decomposer_parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);

    gobject_class->dispose = decomposer_dispose;

    decomposer_class->feed = feed;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaRubyDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaRubyDecomposer),
        0,
        (GInstanceInitFunc)decomposer_init,
    };
    const gchar *type_name = "ChupaRubyDecomposer";

    chupa_type_ruby_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

static void
load_decomposer(ChupaRubyDecomposer *decomposer, VALUE loader)
{
    const gchar *mime_type;

    mime_type = chupa_decomposer_get_mime_type(CHUPA_DECOMPOSER(decomposer));
    decomposer->decomposer = rb_funcall(loader, rb_intern("decomposer"),
                                        1, CSTR2RVAL(mime_type));
    rb_gc_register_address(&(decomposer->decomposer));
}


/* ChupaRubyDecomposerFactory */
#define CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY \
    (chupa_type_ruby_decomposer_factory)
#define CHUPA_RUBY_DECOMPOSER_FACTORY(obj)                              \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY,     \
                                ChupaRubyDecomposerFactory))
#define CHUPA_RUBY_DECOMPOSER_FACTORY_CLASS(klass)                      \
    (G_TYPE_CHECK_CLASS_CAST((klass),                                   \
                             CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY,        \
                             ChupaRubyDecomposerFactoryClass))
#define CHUPA_IS_RUBY_DECOMPOSER_FACTORY(obj)                           \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY))
#define CHUPA_IS_RUBY_DECOMPOSER_FACTORY_CLASS(klass)                   \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY))
#define CHUPA_RUBY_DECOMPOSER_FACTORY_GET_CLASS(obj)                    \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY,      \
                               ChupaRubyDecomposerFactoryClass))

typedef struct _ChupaRubyDecomposerFactory ChupaRubyDecomposerFactory;
typedef struct _ChupaRubyDecomposerFactoryClass ChupaRubyDecomposerFactoryClass;

struct _ChupaRubyDecomposerFactory
{
    ChupaDecomposerFactory     object;

    VALUE loader;
};

struct _ChupaRubyDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_ruby_decomposer_factory = 0;
static ChupaDecomposerFactoryClass *factory_parent_class;

static void       factory_dispose  (GObject                *object);
static GList     *get_mime_types   (ChupaDecomposerFactory *factory);
static GObject   *create           (ChupaDecomposerFactory *factory,
                                    const gchar            *label,
                                    const gchar            *mime_type);

static void
factory_class_init(ChupaDecomposerFactoryClass *klass)
{
    GObjectClass *gobject_class;
    ChupaDecomposerFactoryClass *factory_class;

    factory_parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class = CHUPA_DECOMPOSER_FACTORY_CLASS(klass);

    gobject_class->dispose = factory_dispose;

    factory_class->get_mime_types   = get_mime_types;
    factory_class->create           = create;
}

static VALUE
factory_init_protect(VALUE arg)
{
    ChupaRubyDecomposerFactory *factory = (ChupaRubyDecomposerFactory *)arg;
    VALUE cLoader;

    cLoader = rb_const_get(rb_const_get(rb_cObject, rb_intern("Chupa")),
                           rb_intern("Loader"));
    factory->loader = rb_funcall2(cLoader, rb_intern("new"), 0, NULL);
    rb_gc_register_address(&(factory->loader));
    rb_funcall2(factory->loader, rb_intern("load"), 0, NULL);
    return Qnil;
}

static void
factory_init(ChupaRubyDecomposerFactory *factory)
{
    chupa_ruby_protect(factory_init_protect, (VALUE)factory, NULL, NULL);
}

static void
factory_dispose(GObject *object)
{
    ChupaRubyDecomposerFactory *factory;

    factory = CHUPA_RUBY_DECOMPOSER_FACTORY(object);
    rb_gc_unregister_address(&(factory->loader));

    G_OBJECT_CLASS(factory_parent_class)->dispose(object);
}

static void
factory_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info =
        {
            sizeof (ChupaRubyDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaRubyDecomposerFactory),
            0,
            (GInstanceInitFunc)factory_init,
        };
    const gchar *type_name = "ChupaRubyDecomposerFactory";

    chupa_type_ruby_decomposer_factory =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER_FACTORY,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

static GList *
get_mime_types(ChupaDecomposerFactory *factory)
{
    GList *mime_types = NULL;
    VALUE rb_loader;
    VALUE rb_mime_types;
    long i, length;

    rb_loader = CHUPA_RUBY_DECOMPOSER_FACTORY(factory)->loader;
    rb_mime_types = rb_funcall(rb_loader, rb_intern("mime_types"), 0);
    length = RARRAY_LEN(rb_mime_types);
    for (i = 0; i < length; i++) {
        VALUE rb_mime_type;
        rb_mime_type = RARRAY_PTR(rb_mime_types)[i];
        mime_types = g_list_prepend(mime_types, g_strdup(StringValueCStr(rb_mime_type)));
    }

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label, const gchar *mime_type)
{
    GObject *object;

    object = g_object_new(CHUPA_TYPE_RUBY_DECOMPOSER,
                          "mime-type", mime_type,
                          NULL);
    load_decomposer(CHUPA_RUBY_DECOMPOSER(object),
                    CHUPA_RUBY_DECOMPOSER_FACTORY(factory)->loader);
    return object;
}

int ruby_executable_node(void *n, int *status); /* prototype was missing in 1.9.2 */

static void
add_load_path(void)
{
    const gchar *ruby_library_dir;

    ruby_library_dir = g_getenv("CHUPA_RUBY_LIBRARY_DIR");
    if (!ruby_library_dir)
        ruby_library_dir = CHUPA_RUBY_LIBRARY_DIR;
    ruby_incpush(ruby_library_dir);
}

static gboolean
init_ruby_interpreter(void)
{
    extern void *chupa_stack_base;
    const VALUE *outer_klass = &rb_cObject;
    int argc;
    const char *args[6];
    char **argv;
    void *node;

    if (outer_klass && *outer_klass) {
        add_load_path();
        return TRUE;
    }

    argv = (char **)args;
    argc = 0;
    args[argc++] = "chupa";
    args[argc] = NULL;
    ruby_sysinit(&argc, &argv);
    ruby_init_stack(chupa_stack_base);
    ruby_init();
    add_load_path();
    argc = 1;
    args[argc++] = "-e;";
    args[argc] = NULL;
    node = ruby_options(argc, argv);
    /* if (!ruby_executable_node(node, &status)) { */
    /*     ruby_cleanup(status); */
    /*     return FALSE; */
    /* } */

    return TRUE;
}

static void
init_ruby(void)
{
    if (!init_ruby_interpreter())
        return;

    chupa_ruby_init();
}

/* module entry points */
G_MODULE_EXPORT GList *
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    init_ruby();

    return registered_types;
}

G_MODULE_EXPORT gboolean
CHUPA_DECOMPOSER_QUIT(void)
{
    ruby_cleanup(0);

    return TRUE;
}

G_MODULE_EXPORT GObject *
CHUPA_DECOMPOSER_CREATE_FACTORY(const gchar *first_property, va_list va_args)
{
    return g_object_new_valist(CHUPA_TYPE_RUBY_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
