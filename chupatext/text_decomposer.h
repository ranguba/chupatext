/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef TEXT_DECOMPOSER_H
#define TEXT_DECOMPOSER_H

#include "chupatext/chupa_decomposer.h"
#include <glib.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_TEXT_DECOMPOSER            chupa_text_decomposer_get_type()
#define CHUPA_TEXT_DECOMPOSER(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT_DECOMPOSER, ChupaTextDecomposer)
#define CHUPA_TEXT_DECOMPOSER_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT_DECOMPOSER, ChupaTextDecomposerClass)
#define CHUPA_IS_TEXT_DECOMPOSER(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT_DECOMPOSER)
#define CHUPA_IS_TEXT_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT_DECOMPOSER)
#define CHUPA_TEXT_DECOMPOSER_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT_DECOMPOSER, ChupaTextDecomposerClass)

typedef struct _ChupaTextDecomposer ChupaTextDecomposer;
typedef struct _ChupaTextDecomposerClass ChupaTextDecomposerClass;

struct _ChupaTextDecomposer
{
    ChupaDecomposer object;
};

struct _ChupaTextDecomposerClass
{
    ChupaDecomposerClass parent_class;
};

#endif
