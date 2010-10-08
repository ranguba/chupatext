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

#define CHUPA_TYPE_HTML_DECOMPOSER chupa_type_html_decomposer
#define CHUPA_HTML_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_HTML_DECOMPOSER, ChupaHtmlDecomposer)
#define CHUPA_HTML_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_HTML_DECOMPOSER, ChupaHtmlDecomposerClass)
#define CHUPA_IS_HTML_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_HTML_DECOMPOSER)
#define CHUPA_IS_HTML_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_HTML_DECOMPOSER)
#define CHUPA_HTML_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_HTML_DECOMPOSER, ChupaHtmlDecomposerClass)

typedef ChupaRubyDecomposer ChupaHtmlDecomposer;
typedef ChupaRubyDecomposerClass ChupaHtmlDecomposerClass;

static GType chupa_type_html_decomposer = 0;

static VALUE
chupa_ruby_html_init(VALUE arg)
{
    rb_require("chupa/html");
    return rb_path2class("Chupa::HTML");
}

static void
chupa_html_decomposer_class_init(ChupaHtmlDecomposerClass *klass)
{
    ChupaRubyDecomposerClass *super = CHUPA_RUBY_DECOMPOSER_CLASS(klass);
    GError *error = NULL;
    VALUE vklass;

    vklass = chupa_ruby_protect(chupa_ruby_html_init, Qnil, NULL, &error);
    if (NIL_P(vklass)) {
        g_error("%s", error->message);
    }
    else {
        super->klass = vklass;
    }
}

static void
register_type(GTypeModule *type_module)
{
    static const GTypeInfo info = {
        sizeof(ChupaHtmlDecomposerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) chupa_html_decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaHtmlDecomposer),
        0,
        (GInstanceInitFunc) NULL,
    };

    chupa_type_html_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_RUBY_DECOMPOSER,
                                    "ChupaHtmlDecomposer",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CHUPA_MODULE_IMPL_INIT(GTypeModule *type_module)
{
    GList *registered_types = NULL;
    register_type(type_module);

    registered_types =
        g_list_prepend(registered_types,
                       (gchar *)g_type_name(chupa_type_html_decomposer));

    return registered_types;
}

G_MODULE_EXPORT void
CHUPA_MODULE_IMPL_EXIT(void)
{
}

G_MODULE_EXPORT GObject *
CHUPA_MODULE_IMPL_INSTANTIATE(const gchar *first_property, va_list var_args)
{
    g_object_ref(chupa_decomposer_new("ruby", NULL));

    return g_object_new_valist(CHUPA_TYPE_HTML_DECOMPOSER,
                               first_property, var_args);
}