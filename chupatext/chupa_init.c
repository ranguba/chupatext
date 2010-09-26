/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include <glib.h>

#include "chupa_private.h"
#include "chupa_module_factory_utils.h"

int
chupa_init(void *var)
{
    g_type_init();
    chupa_module_factory_init();
    chupa_module_factory_load(chupa_module_factory_get_module_dir(),
                              "decomposer");
    return 0;
}

int
chupa_cleanup(void)
{
    chupa_module_factory_quit();
    return 0;
}
