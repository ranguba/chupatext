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

#ifndef CHUPA_TYPES_H
#define CHUPA_TYPES_H

#include <glib-object.h>

G_BEGIN_DECLS

void       chupa_types_init                (void);
void       chupa_types_quit                (void);

typedef struct _ChupaParamSpecTimeVal      ChupaParamSpecTimeVal;
struct _ChupaParamSpecTimeVal
{
  GParamSpec    parent_instance;
};

#define	    CHUPA_TYPE_SIZE                (chupa_size_get_type())
GType       chupa_size_get_type            (void) G_GNUC_CONST;

#define     CHUPA_VALUE_HOLDS_TIME_VAL(value) \
    (G_TYPE_CHECK_VALUE_TYPE((value), CHUPA_TYPE_TIME_VAL))
#define	    CHUPA_TYPE_TIME_VAL            (chupa_time_val_get_type())
GType       chupa_time_val_get_type        (void) G_GNUC_CONST;

GTimeVal   *chupa_value_get_time_val       (const GValue *value);
void        chupa_value_set_time_val       (GValue       *value,
                                            GTimeVal     *time_value);

#define     CHUPA_PARAM_SPEC_TIME_VAL(pspec)                    \
    (G_TYPE_CHECK_INSTANCE_CAST((pspec),                        \
                                CHUPA_TYPE_PARAM_TIME_VAL,      \
                                ChupaParamSpecTimeVal))
#define	    CHUPA_TYPE_PARAM_TIME_VAL      (chupa_param_time_val_get_type())
GType       chupa_param_time_val_get_type  (void) G_GNUC_CONST;

GTimeVal   *chupa_time_val_dup             (GTimeVal *time_value);

GParamSpec *chupa_param_spec_time_val      (const gchar *name,
                                            const gchar *nick,
                                            const gchar *blurb,
                                            GParamFlags  flags);


G_END_DECLS

#endif
