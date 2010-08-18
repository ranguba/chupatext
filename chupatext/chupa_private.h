/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#ifndef CHUPA_PRIVATE_H
#define CHUPA_PRIVATE_H

#include <chupatext.h>
#include <chupatext/chupa_module.h>

typedef struct ChupaConverter ChupaConverter;
typedef struct ChupaConverterClass ChupaConverterClass;

struct ChupaConverter {
    const ChupaModule *module;
};

#endif
