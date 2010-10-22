/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
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

#ifndef CHUPA_DATA_INPUT_H
#define CHUPA_DATA_INPUT_H

#include <glib-object.h>
#include <gsf/gsf-input.h>
#include <chupatext/chupa_data.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_DATA_INPUT chupa_data_input_get_type()
#define CHUPA_DATA_INPUT(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_DATA_INPUT, ChupaDataInput)
#define CHUPA_DATA_INPUT_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_DATA_INPUT, ChupaDataInputClass)
#define CHUPA_IS_DATA_INPUT(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_DATA_INPUT)
#define CHUPA_IS_DATA_INPUT_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_DATA_INPUT)
#define CHUPA_DATA_INPUT_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_DATA_INPUT, ChupaDataInputClass)

#define CHUPA_DATA_INPUT_SIGNAL_FINISHED chupa_data_input_signal_finished

typedef struct _ChupaDataInput ChupaDataInput;
typedef struct _ChupaDataInputClass ChupaDataInputClass;

GType          chupa_data_input_get_type         (void) G_GNUC_CONST;
GsfInput      *chupa_data_input_new              (ChupaData *data);

G_END_DECLS

#endif  /* CHUPA_DATA_INPUT_H */
