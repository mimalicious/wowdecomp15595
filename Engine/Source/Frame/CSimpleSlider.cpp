#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleSlider.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00CA44E0
int32_t CSimpleSlider::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3A74/0x140CB97D8
int32_t CSimpleSlider::s_objectType;

// STATIC: 0x00CA3464
const char* CSimpleSlider::s_objectTypeName = "Slider";

// STATIC: 0x00DD3A78
CDataAllocator CSimpleSlider::s_allocator(sizeof(CSimpleSlider), 5);

// INLINED
void CSimpleSlider::operator delete(void* ptr) {
    if (ptr) {
        CSimpleSlider::s_allocator.PutData(ptr, NULL, 0);
    }
}

// PROVEN: 5c7338db IDENTICAL/UNPROVEN
// OFFSET: 0x00521140/0x14016EB80
void CSimpleSlider::RunOnMinMaxChangedScript() {
    if (this->HasScript(this->m_onMinMaxChanged)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, this->m_baseValue);
        lua_pushnumber(L, this->m_range + this->m_baseValue);
        this->RunScript(this->m_onMinMaxChanged, 2, NULL);
    }
}

// OFFSET: 0x00520E30
float CSimpleSlider::SnapValueToStep(float value) {
    if (0.0f == this->m_valueStep) {
        return value;
    }

    float offset = value - this->m_baseValue;
    float half = this->m_valueStep * 0.5f;

    if (offset > 0.0f) {
        return static_cast<float>(static_cast<int32_t>((offset + half) / this->m_valueStep)) * this->m_valueStep + this->m_baseValue;
    }

    return static_cast<float>(static_cast<int32_t>((offset - half) / this->m_valueStep)) * this->m_valueStep + this->m_baseValue;
}

// PROVEN: 90afd404 IDENTICAL/UNPROVEN
// OFFSET: 0x00520EF0/0x14016E8B0
FrameScript_Object::ScriptIx* CSimpleSlider::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onvaluechanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,value,userInput) %s end";
        return &this->m_onValueChanged;
    }

    if (!SStrCmp(name, "onminmaxchanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,min,max) %s end";
        return &this->m_onMinMaxChanged;
    }

    return NULL;
}

// PROVEN: df07a5a6 IDENTICAL/IDENTICAL
// OFFSET: 0x00520FA0/0x14016E950
void CSimpleSlider::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onValueChanged, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMinMaxChanged, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: cb8f6130 IDENTICAL/IDENTICAL
// OFFSET: 0x00520FF0/0x14016E9D0
void CSimpleSlider::UnregisterRegion(CSimpleRegion* region) {
    if (region == this->m_thumbTexture) {
        this->m_thumbTexture = NULL;
    }

    CSimpleFrame::UnregisterRegion(region);
}

// OFFSET: 0x00521020/0x14016E9F0
void CSimpleSlider::SetThumbTexture(CSimpleTexture* texture, uint32_t drawLayer) {
    if (texture == this->m_thumbTexture) {
        return;
    }

    if (this->m_thumbTexture) {
        delete this->m_thumbTexture;
    }

    if (texture) {
        texture->SetFrame(this, drawLayer, 0, true);
        texture->MarkFramePointsUnused();
    }

    this->m_changed = 1;
    this->m_thumbTexture = texture;
}

// OFFSET: 0x00521070/0x14016EA70
void CSimpleSlider::SetOrientation(ORIENTATION orientation) {
    this->m_orientation = orientation;

    if (this->m_thumbTexture) {
        this->m_thumbTexture->MarkFramePointsUnused();
    }

    this->m_changed = 1;
}

// PROVEN: 041c5feb IDENTICAL/IDENTICAL
// OFFSET: 0x005210A0/0x14016EAB0
void CSimpleSlider::OnFrameSizeChanged(const CRect& rect) {
    CSimpleFrame::OnFrameSizeChanged(rect);
    this->m_changed = 1;
}

