#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/CCharEvent.h"
#include "Engine/Source/Event/CKeyEvent.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Frame/CBackdropGenerator.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00BB834C
int32_t CSimpleFrame::s_metatable = LUA_NOREF;

// STATIC: %0x00CD128C
int32_t CSimpleFrame::s_objectType;

// STATIC: %0x00BB8260
const char* CSimpleFrame::s_objectTypeName = "Frame";

// STATIC: %0x00CD1288
int32_t CSimpleFrame::s_frameCount;

// STATIC: %0x00CD1290
CDataAllocator CSimpleFrame::s_allocator(sizeof(CSimpleFrame), 100);

// STATIC: %0x00BB8264
static float s_dragThreshold = 0.0001f;

// STATIC: %0x00CD127C
bool CSimpleFrame::s_defaultPreload;

// OFFSET: %0x00512750
static const char* KeyCodeToString(KEY key) {
    static char charBuf[8];

    if (key - 33 <= 0xDE) {
        sputu8(key, charBuf);
        return charBuf;
    }

    switch (key) {
        case KEY_NONE:
            return "NONE";
        case KEY_LSHIFT:
            return "LSHIFT";
        case KEY_RSHIFT:
            return "RSHIFT";
        case KEY_LCONTROL:
            return "LCTRL";
        case KEY_RCONTROL:
            return "RCTRL";
        case KEY_LALT:
            return "LALT";
        case KEY_RALT:
            return "RALT";
        case KEY_SPACE:
            return "SPACE";
        case KEY_NUMPAD0:
        case KEY_NUMPAD1:
        case KEY_NUMPAD2:
        case KEY_NUMPAD3:
        case KEY_NUMPAD4:
        case KEY_NUMPAD5:
        case KEY_NUMPAD6:
        case KEY_NUMPAD7:
        case KEY_NUMPAD8:
        case KEY_NUMPAD9:
            SStrPrintf(charBuf, sizeof(charBuf), "NUMPAD%d", key - KEY_NUMPAD0);
            return charBuf;
        case KEY_NUMPAD_PLUS:
            return "NUMPADPLUS";
        case KEY_NUMPAD_MINUS:
            return "NUMPADMINUS";
        case KEY_NUMPAD_MULTIPLY:
            return "NUMPADMULTIPLY";
        case KEY_NUMPAD_DIVIDE:
            return "NUMPADDIVIDE";
        case KEY_NUMPAD_DECIMAL:
            return "NUMPADDECIMAL";
        case KEY_NUMPAD_EQUALS:
            return "NUMPADEQUALS";
        case KEY_ESCAPE:
            return "ESCAPE";
        case KEY_ENTER:
            return "ENTER";
        case KEY_BACKSPACE:
            return "BACKSPACE";
        case KEY_TAB:
            return "TAB";
        case KEY_LEFT:
            return "LEFT";
        case KEY_UP:
            return "UP";
        case KEY_RIGHT:
            return "RIGHT";
        case KEY_DOWN:
            return "DOWN";
        case KEY_INSERT:
            return "INSERT";
        case KEY_DELETE:
            return "DELETE";
        case KEY_HOME:
            return "HOME";
        case KEY_END:
            return "END";
        case KEY_PAGEUP:
            return "PAGEUP";
        case KEY_PAGEDOWN:
            return "PAGEDOWN";
        case KEY_CAPSLOCK:
            return "CAPSLOCK";
        case KEY_NUMLOCK:
            return "NUMLOCK";
        case KEY_PRINTSCREEN:
            return "PRINTSCREEN";
        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
        case KEY_F6:
        case KEY_F7:
        case KEY_F8:
        case KEY_F9:
        case KEY_F10:
        case KEY_F11:
        case KEY_F12:
        case KEY_F14:
        case KEY_F15:
        case KEY_F16:
        case KEY_F17:
        case KEY_F18:
        case KEY_F19:
            SStrPrintf(charBuf, sizeof(charBuf), "F%d", key - KEY_F1 + 1);
            return charBuf;
        default:
            return "UNKNOWN";
    }
}

