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

#ifndef __CHUPA_MODULE_DESCRIPTION_H__
#define __CHUPA_MODULE_DESCRIPTION_H__

#include <glib-object.h>

#include <chupatext/chupa_decomposer_factory.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_MODULE_DESCRIPTION            (chupa_module_description_get_type ())
#define CHUPA_MODULE_DESCRIPTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_MODULE_DESCRIPTION, ChupaModuleDescription))
#define CHUPA_MODULE_DESCRIPTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_MODULE_DESCRIPTION, ChupaModuleDescriptionClass))
#define CHUPA_IS_MODULE_DESCRIPTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_MODULE_DESCRIPTION))
#define CHUPA_IS_MODULE_DESCRIPTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_MODULE_DESCRIPTION))
#define CHUPA_MODULE_DESCRIPTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_MODULE_DESCRIPTION, ChupaModuleDescriptionClass))

typedef struct _ChupaModuleDescription         ChupaModuleDescription;
typedef struct _ChupaModuleDescriptionClass    ChupaModuleDescriptionClass;

struct _ChupaModuleDescription
{
    GObject object;
};

struct _ChupaModuleDescriptionClass
{
    GObjectClass parent_class;
};

GType        chupa_module_description_get_type  (void) G_GNUC_CONST;

ChupaModuleDescription *
             chupa_module_description_new       (const gchar            *name);

const gchar *chupa_module_description_get_name  (ChupaModuleDescription *description);
const gchar *chupa_module_description_get_label (ChupaModuleDescription *description);
void         chupa_module_description_set_label (ChupaModuleDescription *description,
                                                 const gchar            *label);
GList       *chupa_module_description_get_mime_types
                                                (ChupaModuleDescription *description);
void         chupa_module_description_add_mime_type
                                                (ChupaModuleDescription *description,
                                                 const gchar            *mime_type);

ChupaDecomposerFactory *
             chupa_module_description_get_factory
                                                (ChupaModuleDescription *description);


G_END_DECLS

#endif /* __CHUPA_MODULE_DESCRIPTION_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
