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

#define CHUPA_TYPE_RUBY_DECOMPOSER chupa_type_ruby_decomposer
#include "chupa_ruby.h"

static GType chupa_type_ruby_decomposer = 0;

enum {
    PROP_0,
    PROP_CLASS,
    PROP_DUMMY
};

static VALUE
chupa_ruby_call_init(VALUE base)
{
    int argc;
    const char *args[6];
    char **argv;
    gchar *rubydir, *rubyarchdir;

    argv = (char **)args;
    argc = 0;
    args[argc++] = "chupa";
    args[argc] = NULL;
    ruby_sysinit(&argc, &argv);
    ruby_init_stack((void *)base);
    ruby_init();
    rubydir = g_build_path("/", chupa_module_dir(), "ruby", NULL);
    rubyarchdir = g_build_path("/", rubydir, CHUPA_RUBY_ARCH, NULL);
    ruby_incpush(rubyarchdir);
    ruby_incpush(rubydir);
    g_free(rubyarchdir);
    g_free(rubydir);
    argc = 1;
    args[argc++] = "-rchupa";
    args[argc++] = "-e;";
    args[argc] = NULL;
    (void)ruby_process_options(argc, argv); /* ignore the insns which does nothing */
    return Qtrue;
}

VALUE
chupa_ruby_init(void)
{
    extern void *chupa_stack_base;
    const VALUE *outer_klass = &rb_cObject;
    ID id_Chupa;

    if (!outer_klass || !*outer_klass) {
	int state;
	if (!RTEST(rb_protect(chupa_ruby_call_init, (VALUE)chupa_stack_base, &state))) {
	    return Qnil;
	}
    }
    CONST_ID(id_Chupa, "Chupa");
    return rb_const_get_at(*outer_klass, rb_intern("Chupa"));
}

static gboolean
chupa_ruby_feed(chupa_ruby_t *self, GError **g_error)
{
    return chupa_text_feed(self->chupar, self->target.input, g_error);
}

static gboolean
chupa_ruby_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
                           ChupaTextInput *input, GError **g_error)
{
    VALUE receiver;
    ID id_decompose;
    ChupaRubyDecomposer *rubydec = CHUPA_RUBY_DECOMPOSER(dec);
    chupa_ruby_funcs_t *funcs = CHUPA_RUBY_DECOMPOSER_GET_CLASS(rubydec)->funcs;
    CONST_ID(id_decompose, "decompose");

    receiver = funcs->new(rubydec->label, chupar, input);
    if (!NIL_P(receiver)) {
        VALUE result = funcs->funcall(receiver, id_decompose, 0, 0, g_error);
        if (RTEST(result)) {
            chupa_ruby_feed(DATA_PTR(receiver), g_error);
        }

	return RTEST(result);
    }
    else {
	return FALSE;
    }
}

static void
chupa_ruby_decomposer_init(ChupaRubyDecomposer *obj)
{
    obj->label = NULL;
}

static void
dispose(GObject *object)
{
    ChupaRubyDecomposer *obj = (ChupaRubyDecomposer *)object;
    if (obj->label) {
        g_free(obj->label);
        obj->label = NULL;
    }

    G_OBJECT_CLASS(g_type_class_peek_parent(G_OBJECT_GET_CLASS(object)))->dispose(object);
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
chupa_ruby_decomposer_class_init(ChupaRubyDecomposerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *spec;
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    VALUE cChupa;

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    super->feed = chupa_ruby_decomposer_feed;

    spec = g_param_spec_string("class",
                               "The class of the module",
                               "The class of the module",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_CLASS, spec);

    g_return_if_fail(!NIL_P(cChupa = chupa_ruby_init()));
    {
        VALUE funcs = rb_iv_get(cChupa, "funcs");
        klass->funcs = DATA_PTR(funcs);
    }
}

static void
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaRubyDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_ruby_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaRubyDecomposer),
        0,
        (GInstanceInitFunc) chupa_ruby_decomposer_init,
    };

    chupa_type_ruby_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_DECOMPOSER,
                                    "ChupaRubyDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;
    register_type(type_module);

    registered_types =
        g_list_prepend(registered_types,
                       (gchar *)g_type_name(chupa_type_ruby_decomposer));

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CHUPA_TYPE_RUBY_DECOMPOSER,
                               first_property, var_args);
}
