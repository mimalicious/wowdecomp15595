#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CBackdropGenerator.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CRect.h"

// PROVEN: 4955e577 IDENTICAL
// OFFSET: 0x005106C0
void CBackdropGenerator::SetVertexColor(const CImVector& color) {
    this->m_backgroundColor.a = color.a;
    this->m_backgroundColor.r = color.r;
    this->m_backgroundColor.g = color.g;
    this->m_backgroundColor.b = color.b;

    if (this->m_backgroundTexture) {
        this->m_backgroundTexture->SetVertexColor(color);
    }
}

// PROVEN: 20a35a4b IDENTICAL
// OFFSET: 0x00510700
void CBackdropGenerator::SetBorderVertexColor(const CImVector& color) {
    this->m_borderColor.a = color.a;
    this->m_borderColor.r = color.r;
    this->m_borderColor.g = color.g;
    this->m_borderColor.b = color.b;

    if (this->m_leftTexture) {
        this->m_leftTexture->SetVertexColor(color);
    }

    if (this->m_rightTexture) {
        this->m_rightTexture->SetVertexColor(color);
    }

    if (this->m_topTexture) {
        this->m_topTexture->SetVertexColor(color);
    }

    if (this->m_bottomTexture) {
        this->m_bottomTexture->SetVertexColor(color);
    }

    if (this->m_topLeftTexture) {
        this->m_topLeftTexture->SetVertexColor(color);
    }

    if (this->m_topRightTexture) {
        this->m_topRightTexture->SetVertexColor(color);
    }

    if (this->m_bottomLeftTexture) {
        this->m_bottomLeftTexture->SetVertexColor(color);
    }

    if (this->m_bottomRightTexture) {
        this->m_bottomRightTexture->SetVertexColor(color);
    }
}

// PROVEN: 4d3ece45 IDENTICAL
// OFFSET: 0x005107A0
void CBackdropGenerator::DestroyTextures() {
    if (this->m_backgroundTexture) {
        delete this->m_backgroundTexture;
        this->m_backgroundTexture = NULL;
    }

    if (this->m_leftTexture) {
        delete this->m_leftTexture;
        this->m_leftTexture = NULL;
    }

    if (this->m_rightTexture) {
        delete this->m_rightTexture;
        this->m_rightTexture = NULL;
    }

    if (this->m_topTexture) {
        delete this->m_topTexture;
        this->m_topTexture = NULL;
    }

    if (this->m_bottomTexture) {
        delete this->m_bottomTexture;
        this->m_bottomTexture = NULL;
    }

    if (this->m_topLeftTexture) {
        delete this->m_topLeftTexture;
        this->m_topLeftTexture = NULL;
    }

    if (this->m_topRightTexture) {
        delete this->m_topRightTexture;
        this->m_topRightTexture = NULL;
    }

    if (this->m_bottomLeftTexture) {
        delete this->m_bottomLeftTexture;
        this->m_bottomLeftTexture = NULL;
    }

    if (this->m_bottomRightTexture) {
        delete this->m_bottomRightTexture;
        this->m_bottomRightTexture = NULL;
    }
}

// OFFSET: %0x00520CA0
void CBackdropGenerator::LoadXML(linkObject* node, CStatus* status) {
    Backdrop* backdrop = static_cast<Backdrop*>(node);
    uint32_t pieces = (backdrop->m_backdropFlags & 4) && *backdrop->m_edgeFile.m_string ? 0xFF : 0;
    EGxBlend alphaMode = static_cast<EGxBlend>(backdrop->m_alphaMode);
    int32_t tile = backdrop->m_backdropFlags & 1;

    this->m_bgFile.Copy(backdrop->m_bgFile.m_string);
    this->m_pieces = pieces;
    this->m_tile = tile;
    this->m_edgeFile.Copy(backdrop->m_edgeFile.m_string);
    this->m_alphaMode = alphaMode;

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_BORDERCOLOR: {
                CImVector color(0, 0, 0, 0);

                if (static_cast<Color*>(child)->GetColor(color, status)) {
                    this->SetBorderVertexColor(color);
                }

                break;
            }

            case XML_COLOR: {
                CImVector color(0, 0, 0, 0);

                if (static_cast<Color*>(child)->GetColor(color, status)) {
                    this->SetVertexColor(color);
                }

                break;
            }

            case XML_EDGESIZE: {
                float value;

                if (static_cast<Value*>(child)->Load(value, status)) {
                    this->m_edgeSize = value;
                }

                break;
            }

            case XML_TILESIZE: {
                float value;

                if (static_cast<Value*>(child)->Load(value, status)) {
                    this->m_tileSize = value;
                }

                break;
            }

            case XML_BACKGROUNDINSETS: {
                float left;
                float right;
                float top;
                float bottom;

                if (static_cast<Insets*>(child)->Load(left, right, top, bottom, status)) {
                    this->m_insetLeft = left;
                    this->m_insetRight = right;
                    this->m_insetTop = top;
                    this->m_insetBottom = bottom;
                }

                break;
            }

            default:
                status->Add(STATUS_WARNING, "Unknown child node in %s element: %s", node->m_type.m_string, child->m_type.m_string);
                break;
        }
    }
}

