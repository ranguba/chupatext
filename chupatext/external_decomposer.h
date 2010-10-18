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

#ifndef CHUPATEXT_EXTERNAL_DECOMPOSER_H
#define CHUPATEXT_EXTERNAL_DECOMPOSER_H

#include "chupatext/chupa_decomposer.h"
#include <gio/gio.h>
#include <glib.h>
#include <gsf/gsf-infile.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_EXTERNAL_DECOMPOSER chupa_external_decomposer_get_type()
#define CHUPA_EXTERNAL_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_EXTERNAL_DECOMPOSER, ChupaExternalDecomposer)
#define CHUPA_EXTERNAL_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_EXTERNAL_DECOMPOSER, ChupaExternalDecomposerClass)
#define CHUPA_IS_EXTERNAL_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_EXTERNAL_DECOMPOSER)
#define CHUPA_IS_EXTERNAL_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_EXTERNAL_DECOMPOSER)
#define CHUPA_EXTERNAL_DECOMPOSER_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_EXTERNAL_DECOMPOSER, ChupaExternalDecomposerClass)

typedef struct _ChupaExternalDecomposer ChupaExternalDecomposer;
typedef struct _ChupaExternalDecomposerClass ChupaExternalDecomposerClass;
typedef struct _ChupaExternalDecomposerPrivate ChupaExternalDecomposerPrivate;

struct _ChupaExternalDecomposer
{
    ChupaDecomposer object;

    ChupaExternalDecomposerPrivate *priv;
};

struct _ChupaExternalDecomposerClass
{
    ChupaDecomposerClass parent_class;

    gboolean (*spawn)(ChupaExternalDecomposer *dec, ChupaFeeder *feeder,
                      GOutputStream **stdinput, GInputStream **stdoutput,
                      GError **error);
    void (*set_metadata)(ChupaExternalDecomposer *dec, ChupaData *data);
};

GType chupa_external_decomposer_get_type(void) G_GNUC_CONST;
GsfInfile *chupa_external_decomoser_get_infile(ChupaExternalDecomposer *);
gboolean chupa_external_decomposer_spawn(ChupaExternalDecomposer *, gchar **, GOutputStream **,
                                         GInputStream **, GError **);
void chupa_external_decomposer_set_metadata(ChupaExternalDecomposer *, ChupaData *);

G_END_DECLS

#endif  /* CHUPATEXT_EXTERNAL_DECOMPOSER_H */
