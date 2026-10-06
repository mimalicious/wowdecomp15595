#ifndef WOW_SOURCE_UI_MINIMAPFRAME_INL
#define WOW_SOURCE_UI_MINIMAPFRAME_INL

// INLINED
inline int32_t CGMinimapFrame::GetObjectType() {
    if (!CGMinimapFrame::s_objectType) {
        CGMinimapFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGMinimapFrame::s_objectType;
}

#endif
