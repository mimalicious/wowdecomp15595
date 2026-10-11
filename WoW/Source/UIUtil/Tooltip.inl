#ifndef WOW_SOURCE_UIUTIL_TOOLTIP_INL
#define WOW_SOURCE_UIUTIL_TOOLTIP_INL

// INLINED
inline int32_t CGTooltip::GetObjectType() {
    if (!CGTooltip::s_objectType) {
        CGTooltip::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGTooltip::s_objectType;
}

#endif
