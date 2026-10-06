#include "Engine/Source/Frame/CSimpleEmbeddedTexture.h"
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleTexture.h"

// OFFSET: %0x0050B470
CSimpleEmbeddedTexture::CSimpleEmbeddedTexture() {
    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    this->m_texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;
    this->m_texture->m_textureFlags |= 0x1000;
}

// INLINED
CSimpleEmbeddedTexture::~CSimpleEmbeddedTexture() {
    delete this->m_texture;
}
