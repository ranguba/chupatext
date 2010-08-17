/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <chupatext.h>

static GInputStream *
chupa_text_make_converter_input(GInputStream *input)
{
    return input;
}

static const ChupaExtractor
chupa_text_extactor = {
    {"text", NULL},
    chupa_text_make_converter_input,
    NULL,
};

void
chupa_module_text_init(void)
{
    chupa_regsiter_extactor(&chupa_text_extactor);
}
