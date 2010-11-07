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

#include <glib.h>

#include "chupa_private.h"
#include "chupa_types.h"
#include "chupa_decomposer_factory.h"
#include "chupa_decomposer_description.h"
#include "chupa_logger.h"

static gpointer base_address;

static gboolean initialized = FALSE;

static guint default_log_handler_id = 0;
static guint glib_log_handler_id = 0;
static guint gobject_log_handler_id = 0;
static guint gthread_log_handler_id = 0;
static guint gmodule_log_handler_id = 0;
static guint chupa_log_handler_id = 0;

static void
remove_glib_log_handlers (void)
{
#define REMOVE(domain, prefix)                                          \
    g_log_remove_handler(domain, prefix ## _log_handler_id);            \
    prefix ## _log_handler_id = 0

    REMOVE(NULL, default);
    REMOVE("GLib", glib);
    REMOVE("GLib-GObject", gobject);
    REMOVE("GThread", gthread);
    REMOVE("GModule", gmodule);

#undef REMOVE
}

static void
delegate_glib_log_handlers (void)
{
    default_log_handler_id = CHUPA_GLIB_LOG_DELEGATE(NULL);
    glib_log_handler_id = CHUPA_GLIB_LOG_DELEGATE("GLib");
    gobject_log_handler_id = CHUPA_GLIB_LOG_DELEGATE("GLib-GObject");
    gthread_log_handler_id = CHUPA_GLIB_LOG_DELEGATE("GThread");
    gmodule_log_handler_id = CHUPA_GLIB_LOG_DELEGATE("GModule");
}

void
chupa_reinit_log_handler(void)
{
    remove_glib_log_handlers();
    delegate_glib_log_handlers();
}

void
chupa_init(gpointer address)
{
    base_address = address;

    if (initialized) {
        chupa_reinit_log_handler();
        return;
    }

    initialized = TRUE;

    if (!g_thread_supported())
        g_thread_init(NULL);

    g_type_init();

    chupa_types_init();

    delegate_glib_log_handlers();
    chupa_log_handler_id = CHUPA_GLIB_LOG_DELEGATE("ChupaText");

    chupa_decomposer_factory_init();
    chupa_decomposer_description_init();
}

void
chupa_quit(void)
{
    if (!initialized)
        return;

    chupa_decomposer_description_quit();
    chupa_decomposer_factory_quit();

    remove_glib_log_handlers();
    g_log_remove_handler("ChupaText", chupa_log_handler_id);
    chupa_log_handler_id = 0;

    chupa_types_quit();

    initialized = FALSE;
}

gpointer
chupa_get_base_address(void)
{
    return base_address;
}

