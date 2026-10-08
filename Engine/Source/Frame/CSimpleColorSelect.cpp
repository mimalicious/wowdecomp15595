#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleColorSelect.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/TabardModelFrame.h"

// STATIC: %0x00BB9920
int32_t CSimpleColorSelect::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3ABC/0x140CB9858
int32_t CSimpleColorSelect::s_objectType;

// STATIC: %0x00BB8818
const char* CSimpleColorSelect::s_objectTypeName = "ColorSelect";

// STATIC: 0x00DD3AC0
CDataAllocator CSimpleColorSelect::s_allocator(sizeof(CSimpleColorSelect), 5);

// OFFSET: 0x00523030/0x140171170
static void ValueTextureUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    // STATIC: 0x00DD3AD8
    static CImVector s_texels[64];

    switch (command) {
        case GxTex_Lock: {
            for (uint32_t i = 0; i < 64; i++) {
                s_texels[i] = *static_cast<CImVector*>(userArg);
            }

            break;
        }

        case GxTex_Latch: {
            texelStrideInBytes = 4 * width;
            texels = s_texels;
            break;
        }

        default:
            break;
    }
}

// INLINED
void CSimpleColorSelect::operator delete(void* ptr) {
    CSimpleColorSelect::s_allocator.PutData(ptr, NULL, 0);
}

// PROVEN: f39610ef IDENTICAL/NONE
// OFFSET: 0x00522D60
const char* CSimpleColorSelect::GetObjectTypeName() {
    return CSimpleColorSelect::s_objectTypeName;
}

// PROVEN: 84ef293d IDENTICAL/IDENTICAL
// OFFSET: 0x00522D70/0x140170E10
int32_t CSimpleColorSelect::GetScriptMetaTable() {
    return CSimpleColorSelect::s_metatable;
}

// PROVEN: 196d9f95 IDENTICAL/UNPROVEN
// OFFSET: 0x00522D80/0x140170E20
FrameScript_Object::ScriptIx* CSimpleColorSelect::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "oncolorselect", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,r,g,b) %s end";
        return &this->m_onColorSelect;
    }

    return NULL;
}

// PROVEN: 27e58956 IDENTICAL/IDENTICAL
// OFFSET: 0x00522DF0/0x140170E90
void CSimpleColorSelect::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onColorSelect, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: c26b8b50 IDENTICAL/IDENTICAL
// OFFSET: 0x00522E30/0x140170F00
void CSimpleColorSelect::UnregisterRegion(CSimpleRegion* region) {
    if (region == this->m_wheelTexture) {
        this->m_wheelTexture = NULL;
    }

    if (region == this->m_wheelThumbTexture) {
        this->m_wheelThumbTexture = NULL;
    }

    if (region == this->m_valueTexture) {
        this->m_valueTexture = NULL;
    }

    if (region == this->m_valueThumbTexture) {
        this->m_valueThumbTexture = NULL;
    }

    CSimpleFrame::UnregisterRegion(region);
}

// PROVEN: 85c8ff06 IDENTICAL/NONE
// OFFSET: 0x00522E80
void CSimpleColorSelect::OnLayerHide() {
    this->m_wheelTracking = false;
    this->m_valueTracking = false;
    CSimpleFrame::OnLayerHide();
}

// PROVEN: 633126a5 IDENTICAL/NONE
// OFFSET: 0x00522EA0
int32_t CSimpleColorSelect::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    this->m_wheelTracking = false;
    this->m_valueTracking = false;
    return CSimpleFrame::OnLayerMouseUp(evt, btn);
}

// OFFSET: [%0x00532FC0, %0x00533240]
CSimpleColorSelect::~CSimpleColorSelect() {
}

// OFFSET: 0x00522ED0/0x140170FA0
void CSimpleColorSelect::RunOnColorSelectScript() {
    if (!this->HasScript(this->m_onColorSelect)) {
        return;
    }

    lua_State* L = FrameScript_GetContext();
    C3Vector rgb;
    CImVector color;
    HSVtoRGB(this->m_hsv, rgb);
    color = rgb;
    lua_pushnumber(L, color.r * (1.0 / 255.0));
    lua_pushnumber(L, color.g * (1.0 / 255.0));
    lua_pushnumber(L, color.b * (1.0 / 255.0));
    this->RunScript(this->m_onColorSelect, 3, NULL);
}

