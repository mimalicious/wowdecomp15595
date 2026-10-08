#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include <cmath>
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00DD394C
int32_t s_numRegions;

// STATIC: %0x00CD12C0
CDataAllocator CGradient::s_allocator(sizeof(CGradient), 5);

// OFFSET: 0x0050AA10
CGradient::CGradient() {
    this->m_alphaCount = 4;
    this->m_colorCount = 4;
    memset(this->m_colors, 0, this->m_colorCount * sizeof(CImVector));
    memset(this->m_alphas, 0, this->m_alphaCount);
}

// OFFSET: 0x0050A790
void CGradient::Set(ORIENTATION orientation, const CImVector& minColor, const CImVector& maxColor) {
    if (orientation) {
        this->m_alphas[1] = minColor.a;
        this->m_colors[1] = minColor;
        this->m_alphas[3] = minColor.a;
        this->m_colors[3] = minColor;
        this->m_alphas[0] = maxColor.a;
        this->m_colors[0] = maxColor;
        this->m_alphas[2] = maxColor.a;
        this->m_colors[2] = maxColor;
    } else {
        this->m_alphas[0] = minColor.a;
        this->m_colors[0] = minColor;
        this->m_alphas[1] = minColor.a;
        this->m_colors[1] = minColor;
        this->m_alphas[2] = maxColor.a;
        this->m_colors[2] = maxColor;
        this->m_alphas[3] = maxColor.a;
        this->m_colors[3] = maxColor;
    }
}

// PROVEN: 309df4a1 IDENTICAL/IDENTICAL
// OFFSET: 0x0050A810/0x1401529C0
void CGradient::ApplyAlpha(uint8_t alpha) {
    for (uint32_t i = this->m_colorCount; i; ) {
        i--;
        this->m_colors[i].a = alpha * this->m_alphas[i] / 255;
    }
}

