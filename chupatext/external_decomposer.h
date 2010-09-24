/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

    gboolean (*spawn)(ChupaExternalDecomposer *dec, ChupaText *chupar,
                      GOutputStream **stdinput, GInputStream **stdoutput,
                      GError **error);
    void (*set_metadata)(ChupaExternalDecomposer *dec, ChupaTextInput *input);
};

GType chupa_external_decomposer_get_type(void) G_GNUC_CONST;
GsfInfile *chupa_external_decomoser_get_infile(ChupaExternalDecomposer *);

G_END_DECLS

#endif  /* CHUPATEXT_EXTERNAL_DECOMPOSER_H */