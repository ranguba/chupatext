/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2007-2010  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CHUPA_DECOMPOSER_FACTORY_H__
#define __CHUPA_DECOMPOSER_FACTORY_H__

#include <glib-object.h>

#include <chupatext/chupa_module.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_DECOMPOSER_FACTORY            (chupa_decomposer_factory_get_type ())
#define CHUPA_DECOMPOSER_FACTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_DECOMPOSER_FACTORY, ChupaDecomposerFactory))
#define CHUPA_DECOMPOSER_FACTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_DECOMPOSER_FACTORY, ChupaDecomposerFactoryClass))
#define CHUPA_IS_DECOMPOSER_FACTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_DECOMPOSER_FACTORY))
#define CHUPA_IS_DECOMPOSER_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_DECOMPOSER_FACTORY))
#define CHUPA_DECOMPOSER_FACTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_DECOMPOSER_FACTORY, ChupaDecomposerFactoryClass))

typedef struct _ChupaDecomposerFactory         ChupaDecomposerFactory;
typedef struct _ChupaDecomposerFactoryClass    ChupaDecomposerFactoryClass;

struct _ChupaDecomposerFactory
{
    GObject object;
};

struct _ChupaDecomposerFactoryClass
{
    GObjectClass parent_class;

    GObject     *(*create)              (ChupaDecomposerFactory *factory, const gchar *label);
    GList       *(*get_mime_types)      (ChupaDecomposerFactory *factory);
};

/* ChupaDecomposerFactory related functions */
void         chupa_decomposer_factory_init        (void);
void         chupa_decomposer_factory_quit        (void);

const gchar *chupa_decomposer_factory_get_default_module_dir   (void);
void         chupa_decomposer_factory_set_default_module_dir   (const gchar *dir);
const gchar *chupa_decomposer_factory_get_module_dir           (void);

void         chupa_decomposer_factory_load        (const gchar *dir,
                                                   const gchar *type);
void         chupa_decomposer_factory_load_all    (const gchar *base_dir);
void         chupa_decomposer_factory_unload      (void);
GList       *chupa_decomposer_factory_get_names   (const gchar *type);
gboolean     chupa_decomposer_factory_exist_module(const gchar *type,
                                                   const gchar *name);
ChupaModule *chupa_decomposer_factory_load_module (const gchar *type,
                                                   const gchar *name);

/* ChupaDecomposerFactory */
GType        chupa_decomposer_factory_get_type    (void) G_GNUC_CONST;

ChupaDecomposerFactory *
             chupa_decomposer_factory_new         (const gchar *type,
                                                   const gchar *name,
                                                   const gchar *first_property,
                                                   ...);
ChupaDecomposerFactory *
             chupa_decomposer_factory_new_valist  (const gchar *type,
                                                   const gchar *name,
                                                   const gchar *first_property,
                                                   va_list      var_args);

GObject     *chupa_decomposer_factory_create      (ChupaDecomposerFactory *factory,
                                                   const gchar *label);

GList       *chupa_decomposer_factory_get_mime_types
                                                  (ChupaDecomposerFactory *factory);

G_END_DECLS

#endif /* __CHUPA_DECOMPOSER_FACTORY_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