// INLINED
void CSimpleFrame::operator delete(void* ptr) {
    CSimpleFrame::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: %0x00515F40
CSimpleFrame::CSimpleFrame(CSimpleFrame* parent) {
    this->m_frameScale = 1.0f;
    this->m_depth = 0.0f;
    this->m_parentDepth = 1.0f;
    this->m_hoverFrame = NULL;
    this->m_titleRegion = NULL;
    this->m_backdropGenerator = NULL;
    this->m_id = 0;
    this->m_frameFlags = 0;
    this->m_level = 0;
    this->m_batchDirty = 0;
    this->m_enabledEventsBitmask = 0;
    this->m_dragButtons = 0;
    this->m_mousePosition.x = 0.0f;
    this->m_mousePosition.y = 0.0f;
    this->m_hitRect.minY = 0.0f;
    this->m_hitRect.minX = 0.0f;
    this->m_hitRect.maxY = 0.0f;
    this->m_hitRect.maxX = 0.0f;
    this->m_hitRectInsets.minY = 0.0f;
    this->m_hitRectInsets.minX = 0.0f;
    this->m_hitRectInsets.maxY = 0.0f;
    this->m_hitRectInsets.maxX = 0.0f;
    this->m_clampRectInsets.minY = 0.0f;
    this->m_clampRectInsets.minX = 0.0f;
    this->m_clampRectInsets.maxY = 0.0f;
    this->m_clampRectInsets.maxX = 0.0f;
    this->m_alpha = 0xFF;
    this->m_parentAlpha = 0xFF;
    this->m_animSavedAlpha = 0xFF;
    this->m_frameState = 1;
    this->m_isMouseDragging = 0;
    this->m_strata = FRAME_STRATA_MEDIUM;
    this->m_ignoreDepth = 0;
    this->m_highlightLocked = 0;
    this->m_isMouseDown = 0;
    this->m_isLoading = 0;
    this->m_forceAlpha = 0;

    CSimpleFrame::s_frameCount++;

    this->m_simpleTop = CSimpleTop::s_instance;
    this->m_simpleTop->RegisterFrame(this);

    this->SetParent(parent);

    this->m_drawLayerEnabled |= 0xF;
    this->m_drawLayerEnabled &= ~0x10;
    this->m_renderBatch[0] = NULL;
    this->m_renderBatch[1] = NULL;
    this->m_renderBatch[2] = NULL;
    this->m_renderBatch[3] = NULL;
    this->m_renderBatch[4] = NULL;
    this->m_preload = CSimpleFrame::s_defaultPreload;

    this->Show();
}

// OFFSET: [%0x005162F0, %0x005162C0, %0x00516950]
CSimpleFrame::~CSimpleFrame() {
    this->m_frameState = 3;
    this->m_simpleTop->UnregisterFrame(this);
    this->m_simpleTop->ValidateDeletedFrame(this);
    this->m_simpleTop = NULL;

    if (this->m_titleRegion) {
        delete this->m_titleRegion;
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_drawLayers[i].UnlinkAll();

        if (this->m_renderBatch[i]) {
            delete this->m_renderBatch[i];
        }
    }

    CSimpleRegion* region = this->m_regions.Head();

    while (region) {
        CSimpleRegion* next = this->m_regions.Next(region);
        this->m_regions.UnlinkNode(region);
        delete region;
        region = next;
    }

    this->m_renderList.UnlinkAll();

    while (this->m_children.Head()) {
        CSimpleFrame* child = this->m_children.Head()->m_frame;

        if (child) {
            delete child;
        }
    }

    if (this->m_parent) {
        this->m_parent->UnparentFrame(this);
    }

    if (this->m_backdropGenerator) {
        delete this->m_backdropGenerator;
    }

    this->m_attributes.Clear();

    CSimpleFrame::s_frameCount--;
}

// PROVEN: 7167ddb4 IDENTICAL/IDENTICAL
// OFFSET: 0x004694D0/0x14007D6E0
bool CSimpleFrame::IsA(int32_t type) {
    return type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 5e83dbf6 IDENTICAL/DIFFERENT
// OFFSET: 0x00469500/0x1401712D0
bool CSimpleFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleFrame::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: 88105321 IDENTICAL/NONE
// OFFSET: 0x00506120
const char* CSimpleFrame::GetObjectTypeName() {
    return CSimpleFrame::s_objectTypeName;
}

// PROVEN: f9cb8c7f IDENTICAL/IDENTICAL
// OFFSET: 0x00506130/0x14014CEF0
int32_t CSimpleFrame::GetScriptMetaTable() {
    return CSimpleFrame::s_metatable;
}

// PROVEN: 41141b61 IDENTICAL/UNPROVEN
// OFFSET: 0x00501D20/0x1401479A0
FrameScript_Object::ScriptIx* CSimpleFrame::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = FrameScript_Object::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onload", STORM_MAX_STR)) {
        return &this->m_onLoad;
    }

    if (!SStrCmp(name, "onsizechanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,w,h) %s end";
        return &this->m_onSizeChanged;
    }

    if (!SStrCmp(name, "onupdate", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,elapsed) %s end";
        return &this->m_onUpdate;
    }

    if (!SStrCmp(name, "onshow", STORM_MAX_STR)) {
        return &this->m_onShow;
    }

    if (!SStrCmp(name, "onhide", STORM_MAX_STR)) {
        return &this->m_onHide;
    }

    if (!SStrCmp(name, "onenter", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,motion) %s end";
        data.m_event = SIMPLE_EVENT_MOUSE;
        return &this->m_onCursorEnter;
    }

    if (!SStrCmp(name, "onleave", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,motion) %s end";
        data.m_event = SIMPLE_EVENT_MOUSE;
        return &this->m_onCursorExit;
    }

    if (!SStrCmp(name, "onmousedown", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button) %s end";
        data.m_event = SIMPLE_EVENT_MOUSE;
        return &this->m_onMouseDown;
    }

    if (!SStrCmp(name, "onmouseup", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button) %s end";
        data.m_event = SIMPLE_EVENT_MOUSE;
        return &this->m_onMouseUp;
    }

    if (!SStrCmp(name, "onmousewheel", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,delta) %s end";
        data.m_event = SIMPLE_EVENT_MOUSEWHEEL;
        return &this->m_onMouseWheel;
    }

    if (!SStrCmp(name, "ondragstart", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button) %s end";
        return &this->m_onDragStart;
    }

    if (!SStrCmp(name, "ondragstop", STORM_MAX_STR)) {
        return &this->m_onDragStop;
    }

    if (!SStrCmp(name, "onreceivedrag", STORM_MAX_STR)) {
        return &this->m_onReceiveDrag;
    }

    if (!SStrCmp(name, "onchar", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,text) %s end";
        data.m_event = SIMPLE_EVENT_CHAR;
        return &this->m_onChar;
    }

    if (!SStrCmp(name, "onkeydown", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,key) %s end";
        data.m_event = SIMPLE_EVENT_KEY;
        return &this->m_onKeyDown;
    }

    if (!SStrCmp(name, "onkeyup", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,key) %s end";
        data.m_event = SIMPLE_EVENT_KEY;
        return &this->m_onKeyUp;
    }

    if (!SStrCmp(name, "onattributechanged", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,name,value) %s end";
        return &this->m_onAttributeChanged;
    }

    if (!SStrCmp(name, "onenable", STORM_MAX_STR)) {
        return &this->m_onEnable;
    }

    if (!SStrCmp(name, "ondisable", STORM_MAX_STR)) {
        return &this->m_onDisable;
    }

    return NULL;
}


// PROVEN: 12f982a1 IDENTICAL/NONE
// OFFSET: 0x004F8970
uint8_t CSimpleFrame::GetEffectiveAlpha() {
    return this->m_alpha * this->m_parentAlpha / 255;
}

// PROVEN: 8330e518 IDENTICAL/IDENTICAL
// OFFSET: 0x00502290/0x140147CF0
void CSimpleFrame::SetFrameFlag(uint32_t flag, int32_t set) {
    if (set) {
        this->m_frameFlags |= flag;
    } else {
        this->m_frameFlags &= ~flag;
    }
}

// PROVEN: e79ef268 IDENTICAL/IDENTICAL
// OFFSET: 0x005022C0/0x140147D10
void CSimpleFrame::Raise() {
    this->m_simpleTop->RaiseFrame(this, 1);
}

// PROVEN: 4b152c86 IDENTICAL/UNPROVEN
// OFFSET: 0x005022D0/0x140147D30
int32_t CSimpleFrame::Lower() {
    return this->m_simpleTop->LowerFrame(this);
}

// PROVEN: 648274f4 IDENTICAL/NONE
// OFFSET: 0x005022E0
int32_t CSimpleFrame::DragBy(float offsetX, float offsetY, FRAMEPOINT point) {
    return CLayoutFrame::DragBy(this->m_simpleTop, offsetX, offsetY, point);
}

// PROVEN: 54410507 IDENTICAL/UNPROVEN
// OFFSET: 0x00502310/0x140147D70
void CSimpleFrame::PropagateProtectFlagToParent(uint32_t flag) {
    if (this->m_parent) {
        this->m_parent->SetProtectFlag(flag);
    }
}

// PROVEN: c963bbce IDENTICAL/IDENTICAL
// OFFSET: 0x00502380/0x140147DE0
void CSimpleFrame::SetFrameAlpha(uint8_t alpha) {
    if (alpha != this->m_alpha) {
        this->m_alpha = alpha;
        this->UpdateAlpha();
    }
}

// PROVEN: 6d03452e IDENTICAL/IDENTICAL
// OFFSET: 0x00502400/0x140147E40
void CSimpleFrame::RegisterForEvents(int32_t a2) {
    for (uint32_t i = 0; i < NUM_SIMPLE_EVENTS; i++) {
        if ((1 << i) & this->m_enabledEventsBitmask) {
            this->m_simpleTop->RegisterForEvent(this, static_cast<CSimpleEventType>(i), a2, -1);
        }
    }
}

// PROVEN: dd4bb26e IDENTICAL/IDENTICAL
// OFFSET: 0x00502440/0x140147EB0
void CSimpleFrame::UnregisterForEvents(int32_t a2) {
    for (uint32_t i = 0; i < NUM_SIMPLE_EVENTS; i++) {
        if ((1 << i) & this->m_enabledEventsBitmask) {
            this->m_simpleTop->UnregisterForEvent(this, static_cast<CSimpleEventType>(i), a2);
        }
    }
}

// PROVEN: b842938f IDENTICAL/IDENTICAL
// OFFSET: 0x00502480/0x140147F10
void CSimpleFrame::GetHitRectInsets(float& left, float& right, float& top, float& bottom) {
    left = this->m_hitRectInsets.minX;
    right = this->m_hitRectInsets.maxX;
    top = this->m_hitRectInsets.maxY;
    bottom = this->m_hitRectInsets.minY;
}

// PROVEN: f24e823b IDENTICAL/IDENTICAL
// OFFSET: 0x005024C0/0x140147F40
void CSimpleFrame::SetClampRectInsets(float left, float right, float top, float bottom) {
    this->m_clampRectInsets.minX = left;
    this->m_clampRectInsets.maxX = right;
    this->m_clampRectInsets.maxY = top;
    this->m_clampRectInsets.minY = bottom;
}

// PROVEN: ad4021d8 IDENTICAL/IDENTICAL
// OFFSET: 0x00502500/0x140147F70
void CSimpleFrame::GetClampRectInsets(float& left, float& right, float& top, float& bottom) {
    left = this->m_clampRectInsets.minX;
    right = this->m_clampRectInsets.maxX;
    top = this->m_clampRectInsets.maxY;
    bottom = this->m_clampRectInsets.minY;
}

// OFFSET: %0x00512680
bool CSimpleFrame::IsDragging() {
    return this->m_isMouseDragging || (this->m_parent && this->m_parent->IsDragging());
}

// OFFSET: %0x00515260
void CSimpleFrame::PreOnAnimUpdate() {
    if (this->m_animAlpha && this->m_animSavedAlpha != this->m_alpha) {
        this->m_alpha = this->m_animSavedAlpha;
        this->UpdateAlpha();
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->PreOnAnimUpdate();
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->PreOnAnimUpdate();
    }
}

// OFFSET: %0x005137B0
void CSimpleFrame::RunOnAttributeChangedScript(const char* name, int32_t luaRef) {
    if (!this->HasScript(this->m_onAttributeChanged)) {
        return;
    }

    lua_State* L = FrameScript_GetContext();
    const char* savedTaint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = NULL;
    }

    char* lowerName = static_cast<char*>(alloca(SStrLen(name) + 1));
    SStrCopy(lowerName, name, STORM_MAX_STR);
    SStrLower(lowerName);

    lua_pushstring(L, lowerName);
    lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
    this->RunScript(this->m_onAttributeChanged, 2, NULL);

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = savedTaint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }
}

// PROVEN: f3dfa27e IDENTICAL/DIFFERENT
// OFFSET: 0x00502570/0x140147FE0
void CSimpleFrame::AllowAttributeChanges(int32_t allow) {
    if (allow) {
        this->m_frameFlags |= 0x8000;
        this->m_simpleTop->RegisterForCleanup(this);
    } else {
        this->m_frameFlags &= ~0x8000u;
    }
}

