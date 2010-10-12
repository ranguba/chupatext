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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <chupatext.h>

static void
output_plain(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
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
    while ((size = g_input_stream_read(inst, buf, sizeof(buf), NULL, NULL)) > 0) {
        fwrite(buf, 1, size, out);
    }
    putc('\n', out);
}

static void
write_quote(const char *str, gsize len, FILE *out)
{
    char c, esc;
    for (; len > 0; --len) {
        c = *str++;
        esc = 0;
        if (isascii(c)) {
            switch (c) {
            case '\t': esc = 't'; break;
            case '\r': esc = 'r'; break;
            case '\n': esc = 'n'; break;
            case '\f': esc = 'f'; break;
            case '\b': esc = 'b'; break;
            case '\\': case '"': esc = c; break;
            }
        }
        if (esc) {
            putc('\\', out);
            putc(esc, out);
        }
        else {
            putc(c, out);
        }
    }
}

#define quote(string, out) \
        write_quote(string, (string) ? strlen(string) : 0, out)

static void
output_json(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
{
    GInputStream *inst = G_INPUT_STREAM(chupa_text_input_get_stream(input));
    FILE *out = udata;
    const char *name = chupa_text_input_get_filename(input);
    const char *charset = chupa_text_input_get_charset(input);
    char buf[4096];
    gssize size;

    fputs("[\n\"", out);
    quote(name, out);
    fputs("\",\n\"", out);
    quote(charset, out);
    fputs("\",\n\"", out);
    while ((size = g_input_stream_read(inst, buf, sizeof(buf), NULL, NULL)) > 0) {
        write_quote(buf, size, out);
    }
    fputs("\"\n]\n", out);
}

int
main(int argc, char **argv)
{
    int i;
    int rc = EXIT_SUCCESS;
    ChupaText *chupar;
    GError *err = NULL;
    gboolean json = FALSE;
    gboolean version = FALSE;
    void (*output)(ChupaText *chupar, ChupaTextInput *input, gpointer udata);
    GOptionContext *ctx;
    GOptionEntry opts[] = {
        {
            "json", 'j', 0, G_OPTION_ARG_NONE, NULL,
            "output in JSON", NULL
        },
        {
            "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
            "show version", NULL
        },
        { NULL }
    };
    opts[0].arg_data = &json;
    opts[1].arg_data = &version;

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
    output = json ? output_json : output_plain;
    g_signal_connect(chupar, chupa_text_signal_decomposed,
                     (GCallback)output, stdout);
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
    chupa_quit();
    return rc;
}
