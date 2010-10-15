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

static GType chupa_type_ruby_decomposer = 0;
static ChupaRubyDecomposerClass *decomposer_parent_class;

enum {
    PROP_0,
    PROP_CLASS,
    PROP_DUMMY
};

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
     ChupaTextInput *input, GError **error)
{
    VALUE receiver;
    ID id_feed;
    ChupaRubyDecomposer *ruby_decomposer;

    CONST_ID(id_feed, "feed");
    ruby_decomposer = CHUPA_RUBY_DECOMPOSER(decomposer);
    receiver = chupa_ruby_new(ruby_decomposer->label, feeder, input);
    if (!NIL_P(receiver)) {
        VALUE result = chupa_ruby_funcall(receiver, id_feed, 0, 0, error);
        return RTEST(result);
    }
    else {
        return FALSE;
    }
}

static void
decomposer_init(ChupaRubyDecomposer *decomposer)
{
    decomposer->label = NULL;
}

static void
dispose(GObject *object)
{
    ChupaRubyDecomposer *decomposer;

    decomposer = CHUPA_RUBY_DECOMPOSER(object);
    if (decomposer->label) {
        g_free(decomposer->label);
        decomposer->label = NULL;
    }

    G_OBJECT_CLASS(decomposer_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaRubyDecomposer *obj = CHUPA_RUBY_DECOMPOSER(object);
    switch (prop_id) {
    case PROP_CLASS:
        if (obj->label)
            g_free(obj->label);
        obj->label = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    ChupaRubyDecomposer *obj = CHUPA_RUBY_DECOMPOSER(object);

    switch (prop_id) {
    case PROP_CLASS:
        g_value_set_string(value, obj->label);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
decomposer_class_init(ChupaRubyDecomposerClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;
    ChupaDecomposerClass *decomposer_class;

    decomposer_parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    decomposer_class->feed = feed;

    spec = g_param_spec_string("class",
                               "The class of the module",
                               "The class of the module",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CLASS, spec);
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
};

struct _ChupaRubyDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_ruby_decomposer_factory = 0;
static ChupaDecomposerFactoryClass *factory_parent_class;

static GList     *get_mime_types   (ChupaDecomposerFactory *factory);
static GObject   *create           (ChupaDecomposerFactory *factory,
                                    const gchar            *label);

static void
factory_class_init(ChupaDecomposerFactoryClass *klass)
{
    GObjectClass *gobject_class;
    ChupaDecomposerFactoryClass *factory_class;

    factory_parent_class = g_type_class_peek_parent(klass);

    gobject_class = G_OBJECT_CLASS(klass);
    factory_class = CHUPA_DECOMPOSER_FACTORY_CLASS(klass);

    factory_class->get_mime_types   = get_mime_types;
    factory_class->create           = create;
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
            (GInstanceInitFunc)NULL,
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

    mime_types = g_list_prepend(mime_types, g_strdup("text/html"));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label)
{
    return g_object_new(CHUPA_TYPE_RUBY_DECOMPOSER,
                        "class", label,
                        NULL);
}

struct main_args {
    int argc;
    char **argv;
};

int ruby_executable_node(void *n, int *status); /* prototype was missing in 1.9.2 */

static VALUE
chupa_ruby_init(void)
{
    extern void *chupa_stack_base;
    const VALUE *outer_klass = &rb_cObject;
    ID id_Chupa;

    if (!outer_klass || !*outer_klass) {
        void Init_chupa(void);
        int status;
        int argc;
        const char *args[6];
        char **argv;
        gchar *rubydir, *rubyarchdir;
        void *node;

        argv = (char **)args;
        argc = 0;
        args[argc++] = "chupa";
        args[argc] = NULL;
        ruby_sysinit(&argc, &argv);
        ruby_init_stack(chupa_stack_base);
        ruby_init();
        rubydir = g_build_path("/", chupa_module_dir(), "ruby", NULL);
        rubyarchdir = g_build_path("/", rubydir, CHUPA_RUBY_ARCH, NULL);
        ruby_incpush(rubyarchdir);
        ruby_incpush(rubydir);
        g_free(rubyarchdir);
        g_free(rubydir);
        argc = 1;
        args[argc++] = "-e;";
        args[argc] = NULL;
        node = ruby_options(argc, argv);
        if (!ruby_executable_node(node, &status)) {
            ruby_cleanup(status);
            return Qnil;
        }
        Init_chupa();
    }
    CONST_ID(id_Chupa, "Chupa");
    return rb_const_get_at(*outer_klass, rb_intern("Chupa"));
}

static void
init_ruby(void)
{
    VALUE cChupa;

    cChupa = chupa_ruby_init();
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

G_MODULE_EXPORT void
CHUPA_DECOMPOSER_QUIT(void)
{
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
