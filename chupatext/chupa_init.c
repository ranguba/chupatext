/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupa_private.h"
#include <glib.h>

int
chupa_init(void *var)
{
    chpua_module_load_module();
    return 0;
}

int
chupa_cleanup(void)
{
}
