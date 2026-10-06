#ifndef ENGINE_SOURCE_FRAME_CSIMPLEEDITBOX_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEEDITBOX_INL

// INLINED
inline int32_t CSimpleEditBox::GetObjectType() {
    if (!CSimpleEditBox::s_objectType) {
        CSimpleEditBox::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleEditBox::s_objectType;
}

#endif