// OFFSET: %0x005126F0
int32_t CSimpleFrame::AttributeChangesAllowed() {
    for (CSimpleFrame* frame = this; frame; frame = frame->m_parent) {
        if (frame->m_frameFlags & 0x8000) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: %0x005129B0
void CSimpleFrame::RunOnSizeChangedScript(float width, float height) {
    if (this->HasScript(this->m_onSizeChanged)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * width));
        lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * height));
        this->RunScript(this->m_onSizeChanged, 2, NULL);
    }
}

// PROVEN: 1816dafe IDENTICAL/UNPROVEN
// OFFSET: 0x005028F0/0x140148780
void CSimpleFrame::RunOnEnterScript(int32_t motion) {
    if (this->HasScript(this->m_onCursorEnter)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushboolean(L, motion);
        this->RunScript(this->m_onCursorEnter, 1, NULL);
    }
}

// PROVEN: 55499e11 IDENTICAL/UNPROVEN
// OFFSET: 0x00502940/0x1401487F0
void CSimpleFrame::RunOnLeaveScript(int32_t motion) {
    if (this->HasScript(this->m_onCursorExit)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushboolean(L, motion);
        this->RunScript(this->m_onCursorExit, 1, NULL);
    }
}

// PROVEN: f096a446 IDENTICAL/UNPROVEN
// OFFSET: 0x00502990/0x140148860
void CSimpleFrame::RunOnMouseWheelScript(int32_t delta) {
    if (this->HasScript(this->m_onMouseWheel)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, delta);
        this->RunScript(this->m_onMouseWheel, 1, NULL);
    }
}

// OFFSET: %0x00512B30
void CSimpleFrame::RunOnCharScript(const char* text) {
    if (this->HasScript(this->m_onChar)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, text);
        this->RunScript(this->m_onChar, 1, NULL);
    }
}

// OFFSET: %0x00512B80
void CSimpleFrame::RunOnKeyDownScript(const char* key) {
    if (this->HasScript(this->m_onKeyDown)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, key);
        this->RunScript(this->m_onKeyDown, 1, NULL);
    }
}

// OFFSET: %0x00512BD0
void CSimpleFrame::RunOnKeyUpScript(const char* key) {
    if (this->HasScript(this->m_onKeyUp)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, key);
        this->RunScript(this->m_onKeyUp, 1, NULL);
    }
}

// OFFSET: %0x00512C20
void CSimpleFrame::LoadXML_Layers(linkObject* node, CStatus* status) {
    for (linkObject* layerNode = node->m_child; layerNode; layerNode = layerNode->m_next) {
        if (layerNode->m_element != XML_LAYER) {
            const char* frameName = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Unknown child node in %s element: %s", frameName ? frameName : "<unnamed>", node->m_type.m_string, layerNode->m_type.m_string);
            continue;
        }

        Layer* layer = static_cast<Layer*>(layerNode);
        uint32_t drawLayer = layer->m_level;
        int8_t subLevel = layer->m_subLevel;

        for (linkObject* child = layerNode->m_child; child; child = child->m_next) {
            if (child->m_element == XML_FONTSTRING) {
                CSimpleFontString* string = LoadXML_String(child, this, status);

                if (string) {
                    string->SetFrame(this, drawLayer, 0, string->m_shown);
                }
            } else if (child->m_element == XML_TEXTURE) {
                CSimpleTexture* texture = LoadXML_Texture(child, this, status);

                if (texture) {
                    texture->SetFrame(this, drawLayer, subLevel, texture->m_shown);
                }
            } else {
                const char* frameName = this->GetName();
                status->Add(STATUS_WARNING, "Frame %s: Unknown child node in %s element: %s", frameName ? frameName : "<unnamed>", layerNode->m_type.m_string, child->m_type.m_string);
            }
        }
    }
}

// INLINED
void CSimpleFrame::NotifyDelayedLoad() {
    if (this->m_frameFlags & 0x2000) {
        this->m_batchDirty |= 0x20;
    } else if (this->m_simpleTop && this->m_visible) {
        this->m_simpleTop->NotifyFrameDelayedLoad(this);
    }
}

// PROVEN: 53f175c2 IDENTICAL/IDENTICAL
// OFFSET: 0x00502330/0x140147D90
bool CSimpleFrame::SetFrameScale(float scale, bool force) {
    if ((!force && fabs(scale - this->m_frameScale) < 0.00000023841858f) || scale == 0.0f) {
        return false;
    }

    this->m_frameScale = scale;
    return this->UpdateScale(force);
}

// PROVEN: 941257b8 IDENTICAL/IDENTICAL
// OFFSET: 0x005023B0/0x140147E00
bool CSimpleFrame::SetFrameDepth(float depth, bool force) {
    if (!force && fabs(depth - this->m_depth) < 0.00000023841858f) {
        return false;
    }

    this->m_depth = depth;
    return this->UpdateDepth(force);
}

// OFFSET: %0x00512E40
void CSimpleFrame::Hide() {
    if (!this->ProtectedFunctionsAllowed() && this->m_simpleTop->m_actionBlockedCallback) {
        this->m_simpleTop->m_actionBlockedCallback(NULL);
    }

    this->m_shown = 0;

    if (this->m_preload) {
        this->m_simpleTop->UpdateLoadingQueue(this, false);
    }

    this->HideThis();
}

// OFFSET: %0x00512E90
void CSimpleFrame::Show() {
    if (!this->ProtectedFunctionsAllowed() && this->m_simpleTop->m_actionBlockedCallback) {
        this->m_simpleTop->m_actionBlockedCallback(NULL);
    }

    this->m_shown = 1;

    if (this->m_preload) {
        this->m_simpleTop->UpdateLoadingQueue(this, true);
    }

    this->ShowThis(this->m_preload);
}

// OFFSET: %0x00512EF0
void CSimpleFrame::EnableEvent(CSimpleEventType eventType, int32_t priority) {
    if ((1 << eventType) & this->m_enabledEventsBitmask) {
        return;
    }

    if (this->m_visible) {
        this->m_simpleTop->RegisterForEvent(this, eventType, 0, priority);
    }

    this->m_enabledEventsBitmask |= 1 << eventType;
}

// OFFSET: %0x00512F40
void CSimpleFrame::DisableEvent(CSimpleEventType eventType) {
    if (!((1 << eventType) & this->m_enabledEventsBitmask)) {
        return;
    }

    if (this->m_visible) {
        this->m_simpleTop->UnregisterForEvent(this, eventType, 0);
    }

    this->m_enabledEventsBitmask &= ~(1 << eventType);
}

// OFFSET: %0x00512F80
int32_t CSimpleFrame::TestHitRect(const C2Vector& pt) {
    if (!(this->m_layoutFlags & 0x1)) {
        return 0;
    }

    if (!(this->m_frameFlags & 0x2000)) {
        return this->m_hitRect.IsPointInside(pt);
    }

    CSimpleFrame* scrollFrame = this->m_parent;

    if (!scrollFrame) {
        return 0;
    }

    while (scrollFrame->m_frameFlags & 0x2000) {
        scrollFrame = scrollFrame->m_parent;

        if (!scrollFrame) {
            return 0;
        }
    }

    CRect rect = scrollFrame->m_hitRect;
    CRect clip = rect &= this->m_hitRect;
    return clip.minX <= pt.x && clip.maxX >= pt.x && clip.minY <= pt.y && clip.maxY >= pt.y;
}

// PROVEN: a63b17dc IDENTICAL/IDENTICAL
// OFFSET: 0x00502F50/0x140148FD0
void CSimpleFrame::SetHitRectInsets(float left, float right, float top, float bottom) {
    this->m_hitRectInsets.minX = left;
    this->m_hitRectInsets.maxX = right;
    this->m_hitRectInsets.maxY = top;
    this->m_hitRectInsets.minY = bottom;

    this->m_hitRect.minX = left * this->m_layoutScale + this->m_rect.minX;
    this->m_hitRect.maxX = this->m_rect.maxX - right * this->m_layoutScale;
    this->m_hitRect.minY = bottom * this->m_layoutScale + this->m_rect.minY;
    this->m_hitRect.maxY = this->m_rect.maxY - top * this->m_layoutScale;
}

// OFFSET: %0x00513130
int32_t CSimpleFrame::GetHitRect(CRect& rect) {
    if (!(this->m_layoutFlags & 0x1)) {
        return 0;
    }

    rect = this->m_hitRect;
    return 1;
}

