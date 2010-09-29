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

#define CHUPA_TYPE_RUBY_DECOMPOSER chupa_type_ruby_decomposer
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
typedef struct _ChupaRubyDecomposerPrivate ChupaRubyDecomposerPrivate;

struct _ChupaRubyDecomposer
{
    ChupaDecomposer object;

    VALUE self;
};

struct _ChupaRubyDecomposerClass
{
    ChupaDecomposerClass parent_class;

    VALUE klass;
};

static GType chupa_type_ruby_decomposer = 0;

static gboolean
chupa_ruby_feed(chupa_ruby_t *self, GError **g_error)
{
    return chupa_text_feed(self->chupar, self->feed, g_error);
}

static gboolean
chupa_ruby_decomposer_feed(ChupaDecomposer *dec, ChupaText *chupar,
                           ChupaTextInput *input, GError **g_error)
{
    VALUE receiver;
    VALUE result;
    ID id_decomposed;
    CONST_ID(id_decomposed, "decomposed");

    gsf_input_seek(chupa_text_input_get_base_input(input), 0, G_SEEK_SET);

    receiver = chupa_ruby_new(CHUPA_RUBY_DECOMPOSER_GET_CLASS(dec)->klass, chupar, input);
    if (!NIL_P(receiver) && chupa_ruby_feed(DATA_PTR(receiver), g_error)) {
        result = chupa_ruby_funcall(receiver, id_decomposed, 0, 0, g_error);
    }

    return RTEST(result);
}

static void
chupa_ruby_decomposer_class_init(ChupaRubyDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_ruby_decomposer_feed;

    klass->klass = chupa_ruby_init();
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
        (GInstanceInitFunc) NULL,
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
