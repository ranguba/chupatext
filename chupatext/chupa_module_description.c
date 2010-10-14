/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

#ifdef HAVE_CONFIG_H
#  include <chupatext/config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <glib.h>

#include "chupa_module_description.h"

#define CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(obj)                  \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                            \
                                 CHUPA_TYPE_MODULE_DESCRIPTION,    \
                                 ChupaModuleDescriptionPrivate))

typedef struct _ChupaModuleDescriptionPrivate    ChupaModuleDescriptionPrivate;
struct _ChupaModuleDescriptionPrivate
{
    gchar *name;
    gchar *label;
    GList *mime_types;
    ChupaDecomposerFactory *factory;
};

enum
{
    PROP_0,
    PROP_NAME,
    PROP_LABEL,
    PROP_FACTORY
};

G_DEFINE_TYPE(ChupaModuleDescription, chupa_module_description, G_TYPE_OBJECT)

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);
static void
chupa_module_description_class_init (ChupaModuleDescriptionClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("name",
                               "The name of the module",
                               "The name of the module",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_NAME, spec);

    spec = g_param_spec_string("label",
                               "The label of the module",
                               "The label of the module",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_LABEL, spec);

    spec = g_param_spec_object("factory",
                               "The factory of the decomposer",
                               "The factory of the decomposer",
                               CHUPA_TYPE_DECOMPOSER_FACTORY,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_FACTORY, spec);

    g_type_class_add_private(gobject_class,
                             sizeof(ChupaModuleDescriptionPrivate));
}

static void
chupa_module_description_init (ChupaModuleDescription *description)
{
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description);
    priv->name = NULL;
    priv->label = NULL;
    priv->mime_types = NULL;
    priv->factory = NULL;
}

static void
dispose(GObject *object)
{
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(object);
    if (priv->name) {
        g_free(priv->name);
        priv->name = NULL;
    }

    if (priv->label) {
        g_free(priv->label);
        priv->label = NULL;
    }

    if (priv->mime_types) {
        g_list_foreach(priv->mime_types, (GFunc)g_free, NULL);
        g_list_free(priv->mime_types);
        priv->mime_types = NULL;
    }

    if (priv->factory) {
        g_object_unref(priv->factory);
        priv->factory = NULL;
    }

    G_OBJECT_CLASS(chupa_module_description_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_NAME:
        if (priv->name)
            g_free(priv->name);
        priv->name = g_value_dup_string(value);
        break;
    case PROP_LABEL:
        if (priv->label)
            g_free(priv->label);
        priv->label = g_value_dup_string(value);
        break;
    case PROP_FACTORY:
        if (priv->factory)
            g_object_unref(priv->factory);
        priv->factory = g_value_get_object(value);
        if (priv->factory)
            g_object_ref(priv->factory);
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
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, priv->name);
        break;
    case PROP_LABEL:
        g_value_set_string(value, priv->label);
        break;
    case PROP_FACTORY:
        g_value_set_object(value, priv->factory);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

ChupaModuleDescription *
chupa_module_description_new(const gchar *name)
{
    return g_object_new(CHUPA_TYPE_MODULE_DESCRIPTION,
                        "name", name,
                        NULL);
}

const gchar *
chupa_module_description_get_name (ChupaModuleDescription *description)
{
    return CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description)->name;
}

const gchar *
chupa_module_description_get_label (ChupaModuleDescription *description)
{
    return CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description)->label;
}

void
chupa_module_description_set_label (ChupaModuleDescription *description,
                                    const gchar *label)
{
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description);
    if (priv->label)
        g_free(priv->label);
    priv->label = g_strdup(label);
}

GList *
chupa_module_description_get_mime_types (ChupaModuleDescription *description)
{
    return CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description)->mime_types;
}

void
chupa_module_description_add_mime_type (ChupaModuleDescription *description,
                                        const gchar *mime_type)
{
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description);
    if (chupa_utils_string_equal(mime_type, "x-chupatext/x-dynamic")) {
        ChupaDecomposerFactory *factory;
        GList *mime_types, *node;

        factory = chupa_module_description_get_factory(description);
        mime_types = chupa_decomposer_factory_get_mime_types(factory);
        for (node = mime_types; node; node = g_list_next(node)) {
            const gchar *dynamic_mime_type = node->data;
            priv->mime_types = g_list_append(priv->mime_types,
                                             g_strdup(dynamic_mime_type));
        }
    } else {
        priv->mime_types = g_list_append(priv->mime_types, g_strdup(mime_type));
    }
}

ChupaDecomposerFactory *
chupa_module_description_get_factory (ChupaModuleDescription *description)
{
    ChupaModuleDescriptionPrivate *priv;

    priv = CHUPA_MODULE_DESCRIPTION_GET_PRIVATE(description);
    if (!priv->factory) {
        priv->factory = chupa_decomposer_factory_new(priv->name);
    }
    return priv->factory;
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
