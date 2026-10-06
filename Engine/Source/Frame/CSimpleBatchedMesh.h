#ifndef ENGINE_SOURCE_FRAME_CSIMPLEBATCHEDMESH_H
#define ENGINE_SOURCE_FRAME_CSIMPLEBATCHEDMESH_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"

class C3Vector;
class CGxShader;
class CGxTex;
class CImVector;
class CTexture;

class CSimpleBatchedMesh {
    public:
    /* 0000 */ CTexture* m_texture;
    /* 0004 */ CGxTex* m_textureID;
    /* 0008 */ int8_t m_subLevel;
    /* 000C */ EGxBlend m_alphaMode;
    /* 0010 */ CGxShader* m_shader;
    /* 0014 */ uint32_t m_posCount;
    /* 0018 */ const C3Vector* m_position;
    /* 001C */ const C2Vector* m_texCoord;
    /* 0020 */ const CImVector* m_color;
    /* 0024 */ uint32_t m_colorCount;
    /* 0028 */ const uint16_t* m_indices;
    /* 002C */ uint32_t m_idxCount;
    /* 0030 */ int32_t m_onAtlas;
    /* 0034 */ float m_atlasScale;
    /* 0038 */ C2Vector m_atlasOffset;
};

#endif