// OFFSET: %0x00515730
void CSimpleFrame::AddAnimAlpha(CScriptRegion* region, int16_t alpha) {
    if (region != this && region) {
        for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
            node->m_frame->AddAnimAlpha(region, alpha);
        }

        for (CSimpleRegion* child = this->m_regions.Head(); child; child = this->m_regions.Next(child)) {
            child->AddAnimAlpha(region, alpha);
        }

        return;
    }

    int16_t value = alpha + static_cast<uint8_t>(this->m_alpha * this->m_parentAlpha / 255);
    uint8_t newAlpha;

    if (value > 255) {
        newAlpha = 255;
    } else if (value < 0) {
        newAlpha = 0;
    } else {
        newAlpha = static_cast<uint8_t>(value);
    }

    if (newAlpha != this->m_alpha) {
        this->m_alpha = newAlpha;
        this->UpdateAlpha();
    }
}

// OFFSET: %0x00513170
int32_t CSimpleFrame::OnLayerTrackUpdate(const CMouseEvent& evt) {
    C2Vector pt = evt.m_data.m_mousePosition;

    if (this->m_isMouseDown && !this->m_isMouseDragging) {
        float dx = pt.x - this->m_mousePosition.x;
        float dy = pt.y - this->m_mousePosition.y;

        if (dx * dx + dy * dy >= s_dragThreshold) {
            this->m_isMouseDragging = 1;
            this->OnDragStart(evt);
            this->m_mousePosition.x = 0.0f;
            this->m_mousePosition.y = 0.0f;
        }
    }

    if (!this->TestHitRect(pt)) {
        return 0;
    }

    if (!this->m_ignoreDepth && g_theGxDevicePtr->StereoEnabled()) {
        g_theGxDevicePtr->CursorSetDepth(this->m_layoutDepth);
    }

    return 1;
}

// OFFSET: %0x00513250
void CSimpleFrame::OnFrameSizeChanged(const CRect& rect) {
    CLayoutFrame::OnFrameSizeChanged(rect);

    this->m_hitRect.minX = this->m_hitRectInsets.minX * this->m_layoutScale + this->m_rect.minX;
    this->m_hitRect.maxX = this->m_rect.maxX - this->m_hitRectInsets.maxX * this->m_layoutScale;
    this->m_hitRect.minY = this->m_hitRectInsets.minY * this->m_layoutScale + this->m_rect.minY;
    this->m_hitRect.maxY = this->m_rect.maxY - this->m_hitRectInsets.maxY * this->m_layoutScale;

    if (fabs(rect.maxX - rect.minX - (this->m_rect.maxX - this->m_rect.minX)) >= 0.00000023841858f || fabs(rect.maxY - rect.minY - (this->m_rect.maxY - this->m_rect.minY)) >= 0.00000023841858f) {
        if (this->m_backdropGenerator) {
            this->m_backdropGenerator->Generate(&this->m_rect);
        }

        float scale = 1.0f / this->m_layoutScale;
        this->OnFrameSizeChanged((this->m_rect.maxX - this->m_rect.minX) * scale, (this->m_rect.maxY - this->m_rect.minY) * scale);
    }

    this->m_simpleTop->NotifyFrameMovedOrResized(this);
}

// PROVEN: b5db8ef9 IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CSimpleFrame::OnLayerIme(const CImeEvent& evt) {
    return 0;
}

// PROVEN: 08408449 IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CSimpleFrame::OnLayerKeyDownRepeat(const CKeyEvent& evt) {
    return 0;
}

// OFFSET: %0x00513380
int32_t CSimpleFrame::OnLayerChar(const CCharEvent& evt) {
    if (!this->m_visible || !this->HasScript(this->m_onChar)) {
        return 0;
    }

    char text[8];
    sputu8(evt.m_data.m_ch, text);
    this->RunOnCharScript(text);

    return !(this->m_frameFlags & 0x200000) || this->IsProtected();
}

// OFFSET: %0x00513400
int32_t CSimpleFrame::OnLayerKeyDown(const CKeyEvent& evt) {
    if (!this->m_visible || (!this->HasScript(this->m_onKeyDown) && !this->HasScript(this->m_onKeyUp))) {
        return 0;
    }

    this->RunOnKeyDownScript(KeyCodeToString(evt.m_data.m_key));

    return !(this->m_frameFlags & 0x200000) || this->IsProtected();
}

// OFFSET: %0x00513480
int32_t CSimpleFrame::OnLayerKeyUp(const CKeyEvent& evt) {
    if (!this->m_visible || !this->HasScript(this->m_onKeyUp)) {
        return 0;
    }

    this->RunOnKeyUpScript(KeyCodeToString(evt.m_data.m_key));

    return !(this->m_frameFlags & 0x200000) || this->IsProtected();
}

// OFFSET: %0x005134F0
int32_t CSimpleFrame::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    if (!btn) {
        if (this->m_dragButtons & evt.m_data.m_button) {
            this->m_isMouseDown = 1;
            this->m_isMouseDragging = 0;
            this->m_mouseDragButton = evt.m_data.m_button;
            this->m_mousePosition = evt.m_data.m_mousePosition;
        }

        btn = GetMouseButtonName(evt.m_data.m_button);
    }

    const char* savedButton = this->m_simpleTop->m_mouseButtonName;
    this->m_simpleTop->m_mouseButtonName = btn;

    if (this->HasScript(this->m_onMouseDown)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        this->RunScript(this->m_onMouseDown, 1, NULL);
    }

    this->m_simpleTop->m_mouseButtonName = savedButton;
    return 0;
}

// OFFSET: %0x005135B0
int32_t CSimpleFrame::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    if (!btn) {
        if (this->m_dragButtons & evt.m_data.m_button) {
            this->m_isMouseDown = 0;

            if (this->m_isMouseDragging) {
                this->OnDragStop(evt);

                CSimpleFrame* focus = this->m_simpleTop->m_mouseFocus;

                if (focus) {
                    focus->OnReceiveDrag(evt);
                }

                this->m_isMouseDragging = 0;
                return 1;
            }
        }

        btn = GetMouseButtonName(evt.m_data.m_button);
    }

    const char* savedButton = this->m_simpleTop->m_mouseButtonName;
    this->m_simpleTop->m_mouseButtonName = btn;

    if (this->HasScript(this->m_onMouseUp)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        this->RunScript(this->m_onMouseUp, 1, NULL);
    }

    this->m_simpleTop->m_mouseButtonName = savedButton;
    return 0;
}

// OFFSET: %0x00513690
int32_t CSimpleFrame::OnLayerMouseWheel(const CMouseEvent& evt) {
    if (!this->m_visible || !this->HasScript(this->m_onMouseWheel)) {
        return 0;
    }

    this->RunOnMouseWheelScript(static_cast<int32_t>(evt.m_data.m_wheelAmount) >= 0 ? 1 : -1);
    return 1;
}

// OFFSET: %0x005136E0
void CSimpleFrame::OnDragStart(const CMouseEvent& evt) {
    uint32_t button = 0;

    for (int32_t i = 0; i < 31; i++) {
        if (evt.m_data.m_buttonState & (1 << i)) {
            button = 1 << i;
            break;
        }
    }

    if (this->HasScript(this->m_onDragStart)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, GetMouseButtonName(button));
        this->RunScript(this->m_onDragStart, 1, NULL);
    }
}

// PROVEN: 75fdaf5b IDENTICAL/IDENTICAL
// OFFSET: 0x00503620/0x1401497C0
void CSimpleFrame::OnDragStop(const CMouseEvent& evt) {
    if (this->HasScript(this->m_onDragStop)) {
        this->RunScript(this->m_onDragStop, 0, NULL);
    }
}

// PROVEN: 8b0c6d16 IDENTICAL/IDENTICAL
// OFFSET: 0x00503650/0x140149800
void CSimpleFrame::OnReceiveDrag(const CMouseEvent& evt) {
    if (this->HasScript(this->m_onReceiveDrag)) {
        this->RunScript(this->m_onReceiveDrag, 0, NULL);
    }
}

// OFFSET: %0x005138A0
void CSimpleFrame::NotifyScrollParent() {
    if (!(this->m_frameFlags & 0x2000) || !this->m_visible) {
        return;
    }

    CSimpleFrame* frame = this;

    while (!(frame->m_frameFlags & 0x4000)) {
        frame = frame->m_parent;
    }

    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(frame->m_parent);

    if (scrollFrame && scrollFrame->m_frameState != 3) {
        scrollFrame->m_scrollChildUpdate = 1;
    }
}

