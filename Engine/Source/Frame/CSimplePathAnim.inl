#ifndef ENGINE_SOURCE_FRAME_CSIMPLEPATHANIM_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEPATHANIM_INL

// OFFSET: 0x0053BB20
inline int32_t CSimplePathAnim::GetObjectType() {
    if (!CSimplePathAnim::s_objectType) {
        CSimplePathAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimplePathAnim::s_objectType;
}

#endif