// PROVEN: 2bb853f5 IDENTICAL/NONE
// OFFSET: 0x005210C0
void CSimpleSlider::OnLayerHide() {
    this->m_buttonDown = 0;
    CSimpleFrame::OnLayerHide();
}

// PROVEN: f8bb1aa7 IDENTICAL/NONE
// OFFSET: 0x005210D0
int32_t CSimpleSlider::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    this->m_buttonDown = 0;
    return CSimpleFrame::OnLayerMouseUp(evt, btn);
}

// OFFSET: 0x005211B0
CSimpleSlider::CSimpleSlider(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_changed = 0;
    this->m_rangeSet = 0;
    this->m_valueSet = 0;
    this->m_buttonDown = 0;
    this->m_baseValue = 0.0f;
    this->m_range = 0.0f;
    this->m_value = 0.0f;
    this->m_valueStep = 0.0f;
    this->m_thumbTexture = NULL;
    this->m_orientation = ORIENTATION_VERTICAL;
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
}

// PROVEN: dd6f8f1c IDENTICAL/NONE
// OFFSET: 0x00521230
const char* CSimpleSlider::GetObjectTypeName() {
    return CSimpleSlider::s_objectTypeName;
}

// PROVEN: 3c9e9cd3 IDENTICAL/IDENTICAL
// OFFSET: 0x00521240/0x14016ECA0
int32_t CSimpleSlider::GetScriptMetaTable() {
    return CSimpleSlider::s_metatable;
}

// PROVEN: c2c7203d IDENTICAL/IDENTICAL
// OFFSET: 0x00521260/0x14016ECC0
bool CSimpleSlider::IsA(int32_t type) {
    return type == CSimpleSlider::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: ef82c992 IDENTICAL/DIFFERENT
// OFFSET: 0x005212A0/0x14016ECB0
bool CSimpleSlider::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleSlider::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// INLINED
CSimpleSlider::~CSimpleSlider() {
    if (this->m_thumbTexture) {
        delete this->m_thumbTexture;
        this->m_changed = 1;
        this->m_thumbTexture = NULL;
    }
}

// OFFSET: 0x005212E0/0x14016ED60
int32_t CSimpleSlider::SetThumbTexture(const char* fileName) {
    if (this->m_thumbTexture) {
        this->m_thumbTexture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;

    if (texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        this->SetThumbTexture(texture, 3);
        return 1;
    }

    if (texture) {
        delete texture;
    }

    return 0;
}

// PROVEN: f1a772fa IDENTICAL/NONE
// OFFSET: 0x005210E0
void CSimpleSlider::RunOnValueChangedScript(bool userInput) {
    if (this->HasScript(this->m_onValueChanged)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, this->m_value);
        lua_pushboolean(L, userInput);
        this->RunScript(this->m_onValueChanged, 2, NULL);
    }
}

// OFFSET: 0x00521390/0x14016EE70
void CSimpleSlider::SetValue(float value, bool userInput) {
    if (!this->m_rangeSet) {
        return;
    }

    float maxValue = this->m_range + this->m_baseValue;
    value = std::min(std::max(value, this->m_baseValue), maxValue);
    value = this->SnapValueToStep(value);
    value = maxValue < value + this->m_valueStep ? maxValue : value;
    value = value - this->m_valueStep < this->m_baseValue ? this->m_baseValue : value;

    if (this->m_valueSet && fabsf(value - this->m_value) < 0.00000023841858f && !userInput) {
        return;
    }

    this->m_value = value;
    this->m_changed = 1;
    this->m_valueSet = 1;
    this->RunOnValueChangedScript(userInput);
}

// OFFSET: 0x00521490/0x14016EF70
void CSimpleSlider::SetValueStep(float step) {
    step = std::max(step, FLT_EPSILON);

    if (fabsf(step - this->m_valueStep) < 0.00000023841858f) {
        return;
    }

    this->m_valueStep = step;

    if (this->m_valueSet) {
        this->SetValue(this->m_value, false);
    }
}

// OFFSET: 0x005214F0
void CSimpleSlider::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (!this->m_changed) {
        return;
    }

    CSimpleTexture* thumb = this->m_thumbTexture;

    if (!thumb || !this->m_rangeSet || !this->m_valueSet) {
        return;
    }

    float range = this->m_range + this->m_baseValue - this->m_baseValue;
    float position = fequal(range, 0.0f) ? 0.0f : (this->m_value - this->m_baseValue) / range;

    if (this->m_orientation != ORIENTATION_HORIZONTAL) {
        float height = (this->m_rect.maxY - this->m_rect.minY) / this->m_layoutScale;
        thumb->SetPoint(FRAMEPOINT_TOP, this, FRAMEPOINT_TOP, 0.0f, -((height - thumb->GetHeight()) * position), 1);
    } else {
        float width = (this->m_rect.maxX - this->m_rect.minX) / this->m_layoutScale;
        thumb->SetPoint(FRAMEPOINT_LEFT, this, FRAMEPOINT_LEFT, (width - thumb->GetWidth()) * position, 0.0f, 1);
    }

    this->m_changed = 0;
}