// PROVEN: ec65a0ae IDENTICAL/IDENTICAL
// OFFSET: 0x0050A860/0x140152A10
bool CGradient::IsWhite() {
    for (uint32_t i = this->m_colorCount; i; ) {
        i--;

        if (this->m_alphas[i] < 0xFE || this->m_colors[i].r != 0xFF || this->m_colors[i].g != 0xFF || this->m_colors[i].b != 0xFF) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x004F87A0
void CSimpleRegion::SetVertexAlpha(uint8_t alpha) {
    CImVector color;
    this->GetVertexColor(color);
    color.a = alpha;
    this->SetVertexColor(color);
}

// OFFSET: 0x004F87D0
uint8_t CSimpleRegion::GetVertexAlpha() {
    CImVector color;
    this->GetVertexColor(color);
    return color.a;
}

// PROVEN: 74d0fb14 IDENTICAL/NONE
// OFFSET: 0x004F87F0
void CSimpleRegion::SetIgnoreParentAlpha(bool ignore) {
    if (ignore != this->m_ignoreParentAlpha) {
        this->m_ignoreParentAlpha = ignore;
        this->OnColorChanged(false);
    }
}

// PROVEN: 9747124f IDENTICAL/DIFFERENT
// OFFSET: 0x004FAA60/0x14013E7D0
void CSimpleRegion::SetParent(CSimpleFrame* parent) {
    this->SetFrame(parent, this->m_drawLayer, this->m_subLevel, this->m_shown);
}

// PROVEN: a1cb9dc6 IDENTICAL/UNPROVEN
// OFFSET: 0x004FBDD0/0x14013FFE0
void CSimpleRegion::DumpInfo(int32_t depth, void (*print)(char*, void*), void* param) {
    char buffer[128];

    if (depth) {
        memset(buffer, '.', depth);
    }

    SStrPrintf(&buffer[depth], sizeof(buffer) - depth, "Region %p\n", this);
    print(buffer, param);
}

// PROVEN: 3265573d IDENTICAL/IDENTICAL
// OFFSET: 0x0050A450/0x1401525D0
int32_t CSimpleRegion::CanBeAnchorFor(CLayoutFrame* frame) {
    return frame->IsProtected() == 0;
}

// PROVEN: b9f67c7d IDENTICAL/DIFFERENT
// OFFSET: 0x004FA850/0x1401A1AA0
void CSimpleRegion::GetBaseColor(CImVector& color) {
    const CImVector* source = this->m_hasGradient ? this->Gradient()->m_colors : (this->m_hasColor ? &this->m_color : NULL);

    if (source) {
        color = *source;
    }
}

// PROVEN: 3c7c8b85 IDENTICAL/DIFFERENT
// OFFSET: 0x0050A470/0x1401525F0
void CSimpleRegion::GetVertexColor(CImVector& color) {
    if (this->m_hasColor) {
        color = this->m_color;
        color.a = this->m_vertexAlpha;
    } else {
        color = CImVector(0xFFFFFFFF);
    }
}

// OFFSET: 0x0050A4B0/0x140152620
void CSimpleRegion::MarkDirty() {
    if (!this->m_visible) {
        return;
    }

    this->m_parent->RemoveFrameRegion(this, this->m_drawLayer);
    this->m_parent->AddFrameRegion(this, this->m_drawLayer);
    this->m_visible = 1;
}

// PROVEN: 75b95c58 IDENTICAL/NONE
// OFFSET: 0x0050A500
void CSimpleRegion::SignalLoaded() {
    this->m_loadPending = 0;
}

// OFFSET: 0x0050A510/0x140152690
void CSimpleRegion::HideThis() {
    if (!this->m_visible || !this->m_parent) {
        return;
    }

    if (!this->m_parent->m_isLoading) {
        this->SetDeferredResize(1);
    }

    this->m_parent->RemoveFrameRegion(this, this->m_drawLayer);
    this->m_visible = 0;
}

// OFFSET: 0x0050A560/0x140152700
void CSimpleRegion::ShowThis(bool loadPending) {
    if (!this->m_shown || !this->m_parent || !this->m_parent->m_visible || this->m_visible) {
        return;
    }

    if (!this->m_parent->m_isLoading) {
        this->SetDeferredResize(0);
    }

    this->m_loadPending = loadPending;
    this->m_parent->AddFrameRegion(this, this->m_drawLayer);
    this->m_visible = 1;
}

// OFFSET: 0x0050A5E0/0x1401527A0
void CSimpleRegion::SetFrame(CSimpleFrame* frame, uint32_t drawLayer, int8_t subLevel, bool show) {
    if (this->m_parent == frame) {
        if (this->m_drawLayer == drawLayer && this->m_subLevel == subLevel) {
            if (show != (this->m_shown != 0)) {
                if (show) {
                    this->Show();
                } else {
                    this->Hide();
                }
            }

            return;
        }

        if (this->m_shown) {
            this->Hide();
        }

        this->m_subLevel = subLevel;
        this->m_drawLayer = drawLayer;

        if (show) {
            this->Show();
        }

        return;
    }

    if (this->m_parent) {
        this->HideThis();
        this->m_parent->UnregisterRegion(this);
    }

    this->m_parent = frame;
    this->m_subLevel = subLevel;
    this->m_drawLayer = drawLayer;

    if (!frame) {
        return;
    }

    frame->RegisterRegion(this);
    this->SetDeferredResize(this->m_parent->m_layoutFlags & 2);
    this->OnColorChanged(false);

    if (show) {
        this->Show();
    } else {
        this->Hide();
    }
}

// OFFSET: 0x0050A740/0x140152950
void CSimpleRegion::OnRegionChanged() {
    if (this->m_visible && this->m_parent) {
        this->m_parent->NotifyDrawLayerChanged(this->m_drawLayer);
    }
}

// OFFSET: 0x0050A770/0x140152990
void CSimpleRegion::NotifyDelayedLoad() {
    if (this->m_visible && this->m_parent) {
        this->m_parent->NotifyDelayedLoad();
    }
}

// OFFSET: 0x0050A8A0
bool CSimpleRegion::IsColorWhite() {
    return this->m_vertexAlpha >= 0xFE && this->m_color.r == 0xFF && this->m_color.g == 0xFF && this->m_color.b == 0xFF;
}

// OFFSET: 0x0050A8D0/0x140152A90
void CSimpleRegion::SetVertexColor(const CImVector& color) {
    bool changed = this->m_hasGradient || !this->m_hasColor;

    if (!this->m_hasColor && !this->m_hasGradient && color.a >= 0xFE && color.r == 0xFF && color.g == 0xFF && color.b == 0xFF) {
        return;
    }

    if (this->m_hasColor) {
        if (this->m_vertexAlpha == color.a && this->m_color.r == color.r && this->m_color.g == color.g && this->m_color.b == color.b) {
            return;
        }
    } else {
        this->EnableGradient(false);

        if (!this->m_hasColor) {
            this->m_vertexAlpha = 0xFF;
            this->m_color.b = 0xFF;
            this->m_color.g = 0xFF;
            this->m_color.r = 0xFF;
            this->m_color.a = 0xFF;
        }

        this->m_hasColor = 1;
    }

    this->m_vertexAlpha = color.a;
    this->m_color = color;
    this->OnColorChanged(changed);
}

// PROVEN: 64591d67 IDENTICAL/DIFFERENT
// OFFSET: 0x0050A9A0/0x140152B80
void CSimpleRegion::AddAnimAlpha(CScriptRegion* region, int16_t alpha) {
    if (region && region != this) {
        return;
    }

    CImVector color;
    this->GetVertexColor(color);

    int16_t value = alpha + color.a;

    if (value > 255) {
        color.a = 255;
    } else {
        color.a = value < 0 ? 0 : value;
    }

    this->SetVertexColor(color);
}

// OFFSET: 0x0050AA50/0x140152BF0
void CSimpleRegion::EnableGradient(bool enable) {
    if (enable) {
        if (!this->m_hasGradient) {
            this->m_hasColor = 0;
            void* mem = CGradient::s_allocator.GetData(0, typeid(CGradient).raw_name(), SLOG_OBJECT);
            this->Gradient() = mem ? new (mem) CGradient() : NULL;
            this->m_hasGradient = 1;
        }
    } else if (this->m_hasGradient) {
        if (this->Gradient()) {
            CGradient::s_allocator.PutData(this->Gradient(), NULL, 0);
        }

        this->m_hasGradient = 0;
        this->m_vertexAlpha = 0xFF;
        this->m_color = CImVector(0xFFFFFFFF);
    }
}

// OFFSET: 0x0050AB00
void CSimpleRegion::SetVertexGradient(ORIENTATION orientation, const CImVector& minColor, const CImVector& maxColor) {
    bool changed = !this->m_hasGradient;

    if (!this->m_hasGradient) {
        this->m_hasColor = 0;
        void* mem = CGradient::s_allocator.GetData(0, typeid(CGradient).raw_name(), SLOG_OBJECT);
        this->Gradient() = mem ? new (mem) CGradient() : NULL;
        this->m_hasGradient = 1;
    }

    this->Gradient()->Set(orientation, minColor, maxColor);
    this->OnColorChanged(changed);
}

// OFFSET: 0x0050AB90
void CSimpleRegion::OnColorChanged(bool changed) {
    if (this->m_parent) {
        if (!this->m_hasColor && !this->m_hasGradient) {
            this->m_hasColor = 1;
            this->m_vertexAlpha = 0xFF;
            this->m_color.b = 0xFF;
            this->m_color.g = 0xFF;
            this->m_color.r = 0xFF;
            this->m_color.a = 0xFF;
            changed = true;
        } else if (this->m_parent->GetEffectiveAlpha() >= 0xFE) {
            if (this->m_hasColor) {
                if (this->IsColorWhite()) {
                    this->m_hasColor = 0;
                    changed = true;
                }
            } else if (this->m_hasGradient && this->Gradient()->IsWhite()) {
                this->EnableGradient(false);
                changed = true;
            }
        }

        if (this->m_hasColor) {
            uint8_t alpha = this->m_ignoreParentAlpha ? 0xFF : this->m_parent->GetEffectiveAlpha();
            this->m_color.a = alpha * this->m_vertexAlpha / 255;
        } else if (this->m_hasGradient) {
            this->Gradient()->ApplyAlpha(this->m_ignoreParentAlpha ? 0xFF : this->m_parent->GetEffectiveAlpha());
        }
    }

    if (changed) {
        this->OnRegionChanged();
    }
}

// PROVEN: a62151d3 IDENTICAL/NONE
// OFFSET: 0x0050AD20
void CSimpleRegion::AnimActivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position) {
    if (!this->m_playingAnimGroups || !this->m_playingAnimGroups->Head()) {
        this->AnimData_Init();
    }

    if (alpha) {
        this->AnimData_StoreAlpha(this->GetVertexAlpha());
    }

    if (position) {
        this->StoreAnimPosition();
    }
}

// PROVEN: 153a2cba IDENTICAL/NONE
// OFFSET: 0x0050AD90
void CSimpleRegion::AnimDeactivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position) {
    if (alpha) {
        uint8_t value = this->AnimData_GetAlpha();
        CImVector color;
        this->GetVertexColor(color);
        color.a = value;
        this->SetVertexColor(color);
        this->AnimData_ResetAlphaData();
    }

    if (position) {
        this->AnimData_ResetPositionData();
    }
}

