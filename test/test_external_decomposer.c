/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/external_decomposer.h"
#include "chupa_test_util.h"
#include <glib.h>

#define CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER chupa_test_external_decomposer_get_type()
#define CHUPA_TEST_EXTERNAL_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER, ChupaTestExternalDecomposer)
#define CHUPA_TEST_EXTERNAL_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER, ChupaTestExternalDecomposerClass)
#define CHUPA_TEST_IS_EXTERNAL_DECOMPOSER(obj) \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER)
#define CHUPA_TEST_IS_EXTERNAL_DECOMPOSER_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER)
#define CHUPA_TEST_EXTERNAL_DECOMPOSER_GET_CLASS(obj) \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER, ChupaTestExternalDecomposerClass)

typedef struct _ChupaTestExternalDecomposer ChupaTestExternalDecomposer;
typedef struct _ChupaTestExternalDecomposerClass ChupaTestExternalDecomposerClass;
typedef struct _ChupaTestExternalDecomposerPrivate ChupaTestExternalDecomposerPrivate;

struct _ChupaTestExternalDecomposer
{
    ChupaExternalDecomposer object;
};

struct _ChupaTestExternalDecomposerClass
{
    ChupaExternalDecomposerClass parent_class;
};

G_DEFINE_TYPE(ChupaTestExternalDecomposer, chupa_test_external_decomposer, CHUPA_TYPE_EXTERNAL_DECOMPOSER)

#define A_LINE "abcdefghijklmnopqrstuvwxyz-0123456789"
#define DOUBLE(x) x x
#define LONG_LINE DOUBLE(DOUBLE(DOUBLE(DOUBLE(A_LINE))))
static const char plain_text[] = "plain text\n"
    "foo bar\n"
    "\fzot\n"
    DOUBLE(DOUBLE(DOUBLE(LONG_LINE "\n")))
    ;

static gboolean
testdec_spawn(ChupaExternalDecomposer *dec, ChupaText *chupar,
              GOutputStream **stdinput, GInputStream **stdoutput,
              GError **error)
{
    gchar *argv[2];
    argv[0] = "cat";
    argv[1] = NULL;
    return chupa_external_decomposer_spawn(dec, argv, stdinput, stdoutput, error);
}

static void
chupa_test_external_decomposer_init(ChupaTestExternalDecomposer *dec)
{
}

static void
chupa_test_external_decomposer_class_init(ChupaTestExternalDecomposerClass *klass)
{
    ChupaExternalDecomposerClass *extdec_class = CHUPA_EXTERNAL_DECOMPOSER_CLASS(klass);
    extdec_class->spawn = testdec_spawn;
}

static const char test_external_type[] = "application/x-chupa-test-external";

void
setup(void)
{
    chupa_test_setup();
    cut_omit("chupa_decomposer_register() was deleted");
    /* chupa_decomposer_register(test_external_type, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER); */
}

void
teardown(void)
{
    /* chupa_decomposer_unregister(test_external_type, CHUPA_TEST_TYPE_EXTERNAL_DECOMPOSER); */
    chupa_test_teardown();
}

void
test_decompose_external(void)
{
    ChupaTextInput *input = chupa_test_decomposer_from_data(plain_text, sizeof(plain_text) - 1, NULL);

    cut_assert_not_null(input);
    chupa_metadata_replace_value(chupa_text_input_get_metadata(input),
                                 "mime-type", test_external_type);
    cut_assert(CHUPA_TEST_IS_EXTERNAL_DECOMPOSER(gcut_take_object(G_OBJECT(chupa_decomposer_search(test_external_type)))));
    cut_assert_equal_string(plain_text, chupa_test_decompose_all(input, NULL));
}
