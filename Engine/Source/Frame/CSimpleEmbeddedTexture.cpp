#include "Engine/Source/Frame/CSimpleEmbeddedTexture.h"
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleTexture.h"

// PROVEN: 5cb53368 IDENTICAL/IDENTICAL
// OFFSET: 0x004FB240/0x14013F150
CSimpleEmbeddedTexture::CSimpleEmbeddedTexture() {
    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    this->m_texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;
    this->m_texture->m_textureFlags |= 0x1000;
}
