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

#include "chupa_ruby.h"

void
chupa_ruby_init(void)
{
    VALUE mGLib, mChupa;

    rb_require("chupatext/pre_init");

    chupa_reinit_log_handler();

    mGLib = rb_define_module("GLib");
    chupa_ruby_g_memory_input_stream_init(mGLib);

    mChupa = rb_define_module("Chupa");

    rb_define_const(mChupa, "VERSION",
                    rb_ary_new3(3,
                                INT2FIX(CHUPA_VERSION_MAJOR),
                                INT2FIX(CHUPA_VERSION_MINOR),
                                INT2FIX(CHUPA_VERSION_MICRO)));
    rb_define_const(mChupa, "VERSION_DESCRIPTION",
                    CSTR2RVAL(CHUPA_VERSION_DESCRIPTION));

    chupa_ruby_types_init(mChupa);
    chupa_ruby_metadata_init(mChupa);
    chupa_ruby_data_init(mChupa);
    chupa_ruby_feeder_init(mChupa);
    chupa_ruby_decomposer_init(mChupa);

    rb_require("chupatext/post_init");
}
