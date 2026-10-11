#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00CA3CB0
int32_t CSimpleStatusBar::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3964
int32_t CSimpleStatusBar::s_objectType;

// STATIC: 0x00CA3260
const char* CSimpleStatusBar::s_objectTypeName = "StatusBar";

// STATIC: 0x00DD3968
CDataAllocator CSimpleStatusBar::s_allocator(sizeof(CSimpleStatusBar), 5);

// INLINED
void CSimpleStatusBar::operator delete(void* ptr) {
    CSimpleStatusBar::s_allocator.PutData(ptr, NULL, 0);
}

// PROVEN: f9cea12a IDENTICAL/DIFFERENT
// OFFSET: 0x0050F230/0x140158430
void CSimpleStatusBar::RunOnMinMaxChangedScript() {
    if (this->HasScript(this->m_onMinMaxChanged)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, this->m_minValue);
        lua_pushnumber(L, this->m_maxValue);
        this->RunScript(this->m_onMinMaxChanged, 2, NULL);
    }
}

// OFFSET: 0x0050EE80
CSimpleStatusBar::~CSimpleStatusBar() {
}

// PROVEN: 09570588 IDENTICAL/IDENTICAL
// OFFSET: 0x0050EEA0/0x140158040
void CSimpleStatusBar::SetStatusBarColor(const CImVector& color) {
    if (this->m_barTexture) {
        this->m_barTexture->SetVertexColor(color);
    }
}

// PROVEN: 95543938 IDENTICAL/NONE
// OFFSET: 0x0050EEC0
const char* CSimpleStatusBar::GetObjectTypeName() {
    return CSimpleStatusBar::s_objectTypeName;
}

// PROVEN: 4459af13 IDENTICAL/IDENTICAL
// OFFSET: 0x0050EED0/0x140158070
int32_t CSimpleStatusBar::GetScriptMetaTable() {
    return CSimpleStatusBar::s_metatable;
}

// PROVEN: 0fec43d5 IDENTICAL/DIFFERENT
// OFFSET: 0x0050EEE0/0x140158080
FrameScript_Object::ScriptIx* CSimpleStatusBar::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onvaluechanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,value) %s end";
        return &this->m_onValueChanged;
    }

    if (!SStrCmp(name, "onminmaxchanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,min,max) %s end";
        return &this->m_onMinMaxChanged;
    }

    return NULL;
}

// PROVEN: 9a450fd0 IDENTICAL/NONE
// OFFSET: 0x0050EF90
void CSimpleStatusBar::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onValueChanged, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMinMaxChanged, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: 245976ac IDENTICAL/IDENTICAL
// OFFSET: 0x0050EFE0/0x1401581A0
void CSimpleStatusBar::UnregisterRegion(CSimpleRegion* region) {
    if (region == this->m_barTexture) {
        this->m_barTexture = NULL;
    }

    CSimpleFrame::UnregisterRegion(region);
}

// PROVEN: 07f92cae IDENTICAL/UNPROVEN
// OFFSET: 0x0050F010/0x1401581C0
void CSimpleStatusBar::SetBarTexture(CSimpleTexture* texture, uint32_t drawLayer, int8_t subLevel) {
    if (texture == this->m_barTexture) {
        return;
    }

    if (this->m_barTexture) {
        delete this->m_barTexture;
    }

    if (texture) {
        texture->SetFrame(this, drawLayer, subLevel, true);
        texture->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT, 0.0f, 0.0f, 1);
        texture->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this, FRAMEPOINT_BOTTOMRIGHT, 0.0f, 0.0f, 1);
        texture->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f, 1);
        texture->SetPoint(FRAMEPOINT_TOPRIGHT, this, FRAMEPOINT_TOPRIGHT, 0.0f, 0.0f, 1);
    }

    this->m_changed = 1;
    this->m_barTexture = texture;
}

// PROVEN: ebf163b1 IDENTICAL/NONE
// OFFSET: 0x0050F0E0
void CSimpleStatusBar::SetOrientation(ORIENTATION orientation) {
    this->m_orientation = orientation;
    this->m_changed = 1;
}

// PROVEN: cf2e93ab IDENTICAL/NONE
// OFFSET: 0x0050F110/0x140158320
void CSimpleStatusBar::SetReverseFill(int32_t reverse) {
    this->m_reverseFill = reverse;
    this->m_changed = 1;
}