// PROVEN: f4343a83 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CSimpleRegion::StoreAnimPosition() {
}

// OFFSET: 0x0050ADF0/0x1401530F0
CSimpleRegion::CSimpleRegion(CSimpleFrame* frame, uint32_t drawLayer, int8_t subLevel, bool show) : m_color(0xFFFFFFFF) {
    this->m_drawLayer = 0;
    this->m_hasGradient = 0;
    this->m_hasColor = 0;
    this->m_ignoreParentAlpha = 0;
    this->m_vertexAlpha = 0;
    s_numRegions++;

    if (frame) {
        this->SetFrame(frame, drawLayer, subLevel, show);
    }
}

// OFFSET: [%0x0051AF10, %0x0051AF00, %0x0051AFF0]
CSimpleRegion::~CSimpleRegion() {
    this->SetFrame(NULL, 1, 0, false);
    this->EnableGradient(false);
    s_numRegions--;
}

// PROVEN: a61fca48 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CSimpleRegion::OnScreenSizeChanged() {
}

// OFFSET: 0x009E80F0
void CSimpleRegion::SetVertexColorRGBA(float r, float g, float b, float a) {
    CImVector color;
    color.Set(a, r, g, b);
    this->SetVertexColor(color);
}

// PROVEN: 862fa581 IDENTICAL/DIFFERENT
// OFFSET: 0x0052B890/0x14017C080
C3Vector& ApplyTranslationAnim(C3Vector& position, const C2Vector& translation) {
    position.x = position.x + translation.x;
    position.y = translation.y + position.y;

    return position;
}

