/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
 */

#include "chupatext/chupa_decomposer.h"
#include "chupatext/chupa_module.h"
#include "chupatext/text_decomposer.h"
#include <glib.h>
#include <string.h>

#define CHUPA_DECOMPOSER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE(obj, \
                                 CHUPA_TYPE_DECOMPOSER_OBJECT, \
                                 ChupaDecomposerPrivate))

static GHashTable *decomp_modules = NULL;
static GHashTable *decomp_load_table = NULL;
static gchar *module_base_dir;

#define module_list_free (GDestroyNotify)g_list_free

static void
decomp_modules_init(void)
{
    decomp_modules = g_hash_table_new_full(g_str_hash, g_str_equal,
                                           NULL, module_list_free);
    decomp_load_table = g_hash_table_new(g_str_hash, g_str_equal);
    module_base_dir = chupa_module_path();

#define REGISTER(mime_type, module) \
    g_hash_table_insert(decomp_load_table, mime_type, module)
#include "module/init_chupa.h"
#undef REGISTER
}

#ifdef USE_CHUPA_DECOMPOSER_PRIVATE
typedef struct _ChupaDecomposerPrivate  ChupaDecomposerPrivate;
struct _ChupaDecomposerPrivate
{
};
#endif

G_DEFINE_ABSTRACT_TYPE_WITH_CODE(ChupaDecomposer, chupa_decomposer, G_TYPE_OBJECT,
                                 decomp_modules_init())

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

gboolean
chupa_decomposer_feed(ChupaDecomposer *dec, ChupaText *text, ChupaTextInput *input, GError **err)
{
    g_return_val_if_fail(CHUPA_IS_DECOMPOSER(dec), FALSE);
    return (*CHUPA_DECOMPOSER_GET_CLASS(dec)->feed)(dec, text, input, err);
}

static const char text_plain[] = "text/plain";

void
chupa_decomposer_load_modules(void)
{
    chupa_decomposer_register(text_plain, CHUPA_TYPE_TEXT_DECOMPOSER);
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
        g_hash_table_steal(decomp_modules, key);
        val = g_list_remove((GList *)val, (gpointer)type);
        if (val) {
            g_hash_table_insert(decomp_modules, key, val);
        }
    }
}

ChupaDecomposer *
chupa_decomposer_search(const gchar *const mime_type)
{
    GList *type_list = NULL;
    const char *sub_type;
    gpointer key, value;
    ChupaModule *mod = NULL;
    int retry = 0;

    do {
        if (g_hash_table_lookup_extended(decomp_modules, mime_type, &key, &value)) {
            type_list = (GList *)value;
        }
        else if (g_hash_table_lookup_extended(decomp_load_table, mime_type, &key, &value) &&
                 (mod = chupa_module_load_module(module_base_dir, (const gchar *)value)) != NULL) {
            if (!g_type_module_use(G_TYPE_MODULE(mod))) {
                break;
            }
        }
        else if ((sub_type = strchr((const char *)mime_type, '/')) != NULL) {
            GString *tmp_type = NULL;
            ++sub_type;
            if (sub_type[0] == 'x' && sub_type[1] == '-') {
                tmp_type = g_string_new_len(mime_type, sub_type - mime_type);
                g_string_append(tmp_type, sub_type += 2);
                if (g_hash_table_lookup_extended(decomp_modules, tmp_type->str, &key, &value)) {
                    type_list = (GList *)value;
                }
            }
            if (!type_list && tmp_type &&
                g_hash_table_lookup_extended(decomp_load_table, tmp_type->str, &key, &value)) {
                mod = chupa_module_load_module(module_base_dir, (const gchar *)value);
            }
            g_string_free(tmp_type, TRUE);
            if (!mod || !g_type_module_use(G_TYPE_MODULE(mod))) {
                break;
            }
        }
    } while (!type_list && !retry++);

    if (!type_list) {
        return NULL;
    }
    return CHUPA_DECOMPOSER(g_object_new((GType)type_list->data, NULL));
}
