/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/text_decomposer.h"

G_DEFINE_TYPE(ChupaTextDecomposer, chupa_text_decomposer, CHUPA_TYPE_DECOMPOSER)

static void
chupa_text_decomposer_init(ChupaTextDecomposer *text_decomposer)
{
}

static void
chupa_text_decomposer_feed(ChupaDecomposer *dec, ChupaText *text, ChupaTextInputStream *stream)
{
    chupa_text_decomposed(text, stream);
}

static void
chupa_text_decomposer_class_init(ChupaTextDecomposerClass *klass)
{
    ChupaDecomposerClass *super = CHUPA_DECOMPOSER_CLASS(klass);
    super->feed = chupa_text_decomposer_feed;
}