// OFFSET: %0x00513910
void CSimpleFrame::LoadXML_Scripts(linkObject* node, CStatus* status) {
    lua_State* L = FrameScript_GetContext();

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (!(child->m_flags & 0x20)) {
            continue;
        }

        Function* function = static_cast<Function*>(child);
        this->LoadFunction(L, function, status);

        if (function->m_functionFlags & 0x2) {
            this->EnableEvent(SIMPLE_EVENT_CHAR, -1);
        }

        if (function->m_functionFlags & 0x4) {
            this->EnableEvent(SIMPLE_EVENT_KEY, -1);
        }

        if (function->m_functionFlags & 0x8) {
            this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
        }

        if (function->m_functionFlags & 0x10) {
            this->EnableEvent(SIMPLE_EVENT_MOUSEWHEEL, -1);
        }
    }
}

// OFFSET: %0x00513A90
void CSimpleFrame::NotifyDrawLayerChanged(uint32_t drawLayer) {
    if (this->m_frameFlags & 0x2000) {
        this->m_batchDirty |= 1 << drawLayer;
    } else if (this->m_simpleTop && this->m_visible) {
        this->m_simpleTop->NotifyFrameLayerChanged(this, drawLayer);
    }

    this->NotifyScrollParent();
}

// OFFSET: %0x00513AF0
void CSimpleFrame::OnLayerShow() {
    if (this->HasScript(this->m_onShow) && !this->m_isLoading) {
        this->RunScript(this->m_onShow, 0, NULL);
    }

    this->NotifyScrollParent();
}

// OFFSET: %0x00513B30
void CSimpleFrame::OnLayerHide() {
    if (this->HasScript(this->m_onHide) && !this->m_isLoading) {
        this->RunScript(this->m_onHide, 0, NULL);
    }

    this->NotifyScrollParent();
}

// PROVEN: d496d9cb IDENTICAL/DIFFERENT
// OFFSET: 0x00503A40/0x140149DE0
void CSimpleFrame::OnFrameSizeChanged(float width, float height) {
    this->RunOnSizeChangedScript(width, height);
    this->NotifyScrollParent();
}

// OFFSET: %0x00513BA0
void CSimpleFrame::PreLoadXML(linkObject* node, CStatus* status) {
    if (node->m_flags & 0x1000000) {
        this->SetName(node->m_name.m_string);
    }

    Frame* frame = static_cast<Frame*>(node);

    if (frame->m_frameFlags & 0x800000) {
        int32_t id = frame->m_id;

        if (id >= 0) {
            this->m_id = id;
        }
    }

    this->m_isLoading = 1;
    this->SetDeferredResize(1);

    if (this->m_titleRegion) {
        this->m_titleRegion->SetDeferredResize(1);
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->SetDeferredResize(1);
    }
}

// OFFSET: %0x00513C40
void CSimpleFrame::PostLoadXML(linkObject* node, CStatus* status) {
    this->m_isLoading = 0;

    if (this->m_visible) {
        this->SetDeferredResize(0);

        if (this->m_titleRegion) {
            this->m_titleRegion->SetDeferredResize(0);
        }

        for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
            if (region->m_visible) {
                region->SetDeferredResize(0);
            }
        }
    }

    int32_t active;
    int32_t activeAlpha;
    this->GetActiveAnimInfo(CSimpleAlphaAnim::GetObjectType(), active, activeAlpha);

    if (active) {
        this->AnimData_Init();
        this->m_animAlpha = activeAlpha;
        this->m_animSavedAlpha = this->m_alpha * this->m_parentAlpha / 255;

        for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
            region->AnimActivated(NULL, 1, activeAlpha);
        }
    }

    if (this->HasScript(this->m_onLoad)) {
        this->RunScript(this->m_onLoad, 0, NULL);
    }

    if (this->m_visible && this->HasScript(this->m_onShow) && !this->m_isLoading) {
        this->RunScript(this->m_onShow, 0, NULL);
    }
}

// OFFSET: %0x00513A30
void CSimpleFrame::SetBackdrop(CBackdropGenerator* backdrop) {
    if (this->m_backdropGenerator) {
        this->m_backdropGenerator->DestroyTextures();
        delete this->m_backdropGenerator;
    }

    if (backdrop) {
        backdrop->SetOutput(this);
    }

    this->m_backdropGenerator = backdrop;
}

// OFFSET: %0x00513DE0
void CSimpleFrame::EnableDrawLayer(uint32_t drawLayer) {
    this->m_drawLayerEnabled |= 1 << drawLayer;
    this->NotifyDrawLayerChanged(drawLayer);
}

// OFFSET: %0x00513E20
void CSimpleFrame::DisableDrawLayer(uint32_t drawLayer) {
    this->m_drawLayerEnabled &= ~(1 << drawLayer);
    this->NotifyDrawLayerChanged(drawLayer);
}

// OFFSET: %0x00514270
void CSimpleFrame::RegisterRegion(CSimpleRegion* region) {
    if (!region) {
        DebugValidate("region");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    this->m_regions.LinkToTail(region);
}

// OFFSET: %0x00513E50
void CSimpleFrame::UnregisterRegion(CSimpleRegion* region) {
    if (!region) {
        DebugValidate("region");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    this->m_regions.UnlinkNode(region);
}

// OFFSET: %0x005142B0
void CSimpleFrame::AddFrameRegion(CSimpleRegion* region, uint32_t drawLayer) {
    region->SetLayoutScale(this->m_layoutScale, false);
    this->m_drawLayers[drawLayer].LinkToTail(region);
    this->NotifyDrawLayerChanged(drawLayer);
}

// OFFSET: %0x00513E90
void CSimpleFrame::RemoveFrameRegion(CSimpleRegion* region, uint32_t drawLayer) {
    this->m_drawLayers[drawLayer].UnlinkNode(region);
    this->NotifyDrawLayerChanged(drawLayer);
}

// OFFSET: %0x00513F00
void CSimpleFrame::OnLayerUpdate(float elapsedSec) {
    if (this->HasScript(this->m_onUpdate)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, elapsedSec);
        this->RunScript(this->m_onUpdate, 1, NULL);
    }

    CScriptRegion::OnLayerUpdate(elapsedSec);

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        if (region->m_animGroups) {
            region->OnLayerUpdate(elapsedSec);
        }
    }
}

// OFFSET: %0x00513FA0
void CSimpleFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    if (!(this->m_drawLayerEnabled & (1 << layer))) {
        return;
    }

    for (CSimpleRegion* region = this->m_drawLayers[layer].Head(); region; region = this->m_drawLayers[layer].Next(region)) {
        region->Draw(batch);
    }
}

// OFFSET: %0x00514010
void CSimpleFrame::OnScreenSizeChanged() {
    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->OnScreenSizeChanged();
    }
}

// OFFSET: %0x00514050
void CSimpleFrame::OnLayerCursorEnter(int32_t a2) {
    if (this->m_hoverFrame) {
        this->m_hoverFrame->Show();
    }

    if (!this->m_highlightLocked) {
        this->m_drawLayerEnabled |= 0x10;
        this->NotifyDrawLayerChanged(4);
    }

    this->RunOnEnterScript(a2);
}

// OFFSET: %0x005140D0
void CSimpleFrame::OnLayerCursorExit(int32_t a2, int32_t a3) {
    if (this->m_hoverFrame) {
        this->m_hoverFrame->Hide();
    }

    if (!this->m_highlightLocked) {
        this->m_drawLayerEnabled &= ~0x10;
        this->NotifyDrawLayerChanged(4);
    }

    if (a3) {
        this->m_isMouseDown = 0;
        this->m_isMouseDragging = 0;
    }

    this->RunOnLeaveScript(a2);
}

// OFFSET: %0x00514160
void CSimpleFrame::LockHighlight(int32_t lock) {
    if (lock == this->m_highlightLocked) {
        return;
    }

    this->m_highlightLocked = lock;

    if (lock) {
        this->m_drawLayerEnabled |= 0x10;
        this->NotifyDrawLayerChanged(4);
    } else if (this->m_simpleTop->m_mouseFocus != this) {
        this->m_drawLayerEnabled &= ~0x10;
        this->NotifyDrawLayerChanged(4);
    }
}

// OFFSET: %0x00514910
void CSimpleFrame::ParentFrame(CSimpleFrame* child) {
    SIMPLEFRAMENODE* node = this->m_children.NewNode(2, 0, 0);
    node->m_frame = child;
}