// PROVEN: 37ad039a IDENTICAL/IDENTICAL
// OFFSET: 0x00522FB0/0x1401710A0
bool CSimpleColorSelect::IsA(int32_t type) {
    return type == CSimpleColorSelect::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 897e1693 IDENTICAL/DIFFERENT
// OFFSET: 0x00522FF0/0x140170F90
bool CSimpleColorSelect::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleColorSelect::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x005230A0/0x140171210
CSimpleColorSelect::CSimpleColorSelect(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_wheelTracking = false;
    this->m_valueTracking = false;
    this->m_wheelTexture = NULL;
    this->m_wheelThumbTexture = NULL;
    this->m_valueTexture = NULL;
    this->m_valueThumbTexture = NULL;
    this->m_hsv = C3Vector(0.0f, 0.0f, 1.0f);
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
}

// OFFSET: 0x00523180/0x140171330
void CSimpleColorSelect::UpdateValueTexture() {
    if (!this->m_valueTexture) {
        return;
    }

    C3Vector rgb;
    C3Vector hsv(this->m_hsv.x, this->m_hsv.y, 1.0f);
    HSVtoRGB(hsv, rgb);
    this->m_valueColor = rgb;
    GxTexUpdate(this->m_valueTexture->GetGxTex(), 0, 0, 0, 0, 0);
}

// OFFSET: 0x00523210/0x1401713D0
void CSimpleColorSelect::RepositionWheelThumb() {
    if (!this->m_wheelTexture || !this->m_wheelThumbTexture) {
        return;
    }

    C2Vector offset;
    float angle = this->m_hsv.x * 0.017453292f;
    offset.x = cos(angle);
    offset.y = sin(angle);

    float radius = this->m_wheelTexture->GetWidth() * 0.5f * this->m_hsv.y;
    offset.x *= radius;
    offset.y *= radius;

    this->m_wheelThumbTexture->SetPoint(FRAMEPOINT_CENTER, this->m_wheelTexture, FRAMEPOINT_CENTER, -offset.x, -offset.y, 1);
}

// PROVEN: d302d544 IDENTICAL/IDENTICAL
// OFFSET: 0x00523300/0x1401714D0
void CSimpleColorSelect::RepositionValueThumb() {
    if (!this->m_valueTexture || !this->m_valueThumbTexture) {
        return;
    }

    float offsetY = this->m_wheelTexture->GetHeight() * this->m_hsv.z;
    this->m_valueThumbTexture->SetPoint(FRAMEPOINT_CENTER, this->m_valueTexture, FRAMEPOINT_BOTTOM, 0.0f, offsetY, 1);
}

// OFFSET: 0x00523360/0x140171560
int32_t CSimpleColorSelect::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    C2Vector point = evt.m_data.m_mousePosition;

    if (this->m_wheelTexture && this->m_wheelTexture->PtInFrameRect(point)) {
        this->m_wheelTracking = true;
    }

    if (this->m_valueTexture && this->m_valueTexture->PtInFrameRect(point)) {
        this->m_valueTracking = true;
    }

    this->OnLayerTrackUpdate(evt);
    return CSimpleFrame::OnLayerMouseDown(evt, btn);
}

// OFFSET: 0x005233F0/0x140171610
void CSimpleColorSelect::SetColorWheelThumbTexture(CSimpleTexture* texture) {
    if (texture == this->m_wheelThumbTexture) {
        return;
    }

    if (this->m_wheelThumbTexture) {
        delete this->m_wheelThumbTexture;
    }

    if (texture) {
        texture->SetFrame(this, 3, 0, true);
        texture->MarkFramePointsUnused();
    }

    this->m_wheelThumbTexture = texture;
    this->RepositionWheelThumb();
}

// OFFSET: 0x00523440/0x140171680
void CSimpleColorSelect::SetColorValueTexture(CSimpleTexture* texture) {
    if (texture == this->m_valueTexture) {
        return;
    }

    if (this->m_valueTexture) {
        delete this->m_valueTexture;
    }

    if (texture) {
        CTexture* valueTexture = TextureCreate(8, 8, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &this->m_valueColor, ValueTextureUpdate, "ValueTexture", 0);
        texture->SetTexture(valueTexture);

        CImVector minColor(0, 0, 0, 0xFF);

        CImVector maxColor(0xFF, 0xFF, 0xFF, 0xFF);

        texture->SetVertexGradient(ORIENTATION_VERTICAL, minColor, maxColor);
        texture->SetFrame(this, 2, 0, true);

        if (valueTexture) {
            TextureRelease(valueTexture);
        }
    }

    this->m_valueTexture = texture;
    this->UpdateValueTexture();
}

// OFFSET: 0x00523510/0x1401717A0
void CSimpleColorSelect::SetColorValueThumbTexture(CSimpleTexture* texture) {
    if (texture == this->m_valueThumbTexture) {
        return;
    }

    if (this->m_valueThumbTexture) {
        delete this->m_valueThumbTexture;
    }

    if (texture) {
        texture->SetFrame(this, 3, 0, true);
        texture->MarkFramePointsUnused();
    }

    this->m_valueThumbTexture = texture;
    this->RepositionValueThumb();
}

