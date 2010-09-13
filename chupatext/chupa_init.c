/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_private.h"
#include <glib.h>

int
chupa_init(void *var)
{
    g_type_init();
    chupa_decomposer_load_modules();
    return 0;
}

int
chupa_cleanup(void)
{
    return 0;
}