// OFFSET: %0x00520EB0
void CBackdropGenerator::Generate(const CRect* rect) {
    if (!rect) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    C2Vector texCoords[4];

    float width = rect->maxX - rect->minX;
    float scale = 1.0f / this->m_edgeSize;
    float tilesX = scale * width - 2.0f;
    float height = rect->maxY - rect->minY;
    float tilesY = scale * height - 2.0f;

    if (tilesX < 0.0f) {
        tilesX = 0.0f;
    }

    if (tilesY < 0.0f) {
        tilesY = 0.0f;
    }

    if (this->m_bgFile.m_str && *this->m_bgFile.m_str && this->m_tile) {
        float u = width / (this->m_tileSize != 0.0f ? this->m_tileSize : this->m_edgeSize);
        float v = height / (this->m_tileSize != 0.0f ? this->m_tileSize : this->m_edgeSize);

        texCoords[0].x = 0.0f;
        texCoords[0].y = 0.0f;
        texCoords[1].x = 0.0f;
        texCoords[1].y = v;
        texCoords[2].x = u;
        texCoords[2].y = 0.0f;
        texCoords[3].x = u;
        texCoords[3].y = v;
        this->m_backgroundTexture->SetTexCoord(texCoords);
    }

    if (this->m_pieces & 0x1) {
        texCoords[0].x = 0.0078125f;
        texCoords[0].y = 0.0625f;
        texCoords[1].x = 0.0078125f;
        texCoords[1].y = tilesY - 0.0625f;
        texCoords[2].x = 0.1171875f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.1171875f;
        texCoords[3].y = tilesY - 0.0625f;
        this->m_leftTexture->SetTexCoord(texCoords);
    }

    if (this->m_pieces & 0x2) {
        texCoords[0].x = 0.1328125f;
        texCoords[0].y = 0.0625f;
        texCoords[1].x = 0.1328125f;
        texCoords[1].y = tilesY - 0.0625f;
        texCoords[2].x = 0.2421875f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.2421875f;
        texCoords[3].y = tilesY - 0.0625f;
        this->m_rightTexture->SetTexCoord(texCoords);
    }

    if (this->m_pieces & 0x4) {
        texCoords[0].x = 0.2578125f;
        texCoords[0].y = tilesX - 0.0625f;
        texCoords[1].x = 0.3671875f;
        texCoords[1].y = tilesX - 0.0625f;
        texCoords[2].x = 0.2578125f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.3671875f;
        texCoords[3].y = 0.0625f;
        this->m_topTexture->SetTexCoord(texCoords);
    }

    if (this->m_pieces & 0x8) {
        texCoords[0].x = 0.3828125f;
        texCoords[0].y = tilesX - 0.0625f;
        texCoords[1].x = 0.4921875f;
        texCoords[1].y = tilesX - 0.0625f;
        texCoords[2].x = 0.3828125f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.4921875f;
        texCoords[3].y = 0.0625f;
        this->m_bottomTexture->SetTexCoord(texCoords);
    }

    this->SetVertexColor(this->m_backgroundColor);
    this->SetBorderVertexColor(this->m_borderColor);
}

// OFFSET: %0x00520C00
CBackdropGenerator::CBackdropGenerator() {
    this->m_backgroundTexture = NULL;
    this->m_leftTexture = NULL;
    this->m_rightTexture = NULL;
    this->m_topTexture = NULL;
    this->m_bottomTexture = NULL;
    this->m_topLeftTexture = NULL;
    this->m_topRightTexture = NULL;
    this->m_bottomLeftTexture = NULL;
    this->m_bottomRightTexture = NULL;
    this->m_edgeSize = 0.025f;
    this->m_tileSize = 0.0f;
    this->m_pieces = 0;
    this->m_insetTop = 0.0f;
    this->m_tile = 0;
    this->m_insetBottom = 0.0f;
    this->m_insetLeft = 0.0f;
    this->m_insetRight = 0.0f;
    this->m_backgroundColor.b = 0xFF;
    this->m_backgroundColor.g = 0xFF;
    this->m_backgroundColor.r = 0xFF;
    this->m_backgroundColor.a = 0xFF;
    this->m_borderColor.b = 0xFF;
    this->m_borderColor.g = 0xFF;
    this->m_borderColor.r = 0xFF;
    this->m_borderColor.a = 0xFF;
}

