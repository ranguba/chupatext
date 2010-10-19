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

struct output_info {
    FILE *out;
    const char *prefix;
};

static void
output_plain(ChupaFeeder *feeder, ChupaData *data, gpointer udata)
{
    GInputStream *inst = chupa_data_get_stream(data);
    struct output_info *uinfo = udata;
    FILE *out = uinfo->out;
    const char *name = chupa_data_get_filename(data);
    const char *charset = chupa_data_get_charset(data);
    char *path = NULL;
    char buf[4096];
    gssize size;

    if (uinfo->prefix) {
        path = g_build_filename(uinfo->prefix, name, NULL);
    }
    fprintf(out, "File: %s\n", path ? path : name ? name : "(noname)");
    if (path) {
        g_free(path);
    }
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
            default:
                if (iscntrl(c)) esc = -1;
            }
        }
        if (esc) {
            putc('\\', out);
            if (esc == -1) {
                fprintf(out, "%.2x", c);
            }
            else {
                putc(esc, out);
            }
            
        }
        else {
            putc(c, out);
        }
    }
}

#define quote(string, out) \
        write_quote(string, (string) ? strlen(string) : 0, out)

static void
output_json(ChupaFeeder *feeder, ChupaData *data, gpointer udata)
{
    GInputStream *inst = chupa_data_get_stream(data);
    struct output_info *uinfo = udata;
    FILE *out = uinfo->out;
    const char *name = chupa_data_get_filename(data);
    const char *charset = chupa_data_get_charset(data);
    char *path = NULL;
    char buf[4096];
    gssize size;

    fputs("{\n\"_key\":\"", out);
    if (uinfo->prefix) {
        path = g_build_filename(uinfo->prefix, name, NULL);
    }
    quote(name, out);
    if (path) {
        g_free(path);
    }
    if (charset) {
        fputs("\",\n\"charset\":\"", out);
        quote(charset, out);
    }
    fputs("\",\n\"body\":\"", out);
    while ((size = g_input_stream_read(inst, buf, sizeof(buf), NULL, NULL)) > 0) {
        write_quote(buf, size, out);
    }
    fputs("\",\n},\n", out);
}

static const struct writer_funcs {
    void (*output)(ChupaFeeder *feeder, ChupaData *data, gpointer udata);
} plain_writer = {output_plain},
    json_writer = {output_json};

int
main(int argc, char **argv)
{
    int i;
    int rc = EXIT_SUCCESS;
    ChupaFeeder *feeder;
    GError *err = NULL;
    gboolean json = FALSE;
    gboolean ignore_errors = FALSE;
    gboolean version = FALSE;
    const struct writer_funcs *writer;
    GOptionContext *ctx;
    struct output_info uinfo;
    GOptionEntry opts[] = {
        {
            "json", 'j', 0, G_OPTION_ARG_NONE, NULL,
            "output in JSON", NULL
        },
        {
            "ignore-errors", 'i', 0, G_OPTION_ARG_NONE, NULL,
            "ignore errors while extracting", NULL
        },
        {
            "prefix", '\0', 0, G_OPTION_ARG_STRING, NULL,
            "prefix for names in output", "PATH"
        },
        {
            "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
            "show version", NULL
        },
        { NULL }
    };
    i = 0;
    opts[i++].arg_data = &json;
    opts[i++].arg_data = &ignore_errors;
    opts[i++].arg_data = &uinfo.prefix;
    opts[i++].arg_data = &version;

    uinfo.prefix = NULL;
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

    chupa_init(&feeder);
    feeder = chupa_feeder_new();
    writer = json ? &json_writer : &plain_writer;
    uinfo.out = stdout;
    g_signal_connect(feeder, "accepted", (GCallback)writer->output, &uinfo);
    --argc;
    ++argv;
    for (i = 0; i < argc; ++i) {
        GFile *file;
        ChupaData *data;
        file = g_file_new_for_commandline_arg(argv[i]);
        data = chupa_data_new_from_file(NULL, file, &err);
        g_object_unref(file);
        if (!data || !chupa_feeder_feed(feeder, data, &err)) {
            fprintf(stderr, "%s: %s\n", argv[0], err->message);
            g_error_free(err);
            err = NULL;
            if (!data || !ignore_errors) {
                rc = EXIT_FAILURE;
                break;
            }
        }
        g_object_unref(data);
    }
    g_object_unref(feeder);
    chupa_quit();
    return rc;
}
