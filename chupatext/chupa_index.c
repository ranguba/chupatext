/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; coding: us-ascii -*- */
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
#include <groonga.h>
#include <gsf/gsf-output-memory.h>

struct table_ctx {
    grn_ctx *ctx;
    grn_obj *table;
};

static const char url[] = "url";

static void
output_to_db(ChupaText *chupar, ChupaTextInput *input, gpointer udata)
{
    struct table_ctx *u = udata;
    grn_ctx *ctx = u->ctx;
    grn_obj *table = u->table;
    GInputStream *inst = G_INPUT_STREAM(chupa_text_input_get_stream(input));
    const char *name = chupa_text_input_get_filename(input);
    const char *charset = chupa_text_input_get_charset(input);
    char buf[4096];
    gssize size;
    grn_id recid;
    grn_obj data;
    int added;

    recid = grn_table_add(ctx, table, name, strlen(name), &added);
#define SET(data)
        grn_obj_set_value(ctx, table, recid, &data, GRN_OBJ_SET);
    if (charset) {
        GRN_SHORT_TEXT_INIT(&data, 0);
        GRN_TEXT_PUTS(ctx, table, charset);
    }
}

/*
    GRN_TABLE_OPEN_OR_CREATE(ctx, , NULL,
                             GRN_OBJ_TABLE_PAT_KEY|GRN_OBJ_PERSISTENT,
                             GRN_DB_SHORT_TEXT);
*/
int
main(int argc, char **argv)
{
    int i;
    int rc = EXIT_SUCCESS;
    const char *dbpath = NULL;
    grn_ctx grnctx_, *grnctx = &grnctx_;
    grn_obj *db;
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
    ctx = g_option_context_new(" db-path input-files...");
    g_option_context_set_description(ctx, "chupatext indexer.");
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
    if (argc < 2) {
        fputs(g_option_context_get_help(ctx, TRUE, NULL), stderr);
        return EXIT_FAILURE;
    }
    dbpath = argv[1];

    chupa_init(&chupar);
    chupar = chupa_text_new();
    grn_ctx_init(grnctx, GRN_CTX_BATCH_MODE);
    if (!GRN_DB_OPEN_OR_CREATE(grnctx, dbpath, 0, db)) {
        fprintf(stderr, "can't open db %s\n", dbpath);
        return EXIT_FAILURE;
    }
    grn_ctx_use(grnctx, db);
    g_signal_connect(chupar, chupa_text_signal_decomposed,
                     (GCallback)output_to_db, grnctx);
    for (i = 2; i < argc; ++i) {
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
