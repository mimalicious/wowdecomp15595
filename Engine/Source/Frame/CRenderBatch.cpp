#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Services/TextBlock.h"

// STATIC: %0x00BB80F0
static uint16_t s_quadIndices[6] = { 0, 1, 2, 2, 1, 3 };

// OFFSET: 0x004F8620
static int32_t SortByTexture(const void* elem1, const void* elem2) {
    const CSimpleBatchedMesh* mesh1 = static_cast<const CSimpleBatchedMesh*>(elem1);
    const CSimpleBatchedMesh* mesh2 = static_cast<const CSimpleBatchedMesh*>(elem2);

    if (mesh1->m_subLevel != mesh2->m_subLevel) {
        return mesh1->m_subLevel - mesh2->m_subLevel;
    }

    if (mesh1->m_textureID != mesh2->m_textureID) {
        return static_cast<int32_t>(reinterpret_cast<uintptr_t>(mesh1->m_textureID) - reinterpret_cast<uintptr_t>(mesh2->m_textureID));
    }

    int32_t alphaMode1 = (mesh1->m_color && mesh1->m_alphaMode < GxBlend_Alpha) ? GxBlend_Alpha : mesh1->m_alphaMode;
    int32_t alphaMode2 = (mesh2->m_color && mesh2->m_alphaMode < GxBlend_Alpha) ? GxBlend_Alpha : mesh2->m_alphaMode;

    if (alphaMode1 != alphaMode2) {
        return alphaMode1 - alphaMode2;
    }

    return static_cast<int32_t>(reinterpret_cast<uintptr_t>(mesh1->m_shader) - reinterpret_cast<uintptr_t>(mesh2->m_shader));
}

// PROVEN: ceaa3c78 IDENTICAL/NONE
// OFFSET: 0x004FA6E0
void CRenderBatch::Finish() {
    if (this->m_texturelist.Count() > 1) {
        qsort(this->m_texturelist.Ptr(), this->m_texturelist.Count(), sizeof(CSimpleBatchedMesh), &SortByTexture);
    }
}

// OFFSET: 0x004FADF0/0x14013EC00
void CRenderBatch::Queue(CTexture* texture, EGxBlend alphaMode, int32_t posCount, const C3Vector* position, const C2Vector* texCoord, int32_t colorCount, const CImVector* color, int32_t idxCount, const uint16_t* indices, CGxShader* shader, int8_t subLevel) {
    CGxTex* gxTex = TextureGetGxTexBlocking(texture);

    if (!gxTex) {
        return;
    }

    uint32_t index = this->m_texturelist.Count();
    this->m_texturelist.SetCount(index + 1);

    CSimpleBatchedMesh* mesh = &this->m_texturelist[index];
    mesh->m_texture = texture;
    mesh->m_textureID = gxTex;
    mesh->m_subLevel = subLevel;
    mesh->m_alphaMode = alphaMode;
    mesh->m_shader = shader;
    mesh->m_posCount = posCount;
    mesh->m_position = position;
    mesh->m_texCoord = texCoord;
    mesh->m_color = color;
    mesh->m_colorCount = colorCount;
    mesh->m_indices = indices;
    mesh->m_idxCount = idxCount;
    mesh->m_onAtlas = TextureOnAtlas(texture);

    if (mesh->m_onAtlas) {
        TextureGetAtlasOffsetAndScale(mesh->m_texture, mesh->m_atlasOffset, mesh->m_atlasScale);
    }

    this->m_count++;
}

// OFFSET: 0x004FC020/0x140140200
CRenderBatch::CRenderBatch() {
    this->m_count = 0;
    this->m_stringbatch = NULL;
}

// OFFSET: 0x004FC580
void CRenderBatch::QueueCallback(void (*callback)(void*), void* param) {
    RENDERCALLBACKNODE* node = this->m_callbacks.NewNode(2, 0, 0);
    node->m_callback = callback;
    node->m_param = param;
    this->m_count++;
}

// OFFSET: 0x004FC730/0x140140B10
void CRenderBatch::Clear() {
    this->m_texturelist.SetCount(0);

    if (this->m_stringbatch) {
        GxuFontDestroyBatch(this->m_stringbatch);
        this->m_stringbatch = NULL;
    }

    this->m_callbacks.Clear();
    this->m_count = 0;
}