// PROVEN: 9af81927 DIFFERENT/IDENTICAL
// OFFSET: 0x0050F490/0x140158740
void CSimpleStatusBar::SetMinMaxValues(float minValue, float maxValue) {
    if (minValue > maxValue) {
        minValue = maxValue;
    }

    if (this->m_rangeSet && this->m_minValue == minValue && this->m_maxValue == maxValue) {
        return;
    }

    this->m_minValue = minValue;
    this->m_changed = 1;
    this->m_rangeSet = 1;
    this->m_maxValue = maxValue;
    this->RunOnMinMaxChangedScript();

    if (this->m_valueSet) {
        this->SetValue(this->m_value);
    }
}

// PROVEN: f1af9da6 IDENTICAL/NONE
// OFFSET: 0x0050F1E0
void CSimpleStatusBar::RunOnValueChangedScript() {
    if (this->HasScript(this->m_onValueChanged)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, this->m_value);
        this->RunScript(this->m_onValueChanged, 1, NULL);
    }
}

// OFFSET: 0x0050F520
void CSimpleStatusBar::SetValue(float value) {
    if (!this->m_rangeSet) {
        return;
    }

    if (value > this->m_maxValue) {
        value = this->m_maxValue;
    } else if (value < this->m_minValue) {
        value = this->m_minValue;
    }

    if (this->m_valueSet && this->m_value == value) {
        return;
    }

    this->m_value = value;
    this->m_changed = 1;
    this->m_valueSet = 1;
    this->RunOnValueChangedScript();
}

// PROVEN: 0132627d IDENTICAL/UNPROVEN
// OFFSET: 0x0050F190/0x140158370
void CSimpleStatusBar::SetWidth(float width) {
    CSimpleFrame::SetWidth(width);
    float value = this->m_value;
    this->m_value = this->m_minValue;
    this->SetValue(value);
}

// PROVEN: 6eee3274 IDENTICAL/IDENTICAL
// OFFSET: 0x0050F140/0x140158340
float CSimpleStatusBar::GetAnimValue() const {
    float range = this->m_maxValue - this->m_minValue;

    if (range > 0.0f) {
        return (this->m_value - this->m_minValue) / range;
    }

    return 0.0f;
}

// PROVEN: 9a365044 IDENTICAL/IDENTICAL
// OFFSET: 0x0050F290/0x1401584C0
bool CSimpleStatusBar::IsA(int32_t type) {
    return type == CSimpleStatusBar::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: f2b20eb7 IDENTICAL/NONE
// OFFSET: 0x0050F2D0
bool CSimpleStatusBar::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleStatusBar::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x0050F310/0x140158560
CSimpleStatusBar::CSimpleStatusBar(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_changed = 0;
    this->m_rangeSet = 0;
    this->m_valueSet = 0;
    this->m_rotatesTexture = 0;
    this->m_reverseFill = 0;
    this->m_orientation = ORIENTATION_HORIZONTAL;
    this->m_minValue = 0.0f;
    this->m_maxValue = 0.0f;
    this->m_barTexture = NULL;
    this->m_value = 0.0f;
}

// OFFSET: 0x0050F3B0/0x140158620
int32_t CSimpleStatusBar::SetBarTexture(const char* fileName, uint32_t drawLayer, int8_t subLevel) {
    CSimpleTexture* texture = this->m_barTexture;

    if (texture) {
        texture->SetTexture(fileName, (texture->m_textureFlags >> 2) & 1, (texture->m_textureFlags >> 3) & 1, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        this->SetBarTexture(texture, drawLayer, subLevel);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;

    if (!texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        if (texture) {
            delete texture;
        }

        return 0;
    }

    this->SetBarTexture(texture, drawLayer, subLevel);
    return 1;
}

// OFFSET: 0x0050F580
void CSimpleStatusBar::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (!this->m_changed || !this->m_barTexture || !this->m_rangeSet || !this->m_valueSet) {
        return;
    }

    float value = this->GetAnimValue();
    float width;
    float height;
    this->GetSize(width, height, 0);

    if (value > 0.0f) {
        this->m_barTexture->Show();
    } else {
        this->m_barTexture->Hide();
    }

    uint32_t textureFlags = this->m_barTexture->m_textureFlags;
    float right = 0.0f;
    float left = 0.0f;
    float bottom = 0.0f;
    float top = 0.0f;

    if (this->m_orientation == ORIENTATION_VERTICAL) {
        if (!((textureFlags >> 2) & 1) && !((textureFlags >> 3) & 1)) {
            if (this->m_rotatesTexture) {
                C2Vector texCoords[4];
                texCoords[0].x = value;
                texCoords[0].y = 0.0f;
                texCoords[1].x = 0.0f;
                texCoords[1].y = 0.0f;
                texCoords[2].x = value;
                texCoords[2].y = 1.0f;
                texCoords[3].x = 0.0f;
                texCoords[3].y = 1.0f;
                this->m_barTexture->SetTexCoord(texCoords);
            } else {
                CRect texRect = { 1.0f - value, 0.0f, 1.0f, 1.0f };
                this->m_barTexture->SetTexCoord(texRect);
            }
        }

        if (this->m_reverseFill) {
            bottom = (1.0f - value) * height;
        } else {
            top = -((1.0f - value) * height);
        }
    } else {
        if (!((textureFlags >> 2) & 1) && !((textureFlags >> 3) & 1)) {
            if (this->m_rotatesTexture) {
                C2Vector texCoords[4];
                texCoords[0].x = 0.0f;
                texCoords[0].y = 1.0f;
                texCoords[1].x = 1.0f;
                texCoords[1].y = 1.0f;
                texCoords[2].x = 0.0f;
                texCoords[2].y = 1.0f - value;
                texCoords[3].x = 1.0f;
                texCoords[3].y = 1.0f - value;
                this->m_barTexture->SetTexCoord(texCoords);
            } else {
                CRect texRect = { 0.0f, 0.0f, 1.0f, value };
                this->m_barTexture->SetTexCoord(texRect);
            }
        }

        if (this->m_reverseFill) {
            left = (1.0f - value) * width;
        } else {
            right = -((1.0f - value) * width);
        }
    }

    this->m_barTexture->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT, left, top, 1);
    this->m_barTexture->SetPoint(FRAMEPOINT_TOPRIGHT, this, FRAMEPOINT_TOPRIGHT, right, top, 1);
    this->m_barTexture->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT, left, bottom, 1);
    this->m_barTexture->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this, FRAMEPOINT_BOTTOMRIGHT, right, bottom, 1);
    this->m_changed = 0;
}

