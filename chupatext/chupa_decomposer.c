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

#include <glib.h>
#include <string.h>

#include "chupa_decomposer.h"
#include "chupa_module.h"

#define CHUPA_DECOMPOSER_GET_PRIVATE(obj)                       \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj,                           \
                                 CHUPA_TYPE_DECOMPOSER,         \
                                 ChupaDecomposerPrivate))

typedef struct _ChupaDecomposerPrivate  ChupaDecomposerPrivate;
struct _ChupaDecomposerPrivate
{
    gchar *mime_type;
};

enum
{
    PROP_0,
    PROP_MIME_TYPE
};

G_DEFINE_ABSTRACT_TYPE(ChupaDecomposer, chupa_decomposer, G_TYPE_OBJECT)

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
chupa_decomposer_class_init(ChupaDecomposerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_string("mime-type",
                               "The MIME type of the decomposer",
                               "The MIME type of the decomposer",
                               NULL,
                               G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_MIME_TYPE, spec);

    g_type_class_add_private(gobject_class, sizeof(ChupaDecomposerPrivate));
}

static void
chupa_decomposer_init(ChupaDecomposer *decomposer)
{
    ChupaDecomposerPrivate *priv;

    priv = CHUPA_DECOMPOSER_GET_PRIVATE(decomposer);
    priv->mime_type = NULL;
}

static void
dispose(GObject *object)
{
    ChupaDecomposerPrivate *priv;

    priv = CHUPA_DECOMPOSER_GET_PRIVATE(object);

    if (priv->mime_type) {
        g_free(priv->mime_type);
        priv->mime_type = NULL;
    }

    G_OBJECT_CLASS(chupa_decomposer_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    ChupaDecomposerPrivate *priv;

    priv = CHUPA_DECOMPOSER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_MIME_TYPE:
        if (priv->mime_type)
            g_free(priv->mime_type);
        priv->mime_type = g_value_dup_string(value);
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
    ChupaDecomposerPrivate *priv;

    priv = CHUPA_DECOMPOSER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_MIME_TYPE:
        g_value_set_string(value, priv->mime_type);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

gboolean
chupa_decomposer_feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
                      ChupaData *data, GError **error)
{
    ChupaDecomposerClass *decomposer_class;

    g_return_val_if_fail(CHUPA_IS_DECOMPOSER(decomposer), FALSE);
    decomposer_class = CHUPA_DECOMPOSER_GET_CLASS(decomposer);
    return decomposer_class->feed(decomposer, feeder, data, error);
}

const gchar *
chupa_decomposer_get_mime_type(ChupaDecomposer *decomposer)
{
    ChupaDecomposerPrivate *priv;

    priv = CHUPA_DECOMPOSER_GET_PRIVATE(decomposer);
    return priv->mime_type;
}

