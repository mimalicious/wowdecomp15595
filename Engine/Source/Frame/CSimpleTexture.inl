#ifndef ENGINE_SOURCE_FRAME_CSIMPLETEXTURE_INL
#define ENGINE_SOURCE_FRAME_CSIMPLETEXTURE_INL

// INLINED
inline int32_t CSimpleTexture::GetObjectType() {
    if (!CSimpleTexture::s_objectType) {
        CSimpleTexture::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleTexture::s_objectType;
}

#endif
