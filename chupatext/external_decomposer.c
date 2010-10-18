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

#include "external_decomposer.h"
#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>

#define CHUPA_EXTERNAL_DECOMPOSER_GET_PRIVATE(obj) NULL

#if 0
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_EXTERNAL_DECOMPOSER, \
                                 ChupaExternalDecomposerPrivate))

struct _ChupaExternalDecomposerPrivate
{
};
#endif

G_DEFINE_ABSTRACT_TYPE(ChupaExternalDecomposer, chupa_external_decomposer, CHUPA_TYPE_DECOMPOSER)

gboolean
chupa_external_decomposer_spawn(ChupaExternalDecomposer *dec, gchar **argv, GOutputStream **stdinput,
                                GInputStream **stdoutput, GError **error)
{
    ChupaExternalDecomposerPrivate *priv;
    gint input, output;

    priv = CHUPA_EXTERNAL_DECOMPOSER_GET_PRIVATE(dec);
    if (!g_spawn_async_with_pipes(NULL, argv, NULL, G_SPAWN_SEARCH_PATH,
                                  NULL, NULL, NULL,
                                  stdinput ? &input : NULL,
                                  stdoutput ? &output : NULL,
                                  NULL, error)) {
        return FALSE;
    }
    if (stdinput) {
        *stdinput = g_unix_output_stream_new(input, TRUE);
    }
    if (stdoutput) {
        *stdoutput = g_unix_input_stream_new(output, TRUE);
    }
    return TRUE;
}

void
chupa_external_decomposer_set_metadata(ChupaExternalDecomposer *dec, ChupaData *data)
{
}

#if 0
#define close_invalidate(stream) (void)(stream ? (g_object_unref(stream), stream = NULL) : NULL)

static void
dispose(GObject *object)
{
    ChupaExternalDecomposerPrivate *priv;

    priv = CHUPA_EXTERNAL_DECOMPOSER(object)->priv;

    G_OBJECT_CLASS(chupa_external_decomposer_parent_class)->dispose(object);
}

static void
set_property(GObject *object,
             guint prop_id,
             const GValue *value,
             GParamSpec *pspec)
{
    ChupaExternalDecomposer *dec = CHUPA_EXTERNAL_DECOMPOSER(object);
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
    ChupaExternalDecomposer *dec = CHUPA_EXTERNAL_DECOMPOSER(object);
    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
constructed(GObject *object)
{
    ChupaExternalDecomposer *dec;
    g_return_if_fail(CHUPA_IS_EXTERNAL_DECOMPOSER(object));
    dec = CHUPA_EXTERNAL_DECOMPOSER(object);
}
#endif

static gboolean
feed(ChupaDecomposer *dec, ChupaFeeder *feeder, ChupaData *data, GError **err)
{
    GInputStream *inp, *sout = NULL;
    GOutputStream *sin = NULL;
    ChupaExternalDecomposerClass *self_class;
    ChupaExternalDecomposer *extdec;
    ChupaExternalDecomposerPrivate *priv;
    gboolean result;

    g_return_val_if_fail(CHUPA_IS_EXTERNAL_DECOMPOSER(dec), FALSE);
    g_return_val_if_fail(CHUPA_IS_DATA(data), FALSE);
    extdec = CHUPA_EXTERNAL_DECOMPOSER(dec);
    self_class = CHUPA_EXTERNAL_DECOMPOSER_GET_CLASS(extdec);
    priv = extdec->priv;
    inp = chupa_data_get_stream(data);
    g_return_val_if_fail(self_class->spawn(extdec, feeder, &sin, &sout, err), FALSE);
    if (sin) {
        g_output_stream_splice_async(sin, inp,
                                     G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE|
                                     G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
                                     0, NULL,
                                     NULL, NULL);
        g_object_unref(sin);
    }
    g_return_val_if_fail(sout, FALSE);
    data = chupa_data_new_from_stream(NULL, sout,
                                      chupa_data_get_filename(data));
    g_object_unref(sout);
    self_class->set_metadata(extdec, data);
    result = chupa_feeder_feed(feeder, data, err);
    g_object_unref(data);
    return result;
}

static void
chupa_external_decomposer_init(ChupaExternalDecomposer *dec)
{
    ChupaExternalDecomposerClass *self_class;
    self_class = CHUPA_EXTERNAL_DECOMPOSER_GET_CLASS(dec);
    g_return_if_fail(self_class);
    dec->priv = CHUPA_EXTERNAL_DECOMPOSER_GET_PRIVATE(dec);
}

static void
chupa_external_decomposer_class_init(ChupaExternalDecomposerClass *klass)
{
#if 0
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
    {
        ChupaExternalDecomposerClass *extdec_class = CHUPA_EXTERNAL_DECOMPOSER_CLASS(klass);
        extdec_class->set_metadata = chupa_external_decomposer_set_metadata;
    }
}
