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
    VALUE mGsf, mChupa;

    rb_require("chupa/pre_init");

    mGsf = rb_define_module("Gsf");
    chupa_ruby_gsf_output_init(mGsf);

    mChupa = rb_define_module("Chupa");
    chupa_ruby_metadata_init(mChupa);
    chupa_ruby_text_input_init(mChupa);
    chupa_ruby_feeder_init(mChupa);

    rb_require("chupa/post_init");
}
