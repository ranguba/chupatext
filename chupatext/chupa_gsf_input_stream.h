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

#include <glib.h>
#include <gio/gio.h>
#include <gsf/gsf-output-memory.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_GSF_INPUT_STREAM            chupa_gsf_input_stream_get_type()
#define CHUPA_GSF_INPUT_STREAM(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_GSF_INPUT_STREAM, ChupaGsfInputStream)
#define CHUPA_GSF_INPUT_STREAM_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_GSF_INPUT_STREAM, ChupaGsfInputStreamClass)
#define CHUPA_IS_GSF_INPUT_STREAM(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_GSF_INPUT_STREAM)
#define CHUPA_IS_GSF_INPUT_STREAM_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_GSF_INPUT_STREAM)
#define CHUPA_GSF_INPUT_STREAM_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_GSF_INPUT_STREAM, ChupaGsfInputStreamClass)

typedef struct _ChupaGsfInputStream ChupaGsfInputStream;
typedef struct _ChupaGsfInputStreamClass ChupaGsfInputStreamClass;

GType chupa_gsf_input_stream_get_type(void);
GInputStream *chupa_gsf_input_stream_new(GsfOutputMemory *);

G_END_DECLS
