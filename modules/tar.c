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

#include <chupatext/chupa_decomposer_module.h>
#include <chupatext/chupa_archive_decomposer.h>
#include <glib.h>
#include <gsf/gsf-infile-tar.h>

/* ChupaTarDecomposer */
#define CHUPA_TYPE_TAR_DECOMPOSER               \
    chupa_type_tar_decomposer
#define CHUPA_TAR_DECOMPOSER(obj)                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                          \
                                CHUPA_TYPE_TAR_DECOMPOSER,      \
                                ChupaTarDecomposer))
#define CHUPA_TAR_DECOMPOSER_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                   \
                             CHUPA_TYPE_TAR_DECOMPOSER, \
                             ChupaTarDecomposerClass))
#define CHUPA_IS_TAR_DECOMPOSER(obj)                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                          \
                                CHUPA_TYPE_TAR_DECOMPOSER))
#define CHUPA_IS_TAR_DECOMPOSER_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                           \
                             CHUPA_TYPE_TAR_DECOMPOSER))
#define CHUPA_TAR_DECOMPOSER_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                           \
                               CHUPA_TYPE_TAR_DECOMPOSER,       \
                               ChupaTarDecomposerClass)

typedef struct _ChupaTarDecomposer ChupaTarDecomposer;
typedef struct _ChupaTarDecomposerClass ChupaTarDecomposerClass;

struct _ChupaTarDecomposer
{
    ChupaArchiveDecomposer parent_object;
};

struct _ChupaTarDecomposerClass
{
    ChupaArchiveDecomposerClass parent_class;
};

static GType chupa_type_tar_decomposer = 0;

static GsfInfile *
get_infile(ChupaArchiveDecomposer *archive_decomposer,
           GsfInput *input, GError **error)
{
    return gsf_infile_tar_new(input, error);
}

static void
decomposer_class_init(ChupaTarDecomposerClass *klass)
{
    ChupaArchiveDecomposerClass *decomposer_class;

    decomposer_class = CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass);
    decomposer_class->get_infile = get_infile;
}

static void
decomposer_register_type(GTypeModule *type_module, GList **registered_types)
{
    static const GTypeInfo info = {
        sizeof(ChupaTarDecomposerClass),
        (GBaseInitFunc)NULL,
        (GBaseFinalizeFunc)NULL,
        (GClassInitFunc)decomposer_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof(ChupaTarDecomposer),
        0,
        (GInstanceInitFunc)NULL,
    };
    const gchar *type_name = "ChupaTarDecomposer";

    chupa_type_tar_decomposer =
        g_type_module_register_type(type_module,
                                    CHUPA_TYPE_ARCHIVE_DECOMPOSER,
                                    type_name,
                                    &info, 0);

    *registered_types = g_list_prepend(*registered_types, g_strdup(type_name));
}

/* ChupaTarDecomposerFactory */
#define CHUPA_TYPE_TAR_DECOMPOSER_FACTORY       \
    (chupa_type_tar_decomposer_factory)
#define CHUPA_TAR_DECOMPOSER_FACTORY(obj)                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),                                  \
                                CHUPA_TYPE_TAR_DECOMPOSER_FACTORY,      \
                                ChupaTarDecomposerFactory))
#define CHUPA_TAR_DECOMPOSER_FACTORY_CLASS(klass)               \
    (G_TYPE_CHECK_CLASS_CAST((klass),                           \
                             CHUPA_TYPE_TAR_DECOMPOSER_FACTORY, \
                             ChupaTarDecomposerFactoryClass))
#define CHUPA_IS_TAR_DECOMPOSER_FACTORY(obj)                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),                                  \
                                CHUPA_TYPE_TAR_DECOMPOSER_FACTORY))
#define CHUPA_IS_TAR_DECOMPOSER_FACTORY_CLASS(klass)                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass),                                   \
                             CHUPA_TYPE_TAR_DECOMPOSER_FACTORY))
#define CHUPA_TAR_DECOMPOSER_FACTORY_GET_CLASS(obj)                     \
    (G_TYPE_INSTANCE_GET_CLASS((obj),                                   \
                               CHUPA_TYPE_TAR_DECOMPOSER_FACTORY,       \
                               ChupaTarDecomposerFactoryClass))

typedef struct _ChupaTarDecomposerFactory ChupaTarDecomposerFactory;
typedef struct _ChupaTarDecomposerFactoryClass ChupaTarDecomposerFactoryClass;

struct _ChupaTarDecomposerFactory
{
    ChupaDecomposerFactory     object;
};

struct _ChupaTarDecomposerFactoryClass
{
    ChupaDecomposerFactoryClass parent_class;
};

static GType chupa_type_tar_decomposer_factory = 0;
static ChupaDecomposerFactoryClass *factory_parent_class;

static GList     *get_mime_types   (ChupaDecomposerFactory *factory);
static GObject   *create           (ChupaDecomposerFactory *factory,
                                    const gchar            *label,
                                    const gchar            *mime_type,
                                    GError                **error);

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
            sizeof (ChupaTarDecomposerFactoryClass),
            (GBaseInitFunc)NULL,
            (GBaseFinalizeFunc)NULL,
            (GClassInitFunc)factory_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(ChupaTarDecomposerFactory),
            0,
            (GInstanceInitFunc)NULL,
        };
    const gchar *type_name = "ChupaTarDecomposerFactory";

    chupa_type_tar_decomposer_factory =
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

    mime_types = g_list_prepend(mime_types, g_strdup("application/tar"));

    return mime_types;
}

static GObject *
create(ChupaDecomposerFactory *factory, const gchar *label,
       const gchar *mime_type, GError **error)
{
    return g_object_new(CHUPA_TYPE_TAR_DECOMPOSER,
                        "mime-type", mime_type,
                        NULL);
}

/* module entry points */
G_MODULE_EXPORT GList *
CHUPA_DECOMPOSER_INIT(GTypeModule *type_module, GError **error)
{
    GList *registered_types = NULL;

    decomposer_register_type(type_module, &registered_types);
    factory_register_type(type_module, &registered_types);

    return registered_types;
}

G_MODULE_EXPORT gboolean
CHUPA_DECOMPOSER_QUIT(void)
{
    return TRUE;
}

G_MODULE_EXPORT GObject *
CHUPA_DECOMPOSER_CREATE_FACTORY(const gchar *first_property, va_list va_args)
{
    return g_object_new_valist(CHUPA_TYPE_TAR_DECOMPOSER_FACTORY,
                               first_property, va_args);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
