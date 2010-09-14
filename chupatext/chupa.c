/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <chupatext.h>

static void
output_to_FILE(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
{
    GInputStream *inst = G_INPUT_STREAM(chupa_text_input_get_stream(input));
    FILE *out = udata;
    const char *name = chupa_text_input_get_filename(input);
    const char *charset = chupa_text_input_get_charset(input);
    char buf[4096];
    gssize size;

    fprintf(out, "File: %s\n", name ? name : "(noname)");
    if (charset) {
        fprintf(out, "Charset: %s\n", charset);
    }
    putc('\n', out);
    while ((size = g_input_stream_read(inst, buf, sizeof(buf), NULL, NULL)) > 0) {
        fwrite(buf, 1, size, out);
    }
    putc('\n', out);
}

int
main(int argc, char **argv)
{
    int i;
    int rc = EXIT_SUCCESS;
    ChupaText *chupar;
    GError *err = NULL;
    gboolean version = FALSE;
    GOptionContext *ctx;
    GOptionEntry opts[] = {
        {
            "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
            "show version", NULL
        },
        { NULL }
    };
    opts[0].arg_data = &version;

    g_type_init();
    ctx = g_option_context_new(" input files...");
    g_option_context_set_description(ctx, "sample wraper of libchupatext.");
    g_option_context_set_help_enabled(ctx, TRUE);
    g_option_context_add_main_entries(ctx, opts, NULL);
    if (!g_option_context_parse(ctx, &argc, &argv, &err)) {
        g_print("Failed to initialize: %s\n", err->message);
        g_error_free(err);
        return EXIT_FAILURE;
    }
    if (version) {
        puts(chupa_version_description());
        return EXIT_SUCCESS;
    }

    chupa_init(&chupar);
    chupar = chupa_text_new();
    chupa_text_connect_decomposed(chupar, output_to_FILE, stdout);
    for (i = 1; i < argc; ++i) {
        GFile *file = g_file_new_for_commandline_arg(argv[i]);
        ChupaTextInput *input = chupa_text_input_new_from_file(NULL, file, &err);
        g_object_unref(file);
        if (!input || !chupa_text_feed(chupar, input, &err)) {
            fprintf(stderr, "%s: %s\n", argv[0], err->message);
            g_error_free(err);
            err = NULL;
            rc = EXIT_FAILURE;
            break;
        }
        g_object_unref(input);
    }
    g_object_unref(chupar);
    chupa_cleanup();
    return rc;
}
