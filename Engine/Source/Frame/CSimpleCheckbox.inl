#ifndef ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_INL
#define ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_INL

// OFFSET: 0x00532670
inline int32_t CSimpleCheckbox::GetObjectType() {
    if (!CSimpleCheckbox::s_objectType) {
        CSimpleCheckbox::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleCheckbox::s_objectType;
}

#endif
