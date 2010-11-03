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

#include "chupa_archive_decomposer.h"
#include "chupa_data_input.h"
#include "chupa_gsf_input_stream.h"

G_DEFINE_ABSTRACT_TYPE(ChupaArchiveDecomposer,          \
                       chupa_archive_decomposer,        \
                       CHUPA_TYPE_DECOMPOSER)

static gboolean
feed(ChupaDecomposer *decomposer, ChupaFeeder *feeder,
     ChupaData *data, GError **error)
{
    GsfInfile *infile;
    gboolean result = TRUE;
    ChupaArchiveDecomposer *archive_decomposer;
    int i, num;

    archive_decomposer = CHUPA_ARCHIVE_DECOMPOSER(decomposer);
    infile = chupa_archive_decomposer_get_infile(archive_decomposer, data, error);
    if (!infile) {
        return FALSE;
    }
    num = gsf_infile_num_children(infile);
    for (i = 0; i < num; i++) {
        const gchar *name;
        GsfInput *input;
        ChupaGsfInputStream *stream;
        ChupaData *data;
        ChupaMetadata *metadata;

        input = gsf_infile_child_by_index(infile, i);
        stream = chupa_gsf_input_stream_new(input);
        g_object_unref(input);
        metadata = chupa_metadata_new();
        name = gsf_infile_name_by_index(infile, i);
        chupa_metadata_set_string(metadata, "filename", name);
        data = chupa_data_new(G_INPUT_STREAM(stream), metadata);
        g_object_unref(stream);
        g_object_unref(metadata);
        chupa_feeder_feed(feeder, data, error);
        g_object_unref(data);
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
    ChupaDecomposerClass *decomposer_class = CHUPA_DECOMPOSER_CLASS(klass);

    decomposer_class->feed = feed;
}

GsfInfile *
chupa_archive_decomposer_get_infile(ChupaArchiveDecomposer *decomposer,
                                    ChupaData *data,
                                    GError **error)
{
    ChupaArchiveDecomposerClass *klass;
    GsfInput *input;
    GsfInfile *infile;

    klass = CHUPA_ARCHIVE_DECOMPOSER_GET_CLASS(decomposer);
    input = chupa_data_input_new(data);
    infile = klass->get_infile(decomposer, input, error);
    g_object_unref(input);
    return infile;
}
