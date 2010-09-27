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

#include "archive_decomposer.h"

#ifdef USE_CHUPA_ARCHIVE_DECOMPOSER_PRIVATE
#define CHUPA_ARCHIVE_DECOMPOSER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_ARCHIVE_DECOMPOSER_OBJECT, \
                                 ChupaArchiveDecomposerPrivate))

typedef struct _ChupaArchiveDecomposerPrivate  ChupaArchiveDecomposerPrivate;
struct _ChupaArchiveDecomposerPrivate
{
};
#endif

G_DEFINE_ABSTRACT_TYPE(ChupaArchiveDecomposer, chupa_archive_decomposer, CHUPA_TYPE_DECOMPOSER)

#ifdef USE_CHUPA_ARCHIVE_DECOMPOSER_PRIVATE
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
#endif

static gboolean
feed(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInput *input, GError **err)
{
    GsfInfile *infile;
    GsfInput *inp;
    gboolean result = TRUE;
    ChupaArchiveDecomposerClass *arch_class;
    int i, num;

    g_return_val_if_fail(CHUPA_IS_ARCHIVE_DECOMPOSER(dec), FALSE);
    g_return_val_if_fail(CHUPA_IS_TEXT_INPUT(input), FALSE);
    arch_class = CHUPA_ARCHIVE_DECOMPOSER_GET_CLASS(dec);
    inp = chupa_text_input_get_base_input(input);
    infile = arch_class->get_infile(inp, err);
    if (!infile) {
        return FALSE;
    }
    num = gsf_infile_num_children(infile);
    for (i = 0; i < num; ++i) {
        const char *name = gsf_infile_name_by_index(infile, i);
        GsfInput *inp = gsf_infile_child_by_index(infile, i);
        ChupaTextInput *t = chupa_text_input_new(NULL, inp);
        g_object_unref(inp);
        if (name) {
            chupa_text_input_set_filename(t, name);
        }
        result = arch_class->feed_component(chupar, t, err);
        g_object_unref(t);
        if (!result) {
            break;
        }
    }
    return result;
}

static void
chupa_archive_decomposer_init(ChupaArchiveDecomposer *dec)
{
}

static void
chupa_archive_decomposer_class_init(ChupaArchiveDecomposerClass *klass)
{
#ifdef USE_CHUPA_ARCHIVE_DECOMPOSER_PRIVATE
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
        gobject_class->constructed  = constructed;
        gobject_class->dispose      = dispose;
        gobject_class->set_property = set_property;
        gobject_class->get_property = get_property;
    }
#endif
    {
        ChupaDecomposerClass *decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);
        decomposer_class->feed = feed;
    }
    CHUPA_ARCHIVE_DECOMPOSER_CLASS(klass)->feed_component = chupa_text_feed;
}
