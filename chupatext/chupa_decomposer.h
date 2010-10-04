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

#ifndef CHUPATEXT_DECOMPOSER_H
#define CHUPATEXT_DECOMPOSER_H

#include <glib.h>
#include <glib-object.h>
#include <chupatext/chupa_text.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_DECOMPOSER            chupa_decomposer_get_type()
#define CHUPA_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_DECOMPOSER, ChupaDecomposer)
#define CHUPA_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_DECOMPOSER, ChupaDecomposerClass)
#define CHUPA_IS_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_DECOMPOSER)
#define CHUPA_IS_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_DECOMPOSER)
#define CHUPA_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_DECOMPOSER, ChupaDecomposerClass)

typedef struct _ChupaDecomposer ChupaDecomposer;
typedef struct _ChupaDecomposerClass ChupaDecomposerClass;

struct _ChupaDecomposer
{
    GObject object;
};

struct _ChupaDecomposerClass
{
    GObjectClass parent_class;

    gboolean (*can_handle)(ChupaDecomposerClass *dec, ChupaTextInput *input, const char *mime_type);
    ChupaTextInput *(*feed)(ChupaDecomposer *dec, ChupaText *text, ChupaTextInput *input, GError **err);
};

GType        chupa_decomposer_get_type(void) G_GNUC_CONST;

ChupaDecomposer *chupa_decomposer_new    (const gchar *name,
                                          const gchar *first_property,
                                          ...);
ChupaTextInput *chupa_decomposer_feed   (ChupaDecomposer *dec,
                                          ChupaText       *text,
                                          ChupaTextInput  *input,
                                          GError         **error);

G_END_DECLS

#endif  /* CHUPATEXT_DECOMPOSER_H */