// OFFSET: 0x0050F880/0x140158BE0
void CSimpleStatusBar::SetRotatesTexture(int32_t rotates) {
    if (rotates) {
        C2Vector texCoords[4];
        texCoords[0].x = 1.0f;
        texCoords[0].y = 0.0f;
        texCoords[1].x = 0.0f;
        texCoords[1].y = 0.0f;
        texCoords[2].x = 1.0f;
        texCoords[2].y = 1.0f;
        texCoords[3].x = 0.0f;
        texCoords[3].y = 1.0f;
        this->m_barTexture->SetTexCoord(texCoords);
    } else {
        CRect texRect = { 0.0f, 0.0f, 1.0f, 1.0f };
        this->m_barTexture->SetTexCoord(texRect);
    }

    this->m_rotatesTexture = rotates;
    this->m_changed = 1;
}

// OFFSET: 0x0050F960/0x140158CB0
void CSimpleStatusBar::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x80)) {
        return;
    }

    StatusBar* statusBar = static_cast<StatusBar*>(node);
    uint32_t drawLayer = statusBar->m_drawLayer;

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_BARCOLOR) {
            CImVector color(0, 0, 0, 0);

            if (LoadXML_Color(child, color, status)) {
                this->SetStatusBarColor(color);
            }
        } else if (child->m_element == XML_BARTEXTURE) {
            this->SetBarTexture(LoadXML_Texture(child, this, status), drawLayer, 0);
        }
    }

    if ((statusBar->m_valueFlags & 0x2) && (statusBar->m_valueFlags & 0x4)) {
        float minValue = statusBar->m_minValue;
        float maxValue = statusBar->m_maxValue;

        if (minValue < -1.0e12 || minValue > 1.0e12 || maxValue < -1.0e12 || maxValue > 1.0e12) {
            const char* name = this->GetName();
            status->Add(STATUS_ERROR, "Frame %s: Min or Max out of range", name ? name : "<unnamed>");
        } else if (maxValue - minValue > 1.0e12) {
            status->Add(STATUS_ERROR, "Frame %s: Min and Max too far apart", this->GetDisplayName());
        } else {
            this->SetMinMaxValues(minValue, maxValue);
        }

        if (statusBar->m_valueFlags & 0x8) {
            this->SetValue(statusBar->m_defaultValue);
        }
    }

    if (statusBar->m_valueFlags & 0x1) {
        this->m_changed = 1;
        this->m_orientation = static_cast<ORIENTATION>(statusBar->m_orientation);
    }

    if (statusBar->m_valueFlags & 0x20) {
        this->SetRotatesTexture((statusBar->m_valueFlags >> 7) & 1);
    }

    if (statusBar->m_valueFlags & 0x40) {
        this->m_reverseFill = (statusBar->m_valueFlags >> 8) & 1;
        this->m_changed = 1;
    }
}
