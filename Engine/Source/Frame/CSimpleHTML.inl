#ifndef ENGINE_SOURCE_FRAME_CSIMPLEHTML_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEHTML_INL

// INLINED
inline int32_t CSimpleHTML::GetObjectType() {
    if (!CSimpleHTML::s_objectType) {
        CSimpleHTML::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleHTML::s_objectType;
}

#endif
