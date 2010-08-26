/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPATEXT_DECOMPOSER_H
#define CHUPATEXT_DECOMPOSER_H

#include <glib.h>
#include <glib-object.h>
#include "chupatext/chupa_text.h"

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

    gboolean (*can_handle)(ChupaDecomposerClass *dec, ChupaTextInputStream *stream, const char *mime_type);
    void (*feed)(ChupaDecomposer *dec, ChupaText *text, ChupaTextInputStream *stream);
};

GType        chupa_decomposer_get_type(void) G_GNUC_CONST;

ChupaDecomposer *chupa_decomposer_search(const gchar *mime_type);
void         chupa_decomposer_feed(ChupaDecomposer *dec, ChupaText *text, ChupaTextInputStream *stream);

void chupa_decomposer_load_modules(void);
void chupa_decomposer_register(const gchar *mime_type, GType type);
void chupa_decomposer_unregister(const gchar *mime_type, GType type);

G_END_DECLS

#endif  /* CHUPATEXT_DECOMPOSER_H */
