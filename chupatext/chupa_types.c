/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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
#include <gobject/gvaluecollector.h>

#include "chupa_types.h"

static void
value_init_size (GValue *value)
{
#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    value->data[0].v_ulong = 0;
#else
    value->data[0].v_uint64 = 0;
#endif
}

static void
value_copy_size (const GValue *src_value,
                 GValue       *dest_value)
{
#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    dest_value->data[0].v_ulong = src_value->data[0].v_ulong;
#else
    dest_value->data[0].v_uint64 = src_value->data[0].v_uint64;
#endif
}

static gchar*
value_collect_size (GValue      *value,
                    guint        n_collect_values,
                    GTypeCValue *collect_values,
                    guint        collect_flags)
{
#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    value->data[0].v_long = (collect_values[0]).v_long;
#else
    value->data[0].v_int64 = collect_values[0].v_int64;
#endif

  return NULL;
}

static gchar*
value_lcopy_size (const GValue *value,
                  guint         n_collect_values,
                  GTypeCValue  *collect_values,
                  guint         collect_flags)
{
    gsize *size_p = collect_values[0].v_pointer;

    if (!size_p) {
        return g_strdup_printf("value location for `%s' passed as NULL",
                               G_VALUE_TYPE_NAME(value));
    }

#if GLIB_SIZEOF_LONG == GLIB_SIZEOF_SIZE_T
    *size_p = value->data[0].v_ulong;
#else
    *size_p = value->data[0].v_uint64;
#endif

    return NULL;
}


GType
chupa_size_get_type (void)
{
    static GType type_id = 0;

    if (!type_id) {
        GTypeInfo info = {
            0,				/* class_size */
            NULL,			/* base_init */
            NULL,			/* base_destroy */
            NULL,			/* class_init */
            NULL,			/* class_destroy */
            NULL,			/* class_data */
            0,				/* instance_size */
            0,				/* n_preallocs */
            NULL,			/* instance_init */
            NULL,			/* value_table */
        };
        const GTypeFundamentalInfo finfo = { G_TYPE_FLAG_DERIVABLE, };
        static const GTypeValueTable value_table = {
            value_init_size,		/* value_init */
            NULL,			/* value_free */
            value_copy_size,		/* value_copy */
            NULL,			/* value_peek_pointer */
            "l",			/* collect_format */
            value_collect_size,		/* collect_value */
            "p",			/* lcopy_format */
            value_lcopy_size,		/* lcopy_value */
        };
        info.value_table = &value_table;
        type_id = g_type_register_fundamental(g_type_fundamental_next(),
                                              g_intern_static_string("gsize"),
                                              &info,
                                              &finfo,
                                              0);
    }

    return type_id;
}

static void
value_init_time_val (GValue *value)
{
    value->data[0].v_pointer = NULL;
}

static void
value_free_time_val (GValue *value)
{
  if (!(value->data[1].v_uint & G_VALUE_NOCOPY_CONTENTS))
    g_free(value->data[0].v_pointer);
}

static void
value_copy_time_val (const GValue *src_value,
                     GValue       *dest_value)
{
    dest_value->data[0].v_pointer =
        chupa_time_val_dup(src_value->data[0].v_pointer);
}

static gpointer
value_peek_time_val (const GValue *value)
{
  return value->data[0].v_pointer;
}

static gchar*
value_collect_time_val (GValue      *value,
                        guint        n_collect_values,
                        GTypeCValue *collect_values,
                        guint        collect_flags)
{
    if (!collect_values[0].v_pointer) {
        value->data[0].v_pointer = NULL;
    } else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
        value->data[0].v_pointer = collect_values[0].v_pointer;
        value->data[1].v_uint = G_VALUE_NOCOPY_CONTENTS;
    } else {
        value->data[0].v_pointer =
            chupa_time_val_dup(collect_values[0].v_pointer);
    }

    return NULL;
}

static gchar*
value_lcopy_time_val (const GValue *value,
                      guint         n_collect_values,
                      GTypeCValue  *collect_values,
                      guint         collect_flags)
{
    GTimeVal **time_value_p = collect_values[0].v_pointer;

    if (!time_value_p)
        return g_strdup_printf("value location for `%s' passed as NULL",
                               G_VALUE_TYPE_NAME(value));

    if (!value->data[0].v_pointer) {
        *time_value_p = NULL;
    } else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
        *time_value_p = value->data[0].v_pointer;
    } else {
        *time_value_p = chupa_time_val_dup(value->data[0].v_pointer);
    }

    return NULL;
}

GType
chupa_time_val_get_type (void)
{
    static GType type_id = 0;

    if (!type_id) {
        GTypeInfo info = {
            0,				/* class_size */
            NULL,			/* base_init */
            NULL,			/* base_destroy */
            NULL,			/* class_init */
            NULL,			/* class_destroy */
            NULL,			/* class_data */
            0,				/* instance_size */
            0,				/* n_preallocs */
            NULL,			/* instance_init */
            NULL,			/* value_table */
        };
        const GTypeFundamentalInfo finfo = { G_TYPE_FLAG_DERIVABLE, };
        static const GTypeValueTable value_table = {
            value_init_time_val,	/* value_init */
            value_free_time_val,	/* value_free */
            value_copy_time_val,	/* value_copy */
            value_peek_time_val,	/* value_peek_pointer */
            "p",			/* collect_format */
            value_collect_time_val,	/* collect_value */
            "p",			/* lcopy_format */
            value_lcopy_time_val,	/* lcopy_value */
        };
        info.value_table = &value_table;
        type_id = g_type_register_fundamental(g_type_fundamental_next(),
                                              g_intern_static_string("GTimeVal"),
                                              &info,
                                              &finfo,
                                              0);
    }

    return type_id;
}

GTimeVal *
chupa_time_val_dup (GTimeVal *time_value)
{
    GTimeVal *new_time_value;

    new_time_value = g_new0(GTimeVal, 1);
    new_time_value->tv_sec = time_value->tv_sec;
    new_time_value->tv_usec = time_value->tv_usec;

    return new_time_value;
}

