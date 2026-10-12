#ifndef ENGINE_SOURCE_FRAME_CSIMPLEEMBEDDEDTEXTURE_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEEMBEDDEDTEXTURE_INL

// INLINED
inline CSimpleEmbeddedTexture::~CSimpleEmbeddedTexture() {
    delete this->m_texture;
}

#endif
