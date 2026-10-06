#ifndef ENGINE_SOURCE_FRAME_CSIMPLEANIM_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEANIM_INL

// INLINED
inline int32_t CSimpleAnim::GetObjectType() {
    if (!CSimpleAnim::s_objectType) {
        CSimpleAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleAnim::s_objectType;
}

#endif
