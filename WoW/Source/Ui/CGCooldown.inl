#ifndef WOW_SOURCE_UI_CGCOOLDOWN_INL
#define WOW_SOURCE_UI_CGCOOLDOWN_INL

// OFFSET: 0x00954900
inline int32_t CGCooldown::GetObjectType() {
    if (!CGCooldown::s_objectType) {
        CGCooldown::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGCooldown::s_objectType;
}

#endif
