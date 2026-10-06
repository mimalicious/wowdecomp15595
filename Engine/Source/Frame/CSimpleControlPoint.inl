#ifndef ENGINE_SOURCE_FRAME_CSIMPLECONTROLPOINT_INL
#define ENGINE_SOURCE_FRAME_CSIMPLECONTROLPOINT_INL

// INLINED
inline int32_t CSimpleControlPoint::GetObjectType() {
    if (!CSimpleControlPoint::s_objectType) {
        CSimpleControlPoint::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleControlPoint::s_objectType;
}

#endif
