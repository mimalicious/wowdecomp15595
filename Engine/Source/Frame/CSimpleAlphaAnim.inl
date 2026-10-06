#ifndef ENGINE_SOURCE_FRAME_CSIMPLEALPHAANIM_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEALPHAANIM_INL

// INLINED
inline int32_t CSimpleAlphaAnim::GetObjectType() {
    if (!CSimpleAlphaAnim::s_objectType) {
        CSimpleAlphaAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleAlphaAnim::s_objectType;
}

#endif
