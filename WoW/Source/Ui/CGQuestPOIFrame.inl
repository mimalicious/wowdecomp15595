#ifndef WOW_SOURCE_UI_CGQUESTPOIFRAME_INL
#define WOW_SOURCE_UI_CGQUESTPOIFRAME_INL

// INLINED
inline int32_t CGQuestPOIFrame::GetObjectType() {
    if (!CGQuestPOIFrame::s_objectType) {
        CGQuestPOIFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGQuestPOIFrame::s_objectType;
}

#endif
