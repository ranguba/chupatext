/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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
#include <gobject/gvaluecollector.h>

#include "chupa_types.h"

GType chupa_type_size;
GType chupa_type_time_val;
GType chupa_type_param_time_val;

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


static void
size_type_init (void)
{
    GTypeInfo info = {
        0,			/* class_size */
        NULL,			/* base_init */
        NULL,			/* base_destroy */
        NULL,			/* class_init */
        NULL,			/* class_destroy */
        NULL,			/* class_data */
        0,			/* instance_size */
        0,			/* n_preallocs */
        NULL,			/* instance_init */
        NULL,			/* value_table */
    };
    const GTypeFundamentalInfo finfo = { G_TYPE_FLAG_DERIVABLE, };
    const GTypeValueTable value_table = {
        value_init_size,	/* value_init */
        NULL,			/* value_free */
        value_copy_size,	/* value_copy */
        NULL,			/* value_peek_pointer */
        "l",			/* collect_format */
        value_collect_size,	/* collect_value */
        "p",			/* lcopy_format */
        value_lcopy_size,	/* lcopy_value */
    };
    info.value_table = &value_table;
    chupa_type_size =
        g_type_register_fundamental(g_type_fundamental_next(),
                                    g_intern_static_string("gsize"),
                                    &info,
                                    &finfo,
                                    0);
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

static void
time_val_type_init (void)
{
    GTypeInfo info = {
        0,			/* class_size */
        NULL,			/* base_init */
        NULL,			/* base_destroy */
        NULL,			/* class_init */
        NULL,			/* class_destroy */
        NULL,			/* class_data */
        0,			/* instance_size */
        0,			/* n_preallocs */
        NULL,			/* instance_init */
        NULL,			/* value_table */
    };
    const GTypeFundamentalInfo finfo = { G_TYPE_FLAG_DERIVABLE, };
    const GTypeValueTable value_table = {
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
    chupa_type_time_val =
        g_type_register_fundamental(g_type_fundamental_next(),
                                    g_intern_static_string("GTimeVal"),
                                    &info,
                                    &finfo,
                                    0);
}

GTimeVal *
chupa_value_get_time_val (const GValue *value)
{
    g_return_val_if_fail(CHUPA_VALUE_HOLDS_TIME_VAL(value), NULL);

    return value->data[0].v_pointer;
}

void
chupa_value_set_time_val (GValue *value, GTimeVal *time_value)
{
    g_return_if_fail(CHUPA_VALUE_HOLDS_TIME_VAL(value));

    if (value->data[1].v_uint & G_VALUE_NOCOPY_CONTENTS) {
        value->data[1].v_uint = 0;
    } else {
        g_free(value->data[0].v_pointer);
    }

    value->data[0].v_pointer = chupa_time_val_dup(time_value);
}

static void
param_time_val_init (GParamSpec *pspec)
{
}

static void
param_time_val_finalize (GParamSpec *pspec)
{
}

static void
param_time_val_set_default (GParamSpec *pspec,
                            GValue     *value)
{
    value->data[0].v_pointer = NULL;
}

static gboolean
param_time_val_validate (GParamSpec *pspec,
                         GValue     *value)
{
    gboolean changed = FALSE;

    return changed;
}

static gint
param_time_val_values_cmp (GParamSpec   *pspec,
                           const GValue *value1,
                           const GValue *value2)
{
    GTimeVal *time_value1, *time_value2;

    time_value1 = value1->data[0].v_pointer;
    time_value2 = value2->data[0].v_pointer;
    if (!time_value1) {
        return time_value2 != NULL ? -1 : 0;
    } else if (!time_value2) {
        return time_value1 != NULL;
    } else {
        if (time_value1->tv_sec > time_value2->tv_sec) {
            return 1;
        } else if (time_value1->tv_sec == time_value2->tv_sec) {
            if (time_value1->tv_usec > time_value2->tv_usec) {
                return 1;
            } else if (time_value1->tv_usec == time_value2->tv_usec) {
                return 0;
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
}

static void
param_time_val_type_init (void)
{
    GParamSpecTypeInfo pspec_info = {
        sizeof(ChupaParamSpecTimeVal),	/* instance_size */
        16,				/* n_preallocs */
        param_time_val_init,		/* instance_init */
        0,				/* value_type */
        param_time_val_finalize,	/* finalize */
        param_time_val_set_default,	/* value_set_default */
        param_time_val_validate,	/* value_validate */
        param_time_val_values_cmp,	/* values_cmp */
    };
    pspec_info.value_type = CHUPA_TYPE_TIME_VAL;
    chupa_type_param_time_val =
        g_param_type_register_static(g_intern_static_string("GParamTimeVal"),
                                     &pspec_info);
}

GTimeVal *
chupa_time_val_dup (GTimeVal *time_value)
{
    GTimeVal *new_time_value;

    if (!time_value)
        return NULL;

    new_time_value = g_new0(GTimeVal, 1);
    new_time_value->tv_sec = time_value->tv_sec;
    new_time_value->tv_usec = time_value->tv_usec;

    return new_time_value;
}

GParamSpec *
chupa_param_spec_time_val (const gchar *name,
                           const gchar *nick,
                           const gchar *blurb,
                           GParamFlags  flags)
{
    ChupaParamSpecTimeVal *pspec;

    pspec = g_param_spec_internal(CHUPA_TYPE_PARAM_TIME_VAL,
                                  name,
                                  nick,
                                  blurb,
                                  flags);

    return G_PARAM_SPEC(pspec);
}

void
chupa_types_init (void)
{
    size_type_init();
    time_val_type_init();
    param_time_val_type_init();
}

void
chupa_types_quit (void)
{
}
