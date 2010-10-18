/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef CHUPA_FEEDER_H
#define CHUPA_FEEDER_H

#include <gio/gio.h>
#include <glib-object.h>
#include <chupatext/chupa_data.h>

G_BEGIN_DECLS

#define CHUPA_FEEDER_ERROR chupa_feeder_error_quark()

#define CHUPA_TYPE_FEEDER            chupa_feeder_get_type()
#define CHUPA_FEEDER(obj)            G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_FEEDER, ChupaFeeder)
#define CHUPA_FEEDER_CLASS(klass)    G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_FEEDER, ChupaFeederClass)
#define IS_CHUPA_FEEDER(obj)         G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_FEEDER)
#define IS_CHUPA_FEEDER_CLASS(klass) G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_FEEDER)
#define CHUPA_FEEDER_GET_CLASS(obj)  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_FEEDER, ChupaFeederClass)

#define CHUPA_FEEDER_SIGNAL_DECOMPOSED chupa_feeder_signal_decomposed
extern const char chupa_feeder_signal_decomposed[];

typedef struct ChupaFeeder ChupaFeeder;
typedef struct ChupaFeederClass ChupaFeederClass;

/**
 * ChupaFeeder:
 *
 * The class to decompose an input and extract feeder portions.
 */
struct ChupaFeeder
{
    GObject parent_instance;
};

/**
 * ChupaFeederClass:
 *
 * @decomposed: the callback function called when each feeder portion is
 * found.
 */
struct ChupaFeederClass
{
    GObjectClass parent_class;

    /* signals */
    void (*decomposed)(GObject *object, ChupaData *data);
};

typedef enum {
    CHUPA_FEEDER_ERROR_NONE,
    CHUPA_FEEDER_ERROR_UNKNOWN_CONTENT,
    CHUPA_FEEDER_ERROR_UNKNOWN_MIME_TYPE,
    CHUPA_FEEDER_ERROR_INVALID_INPUT,
    CHUPA_FEEDER_ERROR_UNKNOWN
} ChupaFeederError;

/**
 * ChupaFeederCallback:
 *
 * The type used for callback functions when extracting the feeder portion.
 */
typedef void (*ChupaFeederCallback)(ChupaFeeder *, ChupaData *, gpointer);

GQuark       chupa_feeder_error_quark(void) G_GNUC_CONST;

GType        chupa_feeder_get_type   (void) G_GNUC_CONST;
ChupaFeeder *chupa_feeder_new        (void);
gboolean     chupa_feeder_feed       (ChupaFeeder *feeder, ChupaData *data, GError **err);
void         chupa_feeder_decomposed (ChupaFeeder *feeder, ChupaData *data);
void         chupa_feeder_decompose  (ChupaFeeder *feeder, ChupaData *feeder_data,
                                      ChupaFeederCallback func, gpointer arg, GError **error);
gchar       *chupa_feeder_decompose_all(ChupaFeeder *feeder, ChupaData *feeder_data, GError **error);

#endif