// OFFSET: %0x005144F0
void CSimpleFrame::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onEvent, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onLoad, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onSizeChanged, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onShow, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onHide, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onCursorEnter, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onCursorExit, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMouseDown, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMouseUp, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onMouseWheel, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onDragStart, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onDragStop, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onReceiveDrag, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onChar, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onKeyDown, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onKeyUp, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onAttributeChanged, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onEnable, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onDisable, time, count);

    if (recurse) {
        for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
            node->m_frame->GetScriptTime(L, time, count, 1);
        }
    }
}

// OFFSET: %0x005146D0
void CSimpleFrame::SetBeingScrolled(int32_t scrolled, int32_t a3) {
    uint32_t visible = this->m_visible;

    if (visible) {
        this->HideThis();
    }

    if (scrolled) {
        this->m_frameFlags |= 0x2000;
    } else {
        this->m_frameFlags &= ~0x2000u;
    }

    if (a3 >= 0) {
        if (a3) {
            this->m_frameFlags |= 0x4000;
        } else {
            this->m_frameFlags &= ~0x4000u;
        }
    }

    if (scrolled) {
        this->m_batchDirty |= 0x1F;
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        if (!(node->m_frame->m_frameFlags & 0x4000)) {
            node->m_frame->SetBeingScrolled(scrolled, -1);
        }
    }

    if (visible) {
        this->ShowThis(false);
    }
}

// OFFSET: %0x005147A0
void CSimpleFrame::SetFrameStrata(FRAME_STRATA strata) {
    if (strata == this->m_strata) {
        return;
    }

    if (this->m_visible) {
        this->m_simpleTop->HideFrame(this, 1);
    }

    this->m_strata = strata;

    if (this->m_visible) {
        this->m_simpleTop->ShowFrame(this, 1);
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->SetFrameStrata(strata);
    }
}

// OFFSET: %0x00514830
void CSimpleFrame::SetFrameLevel(int32_t level, int32_t shiftChildren) {
    if (level < 0) {
        level = 0;
    }

    if ((this->m_frameFlags & 0x100000) && this->m_parent) {
        level = this->m_parent->m_level;
    }

    if (level == this->m_level) {
        return;
    }

    int32_t delta = level - this->m_level;

    if (this->m_visible) {
        this->m_simpleTop->HideFrame(this, 1);
    }

    if (delta > 128) {
        delta = 128;
    }

    this->m_level += delta;

    if (this->m_visible) {
        this->m_simpleTop->ShowFrame(this, 1);
    }

    if (shiftChildren) {
        for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
            CSimpleFrame* child = node->m_frame;

            if (child->m_strata == this->m_strata) {
                child->SetFrameLevel(delta + child->m_level, 1);
            }
        }
    }
}

// OFFSET: %0x005149B0
void CSimpleFrame::UnparentFrame(CSimpleFrame* child) {
    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        if (node->m_frame == child) {
            this->m_children.DeleteNode(node);
            break;
        }
    }
}

// OFFSET: %0x00515A20
void CSimpleFrame::SetParent(CSimpleFrame* parent) {
    if (parent == this->m_parent) {
        return;
    }

    uint32_t visible = this->m_visible;
    uint32_t shown = this->m_shown;

    if (this->m_parent) {
        this->OnProtectedDetach(this->m_parent);
        this->m_parent->UnparentFrame(this);
    }

    if (visible) {
        this->HideThis();
    }

    this->m_parent = parent;

    if (parent) {
        this->SetFrameStrata(static_cast<FRAME_STRATA>(parent->m_strata));
        this->SetFrameLevel(this->m_parent->m_level + 1, 1);
        this->UpdateScale(false);

        uint8_t alpha = this->m_parent->m_alpha * this->m_parent->m_parentAlpha / 255;

        if (!this->m_forceAlpha && alpha != this->m_parentAlpha) {
            this->m_parentAlpha = alpha;
            this->UpdateAlpha();
        }

        this->SetBeingScrolled((this->m_parent->m_frameFlags >> 13) & 1, -1);
        this->SetDontReturnDepth((this->m_parent->m_frameFlags >> 18) & 1);
    } else {
        this->SetFrameStrata(FRAME_STRATA_MEDIUM);
        this->SetFrameLevel(0, 1);
        this->UpdateScale(false);

        if (!this->m_forceAlpha && this->m_parentAlpha != 0xFF) {
            this->m_parentAlpha = 0xFF;
            this->UpdateAlpha();
        }

        this->SetBeingScrolled(0, -1);
    }

    if (this->m_parent) {
        this->OnProtectedAttach(this->m_parent);
        this->m_parent->ParentFrame(this);
    }

    if (shown && (!this->m_parent || this->m_parent->m_visible)) {
        this->ShowThis(false);
    }
}

// OFFSET: %0x00514A00
int32_t CSimpleFrame::AreChildrenProtected(int32_t& deferred) {
    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        if (node->m_frame->m_frameFlags & 0x800) {
            deferred = 1;
        } else if (node->m_frame->IsProtected()) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: %0x00514A70
bool CSimpleFrame::GetBoundsRect(CRect& rect) {
    CRect frameRect = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (this->IsResizePending()) {
        this->Resize(1);
    }

    if (this->GetRect(&frameRect)) {
        rect.minX = frameRect.minX >= rect.minX ? rect.minX : frameRect.minX;
        rect.maxX = frameRect.maxX <= rect.maxX ? rect.maxX : frameRect.maxX;
        rect.minY = frameRect.minY >= rect.minY ? rect.minY : frameRect.minY;
        rect.maxY = frameRect.maxY <= rect.maxY ? rect.maxY : frameRect.maxY;
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        if (!region->m_shown) {
            continue;
        }

        if (region->IsResizePending()) {
            region->Resize(1);
        }

        if (region->GetRect(&frameRect)) {
            rect.minX = frameRect.minX >= rect.minX ? rect.minX : frameRect.minX;
            rect.maxX = frameRect.maxX <= rect.maxX ? rect.maxX : frameRect.maxX;
            rect.minY = frameRect.minY >= rect.minY ? rect.minY : frameRect.minY;
            rect.maxY = frameRect.maxY <= rect.maxY ? rect.maxY : frameRect.maxY;
        }
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        if (node->m_frame->m_shown) {
            node->m_frame->GetBoundsRect(rect);
        }
    }

    return rect.maxY > rect.minY && rect.maxX > rect.minX;
}

// OFFSET: %0x00514C50
bool CSimpleFrame::UpdateScale(bool force) {
    float scale = this->m_frameScale;

    if (this->m_parent) {
        scale *= this->m_parent->m_layoutScale;
    }

    if ((!force && fabs(scale - this->m_layoutScale) < 0.00000023841858f) || scale == 0.0f) {
        return false;
    }

    this->CLayoutFrame::SetLayoutScale(scale, force);

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->SetLayoutScale(scale, force);
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->UpdateScale(false);
    }

    return true;
}

// OFFSET: %0x00514D50
void CSimpleFrame::UpdateAlpha() {
    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->OnColorChanged(false);
    }

    uint8_t alpha = this->m_alpha * this->m_parentAlpha / 255;

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        CSimpleFrame* child = node->m_frame;

        if (!child->m_forceAlpha && alpha != child->m_parentAlpha) {
            child->m_parentAlpha = alpha;
            child->UpdateAlpha();
        }
    }
}

// OFFSET: %0x00514E00
bool CSimpleFrame::UpdateDepth(bool force) {
    float depth = this->m_parentDepth + this->m_depth;

    if (!force && fabs(depth - this->m_layoutDepth) < 0.00000023841858f) {
        return false;
    }

    this->CLayoutFrame::SetLayoutDepth(depth, force);

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->SetLayoutDepth(depth, force);
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        CSimpleFrame* child = node->m_frame;

        if (force || fabs(depth - child->m_parentDepth) >= 0.00000023841858f) {
            child->m_parentDepth = depth;
            child->UpdateDepth(force);
        }
    }

    return true;
}

// OFFSET: %0x00515080
void CSimpleFrame::HideThis() {
    if (!this->m_visible) {
        return;
    }

    if (!this->m_isLoading) {
        this->SetDeferredResize(1);

        if (this->m_titleRegion) {
            this->m_titleRegion->SetDeferredResize(1);
        }
    }

    this->m_visible = 0;
    this->m_simpleTop->HideFrame(this, 0);

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->HideThis();
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->HideThis();
    }

    this->OnLayerHide();
}

// OFFSET: %0x00515140
void CSimpleFrame::ShowThis(bool loadPending) {
    if (!this->m_shown || (this->m_parent && !this->m_parent->m_visible) || this->m_visible) {
        return;
    }

    if (!this->m_isLoading) {
        this->SetDeferredResize(0);

        if (this->m_titleRegion) {
            this->m_titleRegion->SetDeferredResize(0);
        }
    }

    this->m_visible = 1;
    this->m_loadPending = loadPending;
    this->m_simpleTop->ShowFrame(this, 0);

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->ShowThis(loadPending);
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->ShowThis(loadPending);
    }

    if (this->m_frameFlags & 0x1) {
        this->m_simpleTop->RaiseFrame(this, 1);
    }

    this->OnLayerShow();
}

