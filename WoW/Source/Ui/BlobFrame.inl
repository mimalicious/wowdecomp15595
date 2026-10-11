#ifndef WOW_SOURCE_UI_BLOBFRAME_INL
#define WOW_SOURCE_UI_BLOBFRAME_INL

// INLINED
inline int32_t CGBlobFrame::GetObjectType() {
    if (!CGBlobFrame::s_objectType) {
        CGBlobFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGBlobFrame::s_objectType;
}

#endif
