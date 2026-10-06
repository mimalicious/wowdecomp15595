#ifndef ENGINE_SOURCE_FRAME_CSIMPLESCALEANIM_INL
#define ENGINE_SOURCE_FRAME_CSIMPLESCALEANIM_INL

// INLINED
inline int32_t CSimpleScaleAnim::GetObjectType() {
    if (!CSimpleScaleAnim::s_objectType) {
        CSimpleScaleAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleScaleAnim::s_objectType;
}

#endif