// OFFSET: %0x00515300
void CSimpleFrame::StopAnimating() {
    CScriptRegion::StopAnimating();

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->StopAnimating();
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->StopAnimating();
    }
}

// OFFSET: %0x00515380
void CSimpleFrame::AnimActivated(CSimpleAnimGroup* group, int32_t first, int32_t hasAlpha) {
    if (first && (!this->m_playingAnimGroups || !this->m_playingAnimGroups->Head())) {
        this->AnimData_Init();
    }

    if (!this->m_animAlpha && hasAlpha) {
        this->m_animAlpha = 1;
        this->m_animSavedAlpha = this->m_alpha * this->m_parentAlpha / 255;
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->AnimActivated(group, first, hasAlpha);
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->AnimActivated(group, first, hasAlpha);
    }
}

// OFFSET: %0x00515450
void CSimpleFrame::AnimDeactivated(CSimpleAnimGroup* group, int32_t last) {
    if (this->m_animAlpha) {
        this->m_animAlpha = this->HasActiveAnimOfType(CSimpleAlphaAnim::GetObjectType());

        if (!this->m_animAlpha && this->m_animSavedAlpha != this->m_alpha) {
            this->m_alpha = this->m_animSavedAlpha;
            this->UpdateAlpha();
        }
    }

    if (last && (!this->m_playingAnimGroups || !this->m_playingAnimGroups->Head())) {
        this->AnimData_ResetPositionData();
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->AnimDeactivated(group, last);
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->AnimDeactivated(group, last);
    }
}

// OFFSET: %0x00515550
void CSimpleFrame::AddAnimTranslation(CScriptRegion* region, const C2Vector& translation) {
    if (!region || region == this) {
        region = NULL;
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->AddAnimTranslation(region, translation);
    }

    for (CSimpleRegion* child = this->m_regions.Head(); child; child = this->m_regions.Next(child)) {
        child->AddAnimTranslation(region, translation);
    }
}

// OFFSET: %0x005155E0
void CSimpleFrame::AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians) {
    if (!region || region == this) {
        region = NULL;
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->AddAnimRotation(region, origin, offset, radians);
    }

    for (CSimpleRegion* child = this->m_regions.Head(); child; child = this->m_regions.Next(child)) {
        child->AddAnimRotation(region, origin, offset, radians);
    }
}

// OFFSET: %0x00515690
void CSimpleFrame::AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale) {
    if (!region || region == this) {
        region = NULL;
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->AddAnimScale(region, origin, offset, scale);
    }

    for (CSimpleRegion* child = this->m_regions.Head(); child; child = this->m_regions.Next(child)) {
        child->AddAnimScale(region, origin, offset, scale);
    }
}

// OFFSET: %0x00515820
void CSimpleFrame::OnFrameRender() {
    uint32_t dirty = this->m_batchDirty;
    this->m_batchDirty = 0;

    if (dirty) {
        for (uint32_t layer = 0; layer < 5; layer++) {
            if (!this->m_renderBatch[layer]) {
                void* mem = SMemAlloc(sizeof(CRenderBatch), __FILE__, __LINE__, 0);
                this->m_renderBatch[layer] = mem ? new (mem) CRenderBatch() : NULL;
            }

            CRenderBatch* batch = this->m_renderBatch[layer];

            if (this->m_renderList.IsLinked(batch)) {
                this->m_renderList.UnlinkNode(batch);
            }

            if ((1 << layer) & dirty) {
                batch->Clear();
                this->OnFrameRender(batch, layer);
                batch->Finish();
            }

            if (batch->m_count) {
                this->m_renderList.LinkToTail(batch);
            }
        }

        if (this->m_batchDirty & 0x20) {
            for (uint32_t layer = 0; layer < 5; layer++) {
                this->m_batchDirty |= 1 << layer;
            }
        }
    }

    for (uint32_t layer = 0; layer < 5; layer++) {
        if (this->m_renderBatch[layer]) {
            CSimpleRender::DrawBatch(this->m_renderBatch[layer]);
        }
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        CSimpleFrame* child = node->m_frame;

        if (child->m_visible && !(child->m_frameFlags & 0x4000)) {
            child->OnFrameRender();
        }
    }
}

// OFFSET: %0x005166F0
bool CSimpleFrame::GetAttribute(const char* name, int32_t& luaRef) {
    FRAMEATTR* attr = this->m_attributes.Ptr(name);

    if (!attr || attr->m_luaRef == -1) {
        return false;
    }

    luaRef = attr->m_luaRef;
    return true;
}

// OFFSET: %0x00516A80
void CSimpleFrame::LoadXML_Attributes(linkObject* node, CStatus* status) {
    lua_State* L = FrameScript_GetContext();

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element != XML_ATTRIBUTE) {
            const char* frameName = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Unknown attributes element %s", frameName ? frameName : "<unnamed>", child->m_type.m_string);
            continue;
        }

        if (!(child->m_flags & 0x1000000)) {
            const char* frameName = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: unnamed attribute element", frameName ? frameName : "<unnamed>");
            continue;
        }

        Attribute* attribute = static_cast<Attribute*>(child);
        const char* name = child->m_name.m_string;
        const char* type = "string";

        if (attribute->m_attributeFlags & 2) {
            type = attribute->m_attributeType.m_string;
        }

        if (!(attribute->m_attributeFlags & 1) && SStrCmpI(type, "nil", STORM_MAX_STR)) {
            const char* frameName = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: attribute element named %s missing value", frameName ? frameName : "<unnamed>", name);
            continue;
        }

        const char* value = attribute->m_value.m_string;

        if (!SStrCmpI(type, "nil", STORM_MAX_STR)) {
            lua_pushnil(L);
        } else if (!SStrCmpI(type, "boolean", STORM_MAX_STR)) {
            lua_pushboolean(L, StringToBOOL(value));
        } else if (!SStrCmpI(type, "number", STORM_MAX_STR)) {
            lua_pushnumber(L, SStrToFloat(value));
        } else {
            lua_pushstring(L, value);
        }

        FRAMEATTR* attr = this->m_attributes.Ptr(name);

        if (attr) {
            luaL_unref(L, LUA_REGISTRYINDEX, attr->m_luaRef);
        } else {
            attr = this->m_attributes.New(name, 0, 0);
        }

        const char* savedTaint = lua_tainted;

        if (++lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = NULL;
        }

        lua_settaint(L, -1, NULL);
        attr->m_luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

        if (lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = savedTaint;
        }

        if (--lua_taintexpected <= 0) {
            lua_taintexpected = 0;
        }
    }
}

// OFFSET: %0x00516D00
void CSimpleFrame::SetAttribute(const char* name, int32_t luaRef) {
    FRAMEATTR* attr = this->m_attributes.Ptr(name);

    if (!attr) {
        attr = this->m_attributes.New(name, 0, 0);
    }

    attr->m_luaRef = luaRef;
    this->RunOnAttributeChangedScript(name, luaRef);
}

