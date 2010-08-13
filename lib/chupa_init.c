/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_private.h"
#include <glib.h>

int
chupa_init(void *var)
{
    return 0;
}

int
chupa_cleanup(void)
{
}

static GInputStream *
chupa_extrator_make_converter_input(ChupaExtractor *ext, GInputStream *input)
{
    return 0;
}

ChupaSwitcher *
chupa_switcher_new(GInputStream *cabra)
{
    GConverter *conv;
    ChupaSwitcher *chupar = g_new0(ChupaSwitcher, 1);
    ChupaExtractor *ext;

    if (G_IS_FILE_INPUT_STREAM(cabra)) {
        GFileInfo *info = g_file_input_stream_query_info(G_FILE_INPUT_STREAM(cabra),
                                                         G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                                                         NULL, NULL);
        ext = chupa_extractor_new(chupar, g_file_info_get_content_type(info));
        g_object_unref(info);
    }
    else {
        const gssize size = 1024;
        gsize len;
        const char *buf;
        gboolean uncertain;
        GBufferedInputStream *buffered;
        if (!G_IS_BUFFERED_INPUT_STREAM(cabra)) {
            cabra = g_buffered_input_stream_new_sized(cabra, size);
        }
        buffered = G_BUFFERED_INPUT_STREAM(cabra);
        g_buffered_input_stream_fill(buffered, size, NULL, NULL);
        buf = g_buffered_input_stream_peek_buffer(buffered, &len);
        ext = chupa_extractor_new(chupar, g_content_type_guess(NULL, buf, len, &uncertain));
    }
    if (ext) {
        cabra = ext->make_converter_input(cabra);
    }
    chupar->source = cabra;
    chupar->ext = ext;
    return chupar;
}

int
chupa_foreach(ChupaSwitcher *chupar, ChupaCallbackFunc *func, void *arg)
{
    ChupaCallback callback;
    if (!chupar->ext || !chupar->ext->foreach) {
        return (*func)(chupar->source, arg);
    }
    return (*chupar->ext->foreach)(chupar, chupar->source, func, arg);
}

ChupaExtractor *
chupa_extractor_new(ChupaSwitcher *chupar, const char *content_type)
{
    return 0;
}
