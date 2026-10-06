#ifndef WOW_SOURCE_UI_DRESSUPMODELFRAME_INL
#define WOW_SOURCE_UI_DRESSUPMODELFRAME_INL

// INLINED
inline int32_t CGDressUpModelFrame::GetObjectType() {
    if (!CGDressUpModelFrame::s_objectType) {
        CGDressUpModelFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGDressUpModelFrame::s_objectType;
}

#endif
