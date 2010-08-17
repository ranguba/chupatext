#ifndef CHUPA_METADATA_H
#define CHUPA_METADATA_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_METADATA            chupa_metadata_get_type()
#define CHUPA_METADATA(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_METADATA, ChupaMetadata)
#define CHUPA_METADATA_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_METADATA, ChupaMetadataClass)
#define CHUPA_IS_METADATA(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_METADATA)
#define CHUPA_IS_METADATA_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_METADATA)
#define CHUPA_METADATA_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_METADATA, ChupaMetadataClass)

typedef struct _ChupaMetadata ChupaMetadata;
typedef struct _ChupaMetadataClass ChupaMetadataClass;

struct _ChupaMetadata
{
    GObject object;
};

struct _ChupaMetadataClass
{
    GObjectClass parent_class;
};

GType        chupa_metadata_get_type(void) G_GNUC_CONST;
ChupaMetadata *chupa_metadata_new(void);
void chupa_metadata_add_value(ChupaMetadata *metadata, const gchar *key, const gchar *value);
const gchar *chupa_metadata_get_first_value(ChupaMetadata *metadata, const gchar *key);
GList *chupa_metadata_get_values(ChupaMetadata *metadata, const gchar *key);

G_END_DECLS

#endif  /* CHUPA_METADATA_H */
