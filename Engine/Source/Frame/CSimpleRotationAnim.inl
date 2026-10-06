#ifndef ENGINE_SOURCE_FRAME_CSIMPLEROTATIONANIM_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEROTATIONANIM_INL

// INLINED
inline int32_t CSimpleRotationAnim::GetObjectType() {
    if (!CSimpleRotationAnim::s_objectType) {
        CSimpleRotationAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleRotationAnim::s_objectType;
}

#endif
