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

#ifndef CHUPATEXT_ARCHIVE_DECOMPOSER_H
#define CHUPATEXT_ARCHIVE_DECOMPOSER_H

#include <chupatext/chupa_decomposer.h>
#include <glib.h>
#include <gsf/gsf-infile.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_ARCHIVE_DECOMPOSER chupa_archive_decomposer_get_type()
#define CHUPA_ARCHIVE_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_ARCHIVE_DECOMPOSER, ChupaArchiveDecomposer)
#define CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_ARCHIVE_DECOMPOSER, ChupaArchiveDecomposerClass)
#define CHUPA_IS_ARCHIVE_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_ARCHIVE_DECOMPOSER)
#define CHUPA_IS_ARCHIVE_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_ARCHIVE_DECOMPOSER)
#define CHUPA_ARCHIVE_DECOMPOSER_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_ARCHIVE_DECOMPOSER, ChupaArchiveDecomposerClass)

typedef struct _ChupaArchiveDecomposer ChupaArchiveDecomposer;
typedef struct _ChupaArchiveDecomposerClass ChupaArchiveDecomposerClass;

struct _ChupaArchiveDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaArchiveDecomposerClass
{
    ChupaDecomposerClass parent_class;

    GsfInfile *(*get_infile)(GsfInput *input, GError **error);
    gboolean (*feed_component)(ChupaFeeder *feeder, ChupaData *data, GError **error);
};

GType chupa_archive_decomposer_get_type(void) G_GNUC_CONST;
GsfInfile *chupa_archive_decomoser_get_infile(ChupaArchiveDecomposer *);

G_END_DECLS

#endif  /* CHUPATEXT_ARCHIVE_DECOMPOSER_H */
