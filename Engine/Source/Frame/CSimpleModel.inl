#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMODEL_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEMODEL_INL

// INLINED
inline int32_t CSimpleModel::GetObjectType() {
    if (!CSimpleModel::s_objectType) {
        CSimpleModel::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleModel::s_objectType;
}

#endif
