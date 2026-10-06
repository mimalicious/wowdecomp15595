#ifndef ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_INL
#define ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_INL

// INLINED
inline int32_t CSimpleCheckbox::GetObjectType() {
    if (!CSimpleCheckbox::s_objectType) {
        CSimpleCheckbox::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleCheckbox::s_objectType;
}

#endif