// PROVEN: 180b9319 IDENTICAL/IDENTICAL
// OFFSET: 0x00523560/0x140171810
void CSimpleColorSelect::UpdateColor() {
    this->UpdateValueTexture();
    this->RepositionWheelThumb();
    this->RepositionValueThumb();
    this->RunOnColorSelectScript();
}

// OFFSET: 0x00523580
int32_t CSimpleColorSelect::OnLayerTrackUpdate(const CMouseEvent& evt) {
    if (this->m_wheelTracking || this->m_valueTracking) {
        if (this->m_wheelTracking) {
            C2Vector point = evt.m_data.m_mousePosition;
            CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
            this->m_wheelTexture->GetRect(&rect);

            float x = (point.x - (rect.maxX + rect.minX) * 0.5f) / ((rect.maxX - rect.minX) * 0.5f);
            float y = (point.y - (rect.maxY + rect.minY) * 0.5f) / ((rect.maxY - rect.minY) * 0.5f);
            float distance = sqrt(y * y + x * x);

            this->m_hsv.x = (atan2(y, x) + 3.1415927f) * 57.29578f;

            if (distance >= 1.0f) {
                distance = 1.0f;
            }

            this->m_hsv.y = distance;
        }

        if (this->m_valueTracking) {
            float y = evt.m_data.m_mousePosition.y;
            CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
            this->m_valueTexture->GetRect(&rect);

            float value = (y - rect.minY) / (rect.maxY - rect.minY);

            if (value < 0.0f) {
                this->m_hsv.z = 0.0f;
            } else if (value >= 1.0f) {
                this->m_hsv.z = 1.0f;
            } else {
                this->m_hsv.z = value;
            }
        }

        this->UpdateColor();
    }

    return CSimpleFrame::OnLayerTrackUpdate(evt);
}

// OFFSET: 0x00523750/0x140171A50
int32_t CSimpleColorSelect::SetColorWheelThumbTexture(const char* fileName) {
    if (this->m_wheelThumbTexture) {
        this->m_wheelThumbTexture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;

    if (!texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        if (texture) {
            delete texture;
        }

        return 0;
    }

    this->SetColorWheelThumbTexture(texture);
    return 1;
}

// OFFSET: 0x00523800/0x140171B50
int32_t CSimpleColorSelect::SetColorValueThumbTexture(const char* fileName) {
    if (this->m_valueThumbTexture) {
        this->m_valueThumbTexture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;

    if (!texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        if (texture) {
            delete texture;
        }

        return 0;
    }

    this->SetColorValueThumbTexture(texture);
    return 1;
}

// OFFSET: 0x005238B0/0x140171C50
void CSimpleColorSelect::SetColorWheelTexture(CSimpleTexture* texture) {
    if (texture == this->m_wheelTexture) {
        return;
    }

    if (this->m_wheelTexture) {
        delete this->m_wheelTexture;
    }

    if (texture) {
        // STATIC: 0x00DD3BDC
        static TSGrowableArray<CImVector> s_wheelTexels;

        if (!s_wheelTexels.Count()) {
            s_wheelTexels.SetCount(128 * 128);

            uint32_t index = 0;

            for (int32_t y = 63; y > -65; y--) {
                for (int32_t x = -64; x < 64; x++) {
                    CImVector color;
                    int32_t distanceSq = y * y + x * x;

                    if (distanceSq <= 64 * 64) {
                        C3Vector rgb;
                        C3Vector hsv((atan2(static_cast<double>(y), static_cast<double>(x)) + 3.1415927f) * 57.29578f, sqrt(static_cast<double>(distanceSq)) * 0.015625f, 1.0f);
                        HSVtoRGB(hsv, rgb);
                        color = rgb;
                    }

                    s_wheelTexels.m_data[index++] = color;
                }
            }
        }

        CTexture* wheelTexture = TextureCreate(128, 128, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &s_wheelTexels, EmblemTextureUpdate, "WheelTexture", 0);
        texture->SetTexture(wheelTexture);
        texture->SetFrame(this, 2, 0, true);

        if (wheelTexture) {
            TextureRelease(wheelTexture);
        }
    }

    this->m_wheelTexture = texture;
}

// PROVEN: 073fc542 IDENTICAL/DIFFERENT
// OFFSET: 0x00523A70/0x140171FC0
void CSimpleColorSelect::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x200000)) {
        return;
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_COLORWHEELTEXTURE:
                this->SetColorWheelTexture(LoadXML_Texture(child, this, status));
                break;

            case XML_COLORWHEELTHUMBTEXTURE:
                this->SetColorWheelThumbTexture(LoadXML_Texture(child, this, status));
                break;

            case XML_COLORVALUETEXTURE:
                this->SetColorValueTexture(LoadXML_Texture(child, this, status));
                break;

            case XML_COLORVALUETHUMBTEXTURE:
                this->SetColorValueThumbTexture(LoadXML_Texture(child, this, status));
                break;

            default:
                break;
        }
    }
}
