/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 *  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
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

/* FIXME: this API can't handle streaming output.
   We need real Writer object.*/
typedef struct _writer_funcs {
    void (*output)(ChupaData *data, GError *error, gpointer udata);
} writer_funcs;

struct output_info {
    FILE *out;
    const writer_funcs *writer;
};

typedef struct _OutputHeaderData
{
    FILE *output;
    const gchar *original_mime_type;
    const gchar *original_encoding;
    const gchar *original_filename;
    gsize original_content_length;
    GTimeVal *creation_time;
    GTimeVal *modification_time;
} OutputHeaderData;

static void
output_plain_header(ChupaMetadataField *field, gpointer user_data)
{
    OutputHeaderData *data = user_data;
    const gchar *name;
    GString *normalized_name;
    gsize i;
    gboolean capitalize_target = TRUE;

    name = chupa_metadata_field_name(field);
#define EQUAL_NAME(key)                                         \
    chupa_utils_string_equal(name, CHUPA_METADATA_NAME_ ## key)
    if (EQUAL_NAME(PATH) || EQUAL_NAME(MIME_TYPE) || EQUAL_NAME(ENCODING)) {
        return;
    } else if (EQUAL_NAME(ORIGINAL_MIME_TYPE)) {
        data->original_mime_type = chupa_metadata_field_value_string(field);
        return;
    } else if (EQUAL_NAME(ORIGINAL_ENCODING)) {
        data->original_encoding = chupa_metadata_field_value_string(field);
        return;
    } else if (EQUAL_NAME(ORIGINAL_FILENAME)) {
        data->original_filename = chupa_metadata_field_value_string(field);
    } else if (EQUAL_NAME(ORIGINAL_CONTENT_LENGTH)) {
        data->original_content_length = chupa_metadata_field_value_size(field);
    } else if (EQUAL_NAME(CREATION_TIME)) {
        data->creation_time = chupa_metadata_field_value_time_val(field);
    } else if (EQUAL_NAME(MODIFICATION_TIME)) {
        data->modification_time = chupa_metadata_field_value_time_val(field);
    }
#undef EQUAL_NAME

    normalized_name = g_string_new(name);
    for (i = 0; i < normalized_name->len; i++) {
        gchar character = normalized_name->str[i];
        if (capitalize_target && ('a' <= character && character <= 'z')) {
            normalized_name->str[i] += 'A' - 'a';
            capitalize_target = FALSE;
        } else if (character == '-') {
            capitalize_target = TRUE;
        } else {
            capitalize_target = FALSE;
        }
    }
    fprintf(data->output, "%s: %s\n",
            normalized_name->str,
            chupa_metadata_field_value_as_string(field));
    g_string_free(normalized_name, TRUE);
}

static void
output_uri_header(ChupaMetadata *metadata, FILE *output)
{
    const gchar *path;
    gchar *uri;
    GError *conversion_error = NULL;

    path = chupa_metadata_get_path(metadata);
    if (!path) {
        fprintf(output, "URI: (none)\n");
        return;
    }

    uri = g_filename_to_uri(path, NULL, &conversion_error);
    if (conversion_error) {
        g_printerr("failed to convert path to URI: <%s>: <%s>(%d): <%s>",
                   path,
                   g_quark_to_string(conversion_error->domain),
                   conversion_error->code,
                   conversion_error->message);
        g_error_free(conversion_error);
    } else {
        fprintf(output, "URI: %s\n", uri);
        g_free(uri);
    }
}

static void
output_headers(ChupaData *data, FILE *output)
{
    OutputHeaderData header_data;
    ChupaMetadata *metadata;

    memset(&header_data, 0, sizeof(header_data));
    header_data.output = output;

    metadata = chupa_data_get_metadata(data);
    output_uri_header(metadata, output);
    fprintf(output, "Content-Type: text/plain; charset=UTF-8\n");
    chupa_metadata_foreach(metadata, output_plain_header, &header_data);
    if (header_data.original_mime_type) {
        fprintf(output, "Original-Content-Type: %s",
                header_data.original_mime_type);
        if (header_data.original_encoding) {
            fprintf(output, "; charset=%s", header_data.original_encoding);
        }
        fprintf(output, "\n");
    }
    fprintf(output, "Original-Content-Disposition: inline");
    if (header_data.original_filename) {
        fprintf(output, "; filename=%s", header_data.original_filename);
    }
    if (header_data.original_content_length > 0) {
        fprintf(output, "; size=%zd", header_data.original_content_length);
    }
    if (header_data.creation_time) {
        gchar *creation_time_rfc2822;
        creation_time_rfc2822 =
            chupa_utils_format_rfc2822(header_data.creation_time);
        fprintf(output, "; creation-date=%s", creation_time_rfc2822);
        g_free(creation_time_rfc2822);
    }
    if (header_data.modification_time) {
        gchar *modification_time_rfc2822;
        modification_time_rfc2822 =
            chupa_utils_format_rfc2822(header_data.modification_time);
        fprintf(output, "; modification-date=%s", modification_time_rfc2822);
        g_free(modification_time_rfc2822);
    }
    fprintf(output, "\n\n");
}

static void
output_plain(ChupaData *data, GError *error, gpointer user_data)
{
    GInputStream *inst = chupa_data_get_stream(data);
    struct output_info *info = user_data;
    FILE *output = info->out;
    gchar buf[4096];
    gssize size;
    gboolean header_written = FALSE;

    while ((size = g_input_stream_read(inst, buf, sizeof(buf), NULL, NULL)) > 0) {
        if (!header_written) {
            output_headers(data, output);
            header_written = TRUE;
        }
        fwrite(buf, 1, size, output);
    }
    fprintf(output, "\n");
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
output_json(ChupaData *data, GError *error, gpointer udata)
{
    GInputStream *inst = chupa_data_get_stream(data);
    struct output_info *uinfo = udata;
    FILE *out = uinfo->out;
    const char *name = chupa_data_get_filename(data);
    const char *charset = chupa_data_get_charset(data);
    char buf[4096];
    gssize size;

    fputs("{\n\"_key\":\"", out);
    quote(name, out);
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

static void
output(ChupaFeeder *feeder, ChupaData *data, gpointer udata)
{
    struct output_info *info = udata;

    g_signal_connect(data, "finished", (GCallback)(info->writer->output), info);
}

static const writer_funcs plain_writer = {output_plain};
static const writer_funcs json_writer = {output_json};

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
    GOptionContext *ctx;
    struct output_info uinfo;
    GOptionEntry opts[] = {
/* DISABLED.
        {
            "json", 'j', 0, G_OPTION_ARG_NONE, NULL,
            "output in JSON", NULL
        },
*/
        {
            "ignore-errors", 'i', 0, G_OPTION_ARG_NONE, NULL,
            "ignore errors while extracting", NULL
        },
        {
            "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
            "show version", NULL
        },
        { NULL }
    };
    i = 0;
    /* opts[i++].arg_data = &json; */
    opts[i++].arg_data = &ignore_errors;
    opts[i++].arg_data = &version;

    g_type_init();
    ctx = g_option_context_new("FILE...");
    g_option_context_set_description(ctx, "A text and metadata extractor.");
    g_option_context_set_help_enabled(ctx, TRUE);
    g_option_context_add_main_entries(ctx, opts, NULL);
    if (!g_option_context_parse(ctx, &argc, &argv, &err)) {
        g_printerr("Failed to initialize: %s\n", err->message);
        g_error_free(err);
        return EXIT_FAILURE;
    }
    if (version) {
        puts(chupa_version_description());
        return EXIT_SUCCESS;
    }

    chupa_init(&feeder);
    feeder = chupa_feeder_new();
    uinfo.out = stdout;
    uinfo.writer = json ? &json_writer : &plain_writer;
    g_signal_connect(feeder, "accepted", (GCallback)output, &uinfo);
    --argc;
    ++argv;
    for (i = 0; i < argc; ++i) {
        GFile *file;
        ChupaData *data;
        file = g_file_new_for_commandline_arg(argv[i]);
        data = chupa_data_new_from_file(file, NULL, &err);
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
