#ifndef __GST_RECORD_H__
#define __GST_RECORD_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_RECORD (gst_record_get_type())
G_DECLARE_FINAL_TYPE(GstRecord, gst_record, GST, RECORD, GstBaseTransform)

G_END_DECLS



#endif
