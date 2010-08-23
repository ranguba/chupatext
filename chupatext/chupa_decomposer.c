/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_decomposer.h"
#include <glib.h>
#include <string.h>

#define CHUPA_DECOMPOSER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_DECOMPOSER_OBJECT, \
                                 ChupaDecomposerPrivate))

static GHashTable *decomp_modules = NULL;

#define module_list_free (GDestroyNotify)g_list_free

static void
decomp_modules_init(void)
{
    decomp_modules = g_hash_table_new_full(g_str_hash, g_str_equal,
                                           NULL, module_list_free);
}

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
typedef struct _ChupaDecomposerPrivate  ChupaDecomposerPrivate;
struct _ChupaDecomposerPrivate
{
};
#endif

G_DEFINE_ABSTRACT_TYPE_WITH_CODE(ChupaDecomposer, chupa_decomposer, G_TYPE_OBJECT,
                                 decomp_modules_init())

enum {
    PROP_0,
    PROP_SOURCE,
    PROP_DUMMY
};

static void
chupa_decomposer_init(ChupaDecomposer *decomposer)
{
}

static void
chupa_decomposer_class_init(ChupaDecomposerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    /*GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS(klass);*/

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
    g_type_class_add_private(gobject_class, sizeof(ChupaDecomposerPrivate));
#endif
}

void
chupa_decomposer_feed(ChupaDecomposer *dec, ChupaText *text, ChupaTextInputStream *stream)
{
    g_return_if_fail(CHUPA_IS_DECOMPOSER(dec));
    (*CHUPA_DECOMPOSER_GET_CLASS(dec)->feed)(dec, text, stream);
}

void
chupa_decomposer_load_modules(void)
{
}

void
chupa_decomposer_register(const gchar *mime_type, GType type)
{
    gpointer key = (gpointer)mime_type, val = NULL;

    if (g_hash_table_lookup_extended(decomp_modules, key, &key, &val)) {
        g_hash_table_steal(decomp_modules, key);
    }
    val = g_list_prepend((GList *)val, (gpointer)type);
    g_hash_table_insert(decomp_modules, key, val);
}

void
chupa_decomposer_unregister(const gchar *mime_type, GType type)
{
    gpointer key = (gpointer)mime_type, val = NULL;

    if (g_hash_table_lookup_extended(decomp_modules, key, &key, &val)) {
        val = g_list_remove((GList *)val, (gpointer)type);
        if (val) {
            g_hash_table_steal(decomp_modules, key);
            g_hash_table_insert(decomp_modules, key, val);
        }
        else {
            g_hash_table_remove(decomp_modules, key);
        }
    }
}

ChupaDecomposerClass *
chupa_decomposer_search(GInputStream *stream)
{
    GList *type_list = NULL;
    ChupaTextInputStream *tis = chupa_text_input_stream_new(NULL, stream);
    ChupaMetadata *meta = chupa_text_input_stream_get_metadata(tis);
    const gchar *mime_type = chupa_metadata_get_first_value(meta, "mime-type");
    const gchar *slash;
    gpointer key, value;

    if (g_hash_table_lookup_extended(decomp_modules, mime_type, &key, &value)) {
        type_list = (GList *)value;
    }
    else if ((slash = strchr((const char *)mime_type, '/')) != NULL) {
        GString *tmp_type = g_string_new_len(mime_type, slash + 1 - mime_type);
        g_string_append_c(tmp_type, '*');
        if (g_hash_table_lookup_extended(decomp_modules, tmp_type->str, &key, &value)) {
            type_list = (GList *)value;
        }
        g_string_free(tmp_type, TRUE);
    }

    return type_list ?
        CHUPA_DECOMPOSER_CLASS(g_type_class_ref((GType)type_list->data)) :
        NULL;
}
