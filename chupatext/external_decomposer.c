/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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
chupa_external_decomposer_set_metadata(ChupaExternalDecomposer *dec, ChupaTextInput *input)
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
feed(ChupaDecomposer *dec, ChupaText *chupar, ChupaTextInput *input, GError **err)
{
    GInputStream *inp, *sout = NULL;
    GOutputStream *sin = NULL;
    ChupaExternalDecomposerClass *self_class;
    ChupaExternalDecomposer *extdec;
    ChupaExternalDecomposerPrivate *priv;
    ChupaMetadata *meta;
    gboolean result;

    g_return_val_if_fail(CHUPA_IS_EXTERNAL_DECOMPOSER(dec), FALSE);
    g_return_val_if_fail(CHUPA_IS_TEXT_INPUT(input), FALSE);
    extdec = CHUPA_EXTERNAL_DECOMPOSER(dec);
    self_class = CHUPA_EXTERNAL_DECOMPOSER_GET_CLASS(extdec);
    priv = extdec->priv;
    inp = G_INPUT_STREAM(chupa_text_input_get_stream(input));
    g_return_val_if_fail(self_class->spawn(extdec, chupar, &sin, &sout, err), FALSE);
    if (sin) {
        g_output_stream_splice_async(sin, inp,
                                     G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE|
                                     G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
                                     0, NULL,
                                     NULL, NULL);
        g_object_unref(sin);
    }
    g_return_val_if_fail(sout, FALSE);
    input = chupa_text_input_new_from_stream(NULL, sout,
                                             chupa_text_input_get_filename(input));
    g_object_unref(sout);
    self_class->set_metadata(extdec, input);
    result = chupa_text_feed(chupar, input, err);
    g_object_unref(input);
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
