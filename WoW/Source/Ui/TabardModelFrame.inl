#ifndef WOW_SOURCE_UI_TABARDMODELFRAME_INL
#define WOW_SOURCE_UI_TABARDMODELFRAME_INL

// INLINED
inline int32_t CGTabardModelFrame::GetObjectType() {
    if (!CGTabardModelFrame::s_objectType) {
        CGTabardModelFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGTabardModelFrame::s_objectType;
}

#endif