// OFFSET: %0x00521290
void CBackdropGenerator::SetOutput(CSimpleFrame* frame) {
    if (!frame) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    C2Vector texCoords[4];

    if (this->m_bgFile.m_str && *this->m_bgFile.m_str) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 0, 0, true) : NULL;
        this->m_backgroundTexture = texture;

        texture->SetPoint(FRAMEPOINT_TOPLEFT, frame, FRAMEPOINT_TOPLEFT, this->m_insetLeft, -this->m_insetTop, 0);
        texture->SetPoint(FRAMEPOINT_TOPRIGHT, frame, FRAMEPOINT_TOPRIGHT, -this->m_insetRight, -this->m_insetTop, 0);
        texture->SetPoint(FRAMEPOINT_BOTTOMLEFT, frame, FRAMEPOINT_BOTTOMLEFT, this->m_insetLeft, this->m_insetBottom, 0);
        texture->SetPoint(FRAMEPOINT_BOTTOMRIGHT, frame, FRAMEPOINT_BOTTOMRIGHT, -this->m_insetRight, this->m_insetTop, 0);
        texture->Resize(0);
        texture->SetTexture(this->m_bgFile.m_str ? this->m_bgFile.m_str : "", this->m_tile, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    }

    if (this->m_pieces & 0x1) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_leftTexture = texture;

        texture->SetWidth(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_TOPLEFT, frame, FRAMEPOINT_TOPLEFT, 0.0f, -this->m_edgeSize, 0);
        texture->SetPoint(FRAMEPOINT_BOTTOMLEFT, frame, FRAMEPOINT_BOTTOMLEFT, 0.0f, this->m_edgeSize, 0);
        texture->Resize(0);
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 1, 1, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x2) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_rightTexture = texture;

        texture->SetWidth(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_TOPRIGHT, frame, FRAMEPOINT_TOPRIGHT, 0.0f, -this->m_edgeSize, 0);
        texture->SetPoint(FRAMEPOINT_BOTTOMRIGHT, frame, FRAMEPOINT_BOTTOMRIGHT, 0.0f, this->m_edgeSize, 0);
        texture->Resize(0);
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 1, 1, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x4) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_topTexture = texture;

        texture->SetHeight(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_TOPLEFT, frame, FRAMEPOINT_TOPLEFT, this->m_edgeSize, 0.0f, 0);
        texture->SetPoint(FRAMEPOINT_TOPRIGHT, frame, FRAMEPOINT_TOPRIGHT, -this->m_edgeSize, 0.0f, 0);
        texture->Resize(0);
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 1, 1, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x8) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_bottomTexture = texture;

        texture->SetHeight(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_BOTTOMLEFT, frame, FRAMEPOINT_BOTTOMLEFT, this->m_edgeSize, 0.0f, 0);
        texture->SetPoint(FRAMEPOINT_BOTTOMRIGHT, frame, FRAMEPOINT_BOTTOMRIGHT, -this->m_edgeSize, 0.0f, 0);
        texture->Resize(0);
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 1, 1, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x10) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_topLeftTexture = texture;

        texture->SetWidth(this->m_edgeSize);
        texture->SetHeight(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_TOPLEFT, frame, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f, 1);
        texCoords[0].x = 0.5078125f;
        texCoords[0].y = 0.0625f;
        texCoords[1].x = 0.5078125f;
        texCoords[1].y = 0.9375f;
        texCoords[2].x = 0.6171875f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.6171875f;
        texCoords[3].y = 0.9375f;
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetTexCoord(texCoords);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x20) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_topRightTexture = texture;

        texture->SetWidth(this->m_edgeSize);
        texture->SetHeight(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_TOPRIGHT, frame, FRAMEPOINT_TOPRIGHT, 0.0f, 0.0f, 1);
        texCoords[0].x = 0.6328125f;
        texCoords[0].y = 0.0625f;
        texCoords[1].x = 0.6328125f;
        texCoords[1].y = 0.9375f;
        texCoords[2].x = 0.7421875f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.7421875f;
        texCoords[3].y = 0.9375f;
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetTexCoord(texCoords);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x40) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_bottomLeftTexture = texture;

        texture->SetWidth(this->m_edgeSize);
        texture->SetHeight(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_BOTTOMLEFT, frame, FRAMEPOINT_BOTTOMLEFT, 0.0f, 0.0f, 1);
        texCoords[0].x = 0.7578125f;
        texCoords[0].y = 0.0625f;
        texCoords[1].x = 0.7578125f;
        texCoords[1].y = 0.9375f;
        texCoords[2].x = 0.8671875f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.8671875f;
        texCoords[3].y = 0.9375f;
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetTexCoord(texCoords);
        texture->SetBlendMode(this->m_alphaMode);
    }

    if (this->m_pieces & 0x80) {
        void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 1, 0, true) : NULL;
        this->m_bottomRightTexture = texture;

        texture->SetWidth(this->m_edgeSize);
        texture->SetHeight(this->m_edgeSize);
        texture->SetPoint(FRAMEPOINT_BOTTOMRIGHT, frame, FRAMEPOINT_BOTTOMRIGHT, 0.0f, 0.0f, 1);
        texCoords[0].x = 0.8828125f;
        texCoords[0].y = 0.0625f;
        texCoords[1].x = 0.8828125f;
        texCoords[1].y = 0.9375f;
        texCoords[2].x = 0.9921875f;
        texCoords[2].y = 0.0625f;
        texCoords[3].x = 0.9921875f;
        texCoords[3].y = 0.9375f;
        texture->SetTexture(this->m_edgeFile.m_str ? this->m_edgeFile.m_str : "", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetTexCoord(texCoords);
        texture->SetBlendMode(this->m_alphaMode);
    }
}