// OFFSET: [0x004FCB60/0x140140FD0, 0x004FCE60/0x140141500]
CRenderBatch::~CRenderBatch() {
    this->Clear();
}

// OFFSET: 0x004FA630/0x14013E3B0
void CRenderBatch::QueueFontString(CSimpleFontString* string) {
    CRect rect;

    if (!string->GetRect(&rect)) {
        return;
    }

    if (rect.maxY < 0.0f || rect.minY > 1.0f || rect.maxX < 0.0f || rect.minX > 1.0f) {
        return;
    }

    if (!string->m_string) {
        return;
    }

    CGxString* gxString = TextBlockGetStringPtr(string->m_string);

    if (!gxString) {
        return;
    }

    if (!this->m_stringbatch) {
        this->m_stringbatch = GxuFontCreateBatch(false, false, false);
    }

    GxuFontAddToBatch(this->m_stringbatch, gxString);
    this->m_count++;
}

// OFFSET: 0x004FAEA0/0x14013ECF0
void CRenderBatch::QueueTexture(CSimpleTexture* texture) {
    CGxTex* gxTex = TextureGetGxTex(texture->m_texture);

    if (!gxTex) {
        TextureIncreasePriority(texture->m_texture);

        if (texture->m_textureFlags & 1) {
            texture->NotifyDelayedLoad();
            return;
        }

        gxTex = TextureForceLoadForDiskOnly(texture->m_texture);

        if (!gxTex) {
            texture->NotifyDelayedLoad();
            return;
        }
    }

    if (texture->IsResizePending()) {
        texture->Resize(1);
    }

    uint32_t flags = texture->m_textureFlags;

    if (flags & 2) {
        C2Vector texCoords[4];
        texture->GetTexCoord(texCoords);

        float left = texture->m_positions[0].x;
        float right = texture->m_positions[2].x;
        float bottom = texture->m_positions[1].y;
        float top = texture->m_positions[0].y;

        if (flags & 4) {
            uint32_t width = gxTex->m_width;
            float ndcWidth = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (right - left)) / texture->m_layoutScale;

            if (width && ndcWidth > 0.0f) {
                if (texCoords[0].x != 0.0f) {
                    texCoords[0].x = ndcWidth / width;
                }

                if (texCoords[1].x != 0.0f) {
                    texCoords[1].x = ndcWidth / width;
                }

                if (texCoords[2].x != 0.0f) {
                    texCoords[2].x = ndcWidth / width;
                }

                if (texCoords[3].x != 0.0f) {
                    texCoords[3].x = ndcWidth / width;
                }
            }
        }

        if (texture->m_textureFlags & 8) {
            uint32_t height = gxTex->m_height;
            float ndcHeight = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (top - bottom)) / texture->m_layoutScale;

            if (height && ndcHeight > 0.0f) {
                if (texCoords[0].y != 0.0f) {
                    texCoords[0].y = ndcHeight / height;
                }

                if (texCoords[1].y != 0.0f) {
                    texCoords[1].y = ndcHeight / height;
                }

                if (texCoords[2].y != 0.0f) {
                    texCoords[2].y = ndcHeight / height;
                }

                if (texCoords[3].y != 0.0f) {
                    texCoords[3].y = ndcHeight / height;
                }
            }
        }

        texture->m_textureFlags &= ~2u;
        texture->m_texCoords[0] = texCoords[0];
        texture->m_texCoords[1] = texCoords[1];
        texture->m_texCoords[2] = texCoords[2];
        texture->m_texCoords[3] = texCoords[3];
    }

    const CImVector* colors;

    if (texture->m_hasGradient) {
        colors = texture->Gradient()->m_colors;
    } else if (texture->m_hasColor) {
        colors = &texture->m_color;
    } else {
        colors = NULL;
    }

    this->Queue(texture->m_texture, texture->m_alphaMode, 4, texture->m_positions, texture->m_texCoords, texture->m_hasGradient ? 4 : 0, colors, 6, s_quadIndices, texture->m_shader, texture->m_subLevel);
}