// OFFSET: 0x0052B810
void ApplyTranslationAnim(C3Vector* positions, const C2Vector& translation) {
    for (uint32_t i = 0; i < 4; i++) {
        positions[i].x = positions[i].x + translation.x;
        positions[i].y = translation.y + positions[i].y;
    }
}

// INLINED
static C2Vector LerpPoint(float t, const C2Vector& a, const C2Vector& b) {
    C2Vector result;
    result.x = (b.x - a.x) * t + a.x;
    result.y = (b.y - a.y) * t + a.y;

    return result;
}

// OFFSET: 0x0052B8C0
static void GetOriginOffset(FRAMEPOINT origin, const C3Vector* positions, const C2Vector& offset, C2Vector& result) {
    switch (origin) {
        case FRAMEPOINT_TOPLEFT:
            result.x = positions[0].x;
            result.y = positions[0].y;
            break;

        case FRAMEPOINT_TOP:
            result = LerpPoint(0.5f, C2Vector(positions[0]), C2Vector(positions[2]));
            break;

        case FRAMEPOINT_TOPRIGHT:
            result.x = positions[2].x;
            result.y = positions[2].y;
            break;

        case FRAMEPOINT_LEFT:
            result = LerpPoint(0.5f, C2Vector(positions[0]), C2Vector(positions[1]));
            break;

        case FRAMEPOINT_CENTER:
            result = LerpPoint(0.5f, C2Vector(positions[0]), C2Vector(positions[3]));
            break;

        case FRAMEPOINT_RIGHT:
            result = LerpPoint(0.5f, C2Vector(positions[2]), C2Vector(positions[3]));
            break;

        case FRAMEPOINT_BOTTOMLEFT:
            result.x = positions[1].x;
            result.y = positions[1].y;
            break;

        case FRAMEPOINT_BOTTOM:
            result = LerpPoint(0.5f, C2Vector(positions[1]), C2Vector(positions[3]));
            break;

        case FRAMEPOINT_BOTTOMRIGHT:
            result.x = positions[3].x;
            result.y = positions[3].y;
            break;

        default:
            result.x = 0.0f;
            result.y = 0.0f;
            break;
    }

    if (offset.x * offset.x + offset.y * offset.y > 0.00000011920929f) {
        C3Vector edge;
        edge.x = positions[3].x - positions[1].x;
        edge.y = positions[3].y - positions[1].y;
        edge.z = positions[3].z - positions[1].z;
        C2Vector direction(edge);
        float length = direction.y * direction.y + direction.x * direction.x;

        if (length > 0.00000023841858f) {
            float scale = 1.0f / sqrt(length);
            direction.x = direction.x * scale;
            direction.y = direction.y * scale;
        }

        result.x = offset.x * direction.x - offset.y * direction.y + result.x;
        result.y = offset.y * direction.x + offset.x * direction.y + result.y;
    }
}

// OFFSET: 0x0052BB90
void ApplyRotationAnim(C3Vector* positions, FRAMEPOINT origin, const C2Vector& offset, float radians) {
    C2Vector center;
    GetOriginOffset(origin, positions, offset, center);

    float c = cos(radians);
    float s = sin(radians);

    for (uint32_t i = 0; i < 4; i++) {
        float x = positions[i].x - center.x;
        float y = positions[i].y - center.y;
        positions[i].x = x * c - y * s + center.x;
        positions[i].y = x * s + y * c + center.y;
    }
}

// PROVEN: 148452e7 IDENTICAL/NONE
// OFFSET: 0x0052BD00
void ApplyScaleAnim(C3Vector* positions, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale) {
    C2Vector center;
    GetOriginOffset(origin, positions, offset, center);

    for (uint32_t i = 0; i < 4; i++) {
        positions[i].x = (center.x - positions[i].x) * scale.x + positions[i].x;
        positions[i].y = (center.y - positions[i].y) * scale.y + positions[i].y;
    }
}

// OFFSET: 0x004F7610/0x14013A7D0
void CSimpleRegion::PreLoadXML(linkObject* node, CStatus* status) {
    if (node->m_flags & 0x1000000) {
        this->SetName(node->m_name.m_string);
    }
}