// OFFSET: 0x00521620
int32_t CSimpleSlider::OnLayerTrackUpdate(const CMouseEvent& evt) {
    if (this->m_buttonDown && this->m_thumbTexture) {
        float length;
        float offset;

        if (this->m_orientation != ORIENTATION_HORIZONTAL) {
            length = this->m_rect.maxY - this->m_rect.minY - this->m_thumbTexture->GetHeight();
            offset = this->m_rect.maxY - this->m_thumbTexture->GetHeight() * 0.5f - evt.m_data.m_mousePosition.y;
        } else {
            length = this->m_rect.maxX - this->m_rect.minX - this->m_thumbTexture->GetWidth();
            offset = evt.m_data.m_mousePosition.x - (this->m_thumbTexture->GetWidth() * 0.5f + this->m_rect.minX);
        }

        this->SetValue((this->m_range + this->m_baseValue - this->m_baseValue) * (offset / length) + this->m_baseValue, true);
    }

    return CSimpleFrame::OnLayerTrackUpdate(evt);
}

// PROVEN: 2b314e5b IDENTICAL/IDENTICAL
// OFFSET: 0x00521710/0x14016F280
int32_t CSimpleSlider::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    if (!(this->m_frameFlags & 0x400)) {
        this->m_buttonDown = 1;
        this->OnLayerTrackUpdate(evt);
    }

    return CSimpleFrame::OnLayerMouseDown(evt, btn);
}

// PROVEN: 15300170 IDENTICAL/UNPROVEN
// OFFSET: 0x005217B0/0x14016F350
void CSimpleSlider::SetMinMaxValues(float min, float max) {
    float range = max - min;

    if (this->m_rangeSet && fabsf(range - this->m_range) < 0.00000023841858f && fabsf(this->m_baseValue - min) < 0.00000023841858f) {
        return;
    }

    this->m_baseValue = min;
    this->m_changed = 1;
    this->m_rangeSet = 1;
    this->m_range = range;
    this->RunOnMinMaxChangedScript();

    if (this->m_valueSet) {
        this->SetValue(this->m_value, false);
    }
}

// OFFSET: 0x00521850/0x14016F3F0
void CSimpleSlider::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x800)) {
        return;
    }

    Slider* slider = static_cast<Slider*>(node);

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_THUMBTEXTURE) {
            this->SetThumbTexture(LoadXML_Texture(child, this, status), slider->m_drawLayer);
        }
    }

    this->SetValueStep(slider->m_valueFlags & 0x10 ? slider->m_valueStep : 0.001f);

    if ((slider->m_valueFlags & 0x2) && (slider->m_valueFlags & 0x4)) {
        this->SetMinMaxValues(slider->m_minValue, slider->m_maxValue);

        if (slider->m_valueFlags & 0x8) {
            this->SetValue(slider->m_defaultValue, false);
        }
    }

    if (slider->m_valueFlags & 0x1) {
        this->SetOrientation(static_cast<ORIENTATION>(slider->m_orientation));
    }
}
