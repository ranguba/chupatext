/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "archive_decomposer.h"

#define CHUPA_ARCHIVE_DECOMPOSER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_ARCHIVE_DECOMPOSER_OBJECT, \
                                 ChupaArchiveDecomposerPrivate))

typedef struct _ChupaArchiveDecomposerPrivate  ChupaArchiveDecomposerPrivate;
struct _ChupaArchiveDecomposerPrivate
{
};

G_DEFINE_ABSTRACT_TYPE(ChupaArchiveDecomposer, chupa_archive_decomposer, CHUPA_TYPE_DECOMPOSER)

static void
dispose(GObject *object)
{
    ChupaArchiveDecomposer *dec = CHUPA_ARCHIVE_DECOMPOSER(object);
    G_OBJECT_CLASS(chupa_archive_decomposer_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaArchiveDecomposer *dec = CHUPA_ARCHIVE_DECOMPOSER(object);
    GObject *obj;

    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property(GObject *object,
             guint prop_id,
             GValue *value,
             GParamSpec *pspec)
{
    ChupaArchiveDecomposer *dec = CHUPA_ARCHIVE_DECOMPOSER(object);
    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
constructed(GObject *object)
{
    ChupaArchiveDecomposer *dec = CHUPA_ARCHIVE_DECOMPOSER(object);
    g_return_if_fail(dec);
}

static void
feed(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInput *input)
{
    GsfInfile *infile;
    GsfInput *inp;
    int i, num;

    g_return_if_fail(CHUPA_IS_ARCHIVE_DECOMPOSER(dec));
    g_return_if_fail(CHUPA_IS_TEXT_INPUT(input));
    inp = chupa_text_input_get_base_input(input);
    infile = CHUPA_ARCHIVE_DECOMPOSER_GET_CLASS(dec)->get_infile(inp, NULL);
    if (!infile) {
        return;
    }
    num = gsf_infile_num_children(infile);
    for (i = 0; i < num; ++i) {
        const char *name = gsf_infile_name_by_index(infile, i);
        GsfInput *inp = gsf_infile_child_by_index(infile, i);
        ChupaTextInput *t = chupa_text_input_new(NULL, inp);
        g_object_unref(inp);
        chupa_text_feed(chupar, t);
        g_object_unref(t);
    }
}

static void
chupa_archive_decomposer_init(ChupaArchiveDecomposer *dec)
{
}

static void
chupa_archive_decomposer_class_init(ChupaArchiveDecomposerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    ChupaDecomposerClass *decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);

    gobject_class->constructed  = constructed;
    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    decomposer_class->feed = feed;
}
