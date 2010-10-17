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

#ifndef __CHUPA_DECOMPOSER_DESCRIPTION_H__
#define __CHUPA_DECOMPOSER_DESCRIPTION_H__

#include <glib-object.h>

#include <chupatext/chupa_decomposer_factory.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_DECOMPOSER_DESCRIPTION            (chupa_decomposer_description_get_type ())
#define CHUPA_DECOMPOSER_DESCRIPTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHUPA_TYPE_DECOMPOSER_DESCRIPTION, ChupaDecomposerDescription))
#define CHUPA_DECOMPOSER_DESCRIPTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHUPA_TYPE_DECOMPOSER_DESCRIPTION, ChupaDecomposerDescriptionClass))
#define CHUPA_IS_DECOMPOSER_DESCRIPTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHUPA_TYPE_DECOMPOSER_DESCRIPTION))
#define CHUPA_IS_DECOMPOSER_DESCRIPTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHUPA_TYPE_DECOMPOSER_DESCRIPTION))
#define CHUPA_DECOMPOSER_DESCRIPTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CHUPA_TYPE_DECOMPOSER_DESCRIPTION, ChupaDecomposerDescriptionClass))

typedef struct _ChupaDecomposerDescription         ChupaDecomposerDescription;
typedef struct _ChupaDecomposerDescriptionClass    ChupaDecomposerDescriptionClass;

struct _ChupaDecomposerDescription
{
    GObject object;
};

struct _ChupaDecomposerDescriptionClass
{
    GObjectClass parent_class;
};

void         chupa_decomposer_description_init      (void);
void         chupa_decomposer_description_quit      (void);
GList       *chupa_decomposer_description_get_list  (void);


GType        chupa_decomposer_description_get_type  (void) G_GNUC_CONST;

ChupaDecomposerDescription *
             chupa_decomposer_description_new
                                      (const gchar                *name);

const gchar *chupa_decomposer_description_get_name
                                      (ChupaDecomposerDescription *description);
const gchar *chupa_decomposer_description_get_label
                                      (ChupaDecomposerDescription *description);
void         chupa_decomposer_description_set_label
                                      (ChupaDecomposerDescription *description,
                                       const gchar                *label);
GList       *chupa_decomposer_description_get_mime_types
                                      (ChupaDecomposerDescription *description);
void         chupa_decomposer_description_add_mime_type
                                      (ChupaDecomposerDescription *description,
                                       const gchar                *mime_type);

ChupaDecomposerFactory *
             chupa_decomposer_description_get_factory
                                      (ChupaDecomposerDescription *description);


G_END_DECLS

#endif /* __CHUPA_DECOMPOSER_DESCRIPTION_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
