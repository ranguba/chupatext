/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_ARCHIVE_DECOMPOSER_H
#define CHUPATEXT_ARCHIVE_DECOMPOSER_H

#include "chupatext/chupa_decomposer.h"
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
    GsfInfile *infile;
};

struct _ChupaArchiveDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

GType chupa_archive_decomposer_get_type(void) G_GNUC_CONST;
GsfInfile *chupa_archive_decomoser_get_infile(ChupaArchiveDecomposer *);
void chupa_archive_decomposer_feed(ChupaArchiveDecomposer *, ChupaText *, ChupaTextInput *);


G_END_DECLS

#endif  /* CHUPATEXT_ARCHIVE_DECOMPOSER_H */
