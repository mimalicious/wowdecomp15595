#ifndef ENGINE_SOURCE_FRAME_CSIMPLEANIMGROUP_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEANIMGROUP_INL

// OFFSET: %0x0054B850
inline int32_t CSimpleAnimGroup::GetObjectType() {
    if (!CSimpleAnimGroup::s_objectType) {
        CSimpleAnimGroup::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleAnimGroup::s_objectType;
}

#endif