// OFFSET: %0x00516D90
void CSimpleFrame::LoadXML(linkObject* node, CStatus* status) {
    if (!(node->m_flags & 0x8)) {
        return;
    }

    if (node->m_flags & 0x2000000) {
        const char* inherits = node->m_inherits.m_string;
        char name[1024];

        do {
            SStrTokenize(&inherits, name, sizeof(name), " ,", NULL);

            if (!name[0]) {
                break;
            }

            XMLHashNodeRef ref;

            if (!ref.Acquire(name)) {
                status->Add(STATUS_WARNING, "Couldn't find inherited node: %s", ref.m_name);
            } else if (ref.m_locked) {
                status->Add(STATUS_WARNING, "Recursively inherited node: %s", ref.m_name);
            } else {
                FrameScript_GetContext();

                const char* savedTaint = lua_tainted;

                if (++lua_taintexpected && !lua_taintedclosure) {
                    lua_tainted = ref.m_taint;
                }

                const char* savedTag = lua_gctag;
                lua_gctag = ref.m_taint;

                this->LoadXML(ref.m_node, status);

                lua_gctag = savedTag;

                if (lua_taintexpected && !lua_taintedclosure) {
                    lua_tainted = savedTaint;
                }

                if (--lua_taintexpected <= 0) {
                    lua_taintexpected = 0;
                }
            }
        } while (name[0]);
    }

    CScriptRegion::LoadXML(node, status);

    Frame* frame = static_cast<Frame*>(node);

    if (frame->m_regionFlags & 2) {
        if (frame->m_regionFlags & 8) {
            this->Hide();
        } else {
            this->Show();
        }
    }

    if (frame->m_frameFlags & 0x400) {
        this->SetFrameFlag(0x1, frame->m_frameFlags & 0x800);
    }

    if (frame->m_frameFlags & 0x1000) {
        this->SetFrameFlag(0x100, frame->m_frameFlags & 0x2000);
    }

    if (frame->m_frameFlags & 0x4000) {
        this->SetFrameFlag(0x200, frame->m_frameFlags & 0x8000);
    }

    if (frame->m_frameFlags & 0x10000) {
        this->SetFrameStrata(static_cast<FRAME_STRATA>(frame->m_strata));
    }

    if (frame->m_frameFlags & 0x1) {
        if (frame->m_frameLevel > 0) {
            this->SetFrameLevel(frame->m_frameLevel, 1);
        } else {
            const char* frameName = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Unknown frame level: %d", frameName ? frameName : "<unnamed>", frame->m_frameLevel);
        }
    }

    if (frame->m_regionFlags & 1) {
        float value = frame->m_alpha;

        if (value < 1.0f && value <= 0.0f) {
            value = 0.0f;
        } else if (value >= 1.0f) {
            value = 1.0f;
        }

        this->SetFrameAlpha(static_cast<uint8_t>(value * 255.0f));
    }

    if ((frame->m_frameFlags & 0x20000) && (frame->m_frameFlags & 0x40000)) {
        this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    }

    if ((frame->m_frameFlags & 0x80000) && (frame->m_frameFlags & 0x100000)) {
        this->EnableEvent(SIMPLE_EVENT_KEY, -1);
        this->EnableEvent(SIMPLE_EVENT_CHAR, -1);
    }

    if (frame->m_frameFlags & 0x2) {
        this->SetClampedToScreen((frame->m_frameFlags >> 2) & 1);
    }

    if ((frame->m_frameFlags & 0x8) && (frame->m_frameFlags & 0x10)) {
        this->SetProtected();
    }

    if (frame->m_frameFlags & 0x200) {
        this->SetFrameDepth(frame->m_depth, false);
    }

    if (frame->m_frameFlags & 0x2000000) {
        this->SetUseParentLevel((frame->m_frameFlags >> 26) & 1);
    }

    if (frame->m_frameFlags & 0x8000000) {
        this->SetFrameFlag(0x80000, frame->m_frameFlags & 0x10000000);
    }

    if (frame->m_frameFlags & 0x20000000) {
        this->SetFrameFlag(0x200000, frame->m_frameFlags & 0x40000000);
    }

    if (frame->m_frameFlags & 0x80000000) {
        this->SetForceAlpha(frame->m_frameFlags2 & 1);
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_ATTRIBUTES:
                this->LoadXML_Attributes(child, status);
                break;

            case XML_TITLEREGION: {
                void* mem = SMemAlloc(sizeof(CSimpleTitleRegion), __FILE__, __LINE__, 0);

                if (mem) {
                    CSimpleTitleRegion* titleRegion = new (mem) CSimpleTitleRegion;
                    titleRegion->SetParent(this);
                    titleRegion->LoadXML(child, status);
                    this->SetTitleRegion(titleRegion);
                }

                break;
            }

            case XML_RESIZEBOUNDS: {
                float minWidth = 0.0f;
                float minHeight = 0.0f;
                float maxWidth = 0.0f;
                float maxHeight = 0.0f;

                linkObject* minResize = child->GetChild(XML_MINRESIZE);

                if (minResize) {
                    LoadXML_Dimensions(minResize, minWidth, minHeight, status);
                }

                linkObject* maxResize = child->GetChild(XML_MAXRESIZE);

                if (maxResize) {
                    LoadXML_Dimensions(maxResize, maxWidth, maxHeight, status);
                }

                this->m_resizeRect.minY = minHeight;
                this->m_resizeRect.minX = minWidth;
                this->m_resizeRect.maxY = maxHeight;
                this->m_resizeRect.maxX = maxWidth;
                break;
            }

            case XML_BACKDROP: {
                void* mem = SMemAlloc(sizeof(CBackdropGenerator), __FILE__, __LINE__, 0);

                if (mem) {
                    CBackdropGenerator* backdrop = new (mem) CBackdropGenerator();

                    if (backdrop) {
                        backdrop->LoadXML(child, status);
                        this->SetBackdrop(backdrop);
                    }
                }

                break;
            }

            case XML_HITRECTINSETS: {
                float left;
                float right;
                float top;
                float bottom;

                if (LoadXML_Insets(child, left, right, top, bottom, status)) {
                    this->SetHitRectInsets(left, right, top, bottom);
                }

                break;
            }

            case XML_LAYERS:
                this->LoadXML_Layers(child, status);
                break;

            case XML_SCRIPTS:
                this->LoadXML_Scripts(child, status);
                break;

            case XML_FRAMES:
                for (linkObject* frameNode = child->m_child; frameNode; frameNode = frameNode->m_next) {
                    FrameXML_CreateFrame(frameNode, this, status);
                }

                break;
        }
    }
}

// PROVEN: 17362775 IDENTICAL/DIFFERENT
// OFFSET: 0x00502830/0x140147970
void CSimpleFrame::SetTitleRegion(CScriptRegion* region) {
    this->m_titleRegion = region;

    if (region) {
        region->SetDeferredResize(this->m_layoutFlags & 2);
    }
}

// OFFSET: %0x00512DA0
void CSimpleFrame::SetForceAlpha(bool force) {
    if (force) {
        if (!this->m_forceAlpha && this->m_parentAlpha != 0xFF) {
            this->m_parentAlpha = 0xFF;
            this->UpdateAlpha();
        }

        this->m_forceAlpha = 1;
    } else {
        this->m_forceAlpha = 0;

        if (this->m_parent) {
            uint8_t alpha = this->m_parent->m_alpha * this->m_parent->m_parentAlpha / 255;

            if (!this->m_forceAlpha && alpha != this->m_parentAlpha) {
                this->m_parentAlpha = alpha;
                this->UpdateAlpha();
            }
        }
    }
}

// OFFSET: %0x005159D0
void CSimpleFrame::SetUseParentLevel(bool use) {
    if (use) {
        if (this->m_parent) {
            this->SetFrameLevel(this->m_parent->m_level, 1);
        }

        this->m_frameFlags |= 0x100000;
    } else {
        this->m_frameFlags &= ~0x100000;
    }
}

// OFFSET: %0x00514F00
void CSimpleFrame::SetDontReturnDepth(int32_t dontReturn) {
    if (dontReturn) {
        this->m_frameFlags |= 0x40000;
    } else {
        this->m_frameFlags &= ~0x40000;
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->SetDontReturnDepth(dontReturn);
    }
}

// OFFSET: %0x00514F50
void CSimpleFrame::SignalLoaded() {
    this->m_loadPending = 0;

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->SignalLoaded();
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->SignalLoaded();
    }
}

// OFFSET: %0x00514FC0
void CSimpleFrame::MarkDirty() {
    for (uint32_t layer = 0; layer < 5; layer++) {
        if (this->m_drawLayerEnabled & (1 << layer)) {
            if (this->m_frameFlags & 0x2000) {
                this->m_batchDirty |= 1 << layer;
            } else if (this->m_simpleTop && this->m_visible) {
                this->m_simpleTop->NotifyFrameLayerChanged(this, layer);
            }

            this->NotifyScrollParent();
        }
    }

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->MarkDirty();
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->MarkDirty();
    }
}

// OFFSET: %0x00477F60
int32_t CSimpleFrame::IsObjectLoaded() {
    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        if (!region->IsObjectLoaded()) {
            return 0;
        }
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        if (!node->m_frame->IsObjectLoaded()) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: %0x00477E90
void CSimpleFrame::DumpInfo(int32_t depth, void (*print)(char*, void*), void* param) {
    char buffer[128];

    if (depth) {
        memset(buffer, '.', depth);
    }

    SStrPrintf(&buffer[depth], sizeof(buffer) - depth, "Frame %p\n", this);
    print(buffer, param);

    for (CSimpleRegion* region = this->m_regions.Head(); region; region = this->m_regions.Next(region)) {
        region->DumpInfo(depth + 1, print, param);
    }

    for (SIMPLEFRAMENODE* node = this->m_children.Head(); node; node = this->m_children.Next(node)) {
        node->m_frame->DumpInfo(depth + 1, print, param);
    }
}
