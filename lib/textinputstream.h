#ifndef CHUPATEXT_TEXTINPUTSTREAM_H
#define CHUPATEXT_TEXTINPUTSTREAM_H

#include <glib.h>
#include <gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define CHUPA_TYPE_TEXT_INPUT_STREAM            chupa_text_input_stream_get_type()
#define CHUPA_TEXT_INPUT_STREAM(obj)            \
  G_TYPE_CHECK_INSTANCE_CAST(obj, CHUPA_TYPE_TEXT_INPUT_STREAM, ChupaTextInputStream)
#define CHUPA_TEXT_INPUT_STREAM_CLASS(klass)    \
  G_TYPE_CHECK_CLASS_CAST(klass, CHUPA_TYPE_TEXT_INPUT_STREAM, ChupaTextInputStreamClass)
#define CHUPA_IS_TEXT_INPUT_STREAM(obj)         \
  G_TYPE_CHECK_INSTANCE_TYPE(obj, CHUPA_TYPE_TEXT_INPUT_STREAM)
#define CHUPA_IS_TEXT_INPUT_STREAM_CLASS(klass) \
  G_TYPE_CHECK_CLASS_TYPE(klass, CHUPA_TYPE_TEXT_INPUT_STREAM)
#define CHUPA_TEXT_INPUT_STREAM_GET_CLASS(obj)  \
  G_TYPE_INSTANCE_GET_CLASS(obj, CHUPA_TYPE_TEXT_INPUT_STREAM, ChupaTextInputStreamClass)

typedef struct _ChupaTextInputStream ChupaTextInputStream;
typedef struct _ChupaTextInputStreamClass ChupaTextInputStreamClass;

struct _ChupaTextInputStream
{
    GInputStream object;
};

struct _ChupaTextInputStreamClass
{
    GInputStreamClass parent_class;
};

GType        chupa_text_input_stream_get_type(void) G_GNUC_CONST;
ChupaTextInputStream *chupa_text_input_stream_new(GHashTable *metadata, GInputStream *input);
GHashTable *chupa_text_input_stream_get_metadata(ChupaTextInputStream *stream);

G_END_DECLS

#endif  /* CHUPATEXT_TEXTINPUTSTREAM_H */
