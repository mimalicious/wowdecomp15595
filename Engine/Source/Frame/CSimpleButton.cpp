#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00BB9130
int32_t CSimpleButton::s_metatable = LUA_NOREF;

// STATIC: %0x00CD2520
int32_t CSimpleButton::s_objectType;

// STATIC: %0x00BB86A0
const char* CSimpleButton::s_objectTypeName = "Button";

// STATIC: %0x00CD2524
CDataAllocator CSimpleButton::s_allocator(sizeof(CSimpleButton), 100);

// INLINED
void CSimpleButton::operator delete(void* ptr) {
    CSimpleButton::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: %0x00524A90
bool CSimpleButton::IsA(int32_t type) {
    return type == CSimpleButton::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: %0x00524AD0
bool CSimpleButton::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleButton::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: %0x00524920
void CSimpleButton::LockHighlight(int32_t lock) {
    CSimpleFrame::LockHighlight(lock);
    this->UpdateTextState(this->m_state);
}

// OFFSET: %0x00524530
void CSimpleButton::SetMotionScriptsWhileDisabled(int32_t enabled) {
    this->m_motionScriptsWhileDisabled = enabled;
}

// OFFSET: %0x00524550
int32_t CSimpleButton::GetMotionScriptsWhileDisabled() {
    return this->m_motionScriptsWhileDisabled;
}

// OFFSET: %0x00523EC0
uint64_t MouseButtonStringToBitmask(const char* string) {
    if (!string || !*string) {
        return 0;
    }

    if (!SStrCmpI(string, "LeftButtonDown", STORM_MAX_STR)) {
        return 0x1ULL;
    }

    if (!SStrCmpI(string, "LeftButtonUp", STORM_MAX_STR)) {
        return 0x80000000ULL;
    }

    if (!SStrCmpI(string, "MiddleButtonDown", STORM_MAX_STR)) {
        return 0x2ULL;
    }

    if (!SStrCmpI(string, "MiddleButtonUp", STORM_MAX_STR)) {
        return 0x100000000ULL;
    }

    if (!SStrCmpI(string, "RightButtonDown", STORM_MAX_STR)) {
        return 0x4ULL;
    }

    if (!SStrCmpI(string, "RightButtonUp", STORM_MAX_STR)) {
        return 0x200000000ULL;
    }

    if (!SStrCmpI(string, "Button4Down", STORM_MAX_STR)) {
        return 0x8ULL;
    }

    if (!SStrCmpI(string, "Button4Up", STORM_MAX_STR)) {
        return 0x400000000ULL;
    }

    if (!SStrCmpI(string, "Button5Down", STORM_MAX_STR)) {
        return 0x10ULL;
    }

    if (!SStrCmpI(string, "Button5Up", STORM_MAX_STR)) {
        return 0x800000000ULL;
    }

    if (!SStrCmpI(string, "Button6Down", STORM_MAX_STR)) {
        return 0x20ULL;
    }

    if (!SStrCmpI(string, "Button6Up", STORM_MAX_STR)) {
        return 0x1000000000ULL;
    }

    if (!SStrCmpI(string, "Button7Down", STORM_MAX_STR)) {
        return 0x40ULL;
    }

    if (!SStrCmpI(string, "Button7Up", STORM_MAX_STR)) {
        return 0x2000000000ULL;
    }

    if (!SStrCmpI(string, "Button8Down", STORM_MAX_STR)) {
        return 0x80ULL;
    }

    if (!SStrCmpI(string, "Button8Up", STORM_MAX_STR)) {
        return 0x4000000000ULL;
    }

    if (!SStrCmpI(string, "Button9Down", STORM_MAX_STR)) {
        return 0x100ULL;
    }

    if (!SStrCmpI(string, "Button9Up", STORM_MAX_STR)) {
        return 0x8000000000ULL;
    }

    if (!SStrCmpI(string, "Button10Down", STORM_MAX_STR)) {
        return 0x200ULL;
    }

    if (!SStrCmpI(string, "Button10Up", STORM_MAX_STR)) {
        return 0x10000000000ULL;
    }

    if (!SStrCmpI(string, "Button11Down", STORM_MAX_STR)) {
        return 0x400ULL;
    }

    if (!SStrCmpI(string, "Button11Up", STORM_MAX_STR)) {
        return 0x20000000000ULL;
    }

    if (!SStrCmpI(string, "Button12Up", STORM_MAX_STR)) {
        return 0x40000000000ULL;
    }

    if (!SStrCmpI(string, "Button12Down", STORM_MAX_STR)) {
        return 0x800ULL;
    }

    if (!SStrCmpI(string, "Button13Up", STORM_MAX_STR)) {
        return 0x80000000000ULL;
    }

    if (!SStrCmpI(string, "Button13Down", STORM_MAX_STR)) {
        return 0x1000ULL;
    }

    if (!SStrCmpI(string, "Button14Up", STORM_MAX_STR)) {
        return 0x100000000000ULL;
    }

    if (!SStrCmpI(string, "Button14Down", STORM_MAX_STR)) {
        return 0x2000ULL;
    }

    if (!SStrCmpI(string, "Button15Up", STORM_MAX_STR)) {
        return 0x200000000000ULL;
    }

    if (!SStrCmpI(string, "Button15Down", STORM_MAX_STR)) {
        return 0x4000ULL;
    }

    if (!SStrCmpI(string, "Button16Up", STORM_MAX_STR)) {
        return 0x400000000000ULL;
    }

    if (!SStrCmpI(string, "Button16Down", STORM_MAX_STR)) {
        return 0x8000ULL;
    }

    if (!SStrCmpI(string, "Button17Up", STORM_MAX_STR)) {
        return 0x800000000000ULL;
    }

    if (!SStrCmpI(string, "Button17Down", STORM_MAX_STR)) {
        return 0x10000ULL;
    }

    if (!SStrCmpI(string, "Button18Up", STORM_MAX_STR)) {
        return 0x1000000000000ULL;
    }

    if (!SStrCmpI(string, "Button18Down", STORM_MAX_STR)) {
        return 0x20000ULL;
    }

    if (!SStrCmpI(string, "Button19Up", STORM_MAX_STR)) {
        return 0x2000000000000ULL;
    }

    if (!SStrCmpI(string, "Button19Down", STORM_MAX_STR)) {
        return 0x40000ULL;
    }

    if (!SStrCmpI(string, "Button20Down", STORM_MAX_STR)) {
        return 0x80000ULL;
    }

    if (!SStrCmpI(string, "Button20Up", STORM_MAX_STR)) {
        return 0x4000000000000ULL;
    }

    if (!SStrCmpI(string, "Button21Down", STORM_MAX_STR)) {
        return 0x100000ULL;
    }

    if (!SStrCmpI(string, "Button21Up", STORM_MAX_STR)) {
        return 0x8000000000000ULL;
    }

    if (!SStrCmpI(string, "Button22Up", STORM_MAX_STR)) {
        return 0x10000000000000ULL;
    }

    if (!SStrCmpI(string, "Button22Down", STORM_MAX_STR)) {
        return 0x200000ULL;
    }

    if (!SStrCmpI(string, "Button23Up", STORM_MAX_STR)) {
        return 0x20000000000000ULL;
    }

    if (!SStrCmpI(string, "Button23Down", STORM_MAX_STR)) {
        return 0x400000ULL;
    }

    if (!SStrCmpI(string, "Button24Up", STORM_MAX_STR)) {
        return 0x40000000000000ULL;
    }

    if (!SStrCmpI(string, "Button24Down", STORM_MAX_STR)) {
        return 0x800000ULL;
    }

    if (!SStrCmpI(string, "Button25Up", STORM_MAX_STR)) {
        return 0x80000000000000ULL;
    }

    if (!SStrCmpI(string, "Button25Down", STORM_MAX_STR)) {
        return 0x1000000ULL;
    }

    if (!SStrCmpI(string, "Button26Up", STORM_MAX_STR)) {
        return 0x100000000000000ULL;
    }

    if (!SStrCmpI(string, "Button26Down", STORM_MAX_STR)) {
        return 0x2000000ULL;
    }

    if (!SStrCmpI(string, "Button27Up", STORM_MAX_STR)) {
        return 0x200000000000000ULL;
    }

    if (!SStrCmpI(string, "Button27Down", STORM_MAX_STR)) {
        return 0x4000000ULL;
    }

    if (!SStrCmpI(string, "Button28Up", STORM_MAX_STR)) {
        return 0x400000000000000ULL;
    }

    if (!SStrCmpI(string, "Button28Down", STORM_MAX_STR)) {
        return 0x8000000ULL;
    }

    if (!SStrCmpI(string, "Button29Up", STORM_MAX_STR)) {
        return 0x800000000000000ULL;
    }

    if (!SStrCmpI(string, "Button29Down", STORM_MAX_STR)) {
        return 0x10000000ULL;
    }

    if (!SStrCmpI(string, "Button30Down", STORM_MAX_STR)) {
        return 0x20000000ULL;
    }

    if (!SStrCmpI(string, "Button30Up", STORM_MAX_STR)) {
        return 0x1000000000000000ULL;
    }

    if (!SStrCmpI(string, "Button31Down", STORM_MAX_STR)) {
        return 0x40000000ULL;
    }

    if (!SStrCmpI(string, "Button31Up", STORM_MAX_STR)) {
        return 0x2000000000000000ULL;
    }

    if (!SStrCmpI(string, "AnyDown", STORM_MAX_STR)) {
        return 0x7FFFFFFFULL;
    }

    if (!SStrCmpI(string, "AnyUp", STORM_MAX_STR)) {
        return 0xFFFFFFFF80000000ULL;
    }

    return 0;
}

// OFFSET: %0x00524950
void CSimpleButton::RunOnClickScript(const char* btn, int32_t down) {
    if (this->HasScript(this->m_onPreClick)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        lua_pushboolean(L, down);
        this->RunScript(this->m_onPreClick, 2, NULL);
    }

    if (this->HasScript(this->m_onClick)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        lua_pushboolean(L, down);
        this->RunScript(this->m_onClick, 2, NULL);
    }

    if (this->HasScript(this->m_onPostClick)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        lua_pushboolean(L, down);
        this->RunScript(this->m_onPostClick, 2, NULL);
    }
}

// OFFSET: %0x00524A40
void CSimpleButton::RunOnDoubleClickScript(const char* btn) {
    if (this->HasScript(this->m_onDoubleClick)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, btn);
        this->RunScript(this->m_onDoubleClick, 1, NULL);
    }
}

// OFFSET: [%0x00524B10, %0x00524910]
CSimpleButton::~CSimpleButton() {
}

// OFFSET: %0x00524560
const char* CSimpleButton::GetObjectTypeName() {
    return CSimpleButton::s_objectTypeName;
}

// OFFSET: %0x00524570
int32_t CSimpleButton::GetScriptMetaTable() {
    return CSimpleButton::s_metatable;
}

// OFFSET: %0x00524580
FrameScript_Object::ScriptIx* CSimpleButton::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "preclick", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button,down) %s end";
        return &this->m_onPreClick;
    }

    if (!SStrCmp(name, "onclick", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button,down) %s end";
        return &this->m_onClick;
    }

    if (!SStrCmp(name, "postclick", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button,down) %s end";
        return &this->m_onPostClick;
    }

    if (!SStrCmp(name, "ondoubleclick", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,button) %s end";
        return &this->m_onDoubleClick;
    }

    return NULL;
}

// OFFSET: %0x005246C0
void CSimpleButton::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onPreClick, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onClick, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onPostClick, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onDoubleClick, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// OFFSET: %0x00524740
void CSimpleButton::UnregisterRegion(CSimpleRegion* region) {
    if (region == this->m_fontString) {
        this->m_fontString = NULL;
    }

    if (region == this->m_stateTextures[BUTTONSTATE_DISABLED]) {
        this->m_stateTextures[BUTTONSTATE_DISABLED] = NULL;
    }

    if (region == this->m_stateTextures[BUTTONSTATE_NORMAL]) {
        this->m_stateTextures[BUTTONSTATE_NORMAL] = NULL;
    }

    if (region == this->m_stateTextures[BUTTONSTATE_PUSHED]) {
        this->m_stateTextures[BUTTONSTATE_PUSHED] = NULL;
    }

    if (region == this->m_activeTexture) {
        this->m_activeTexture = NULL;
    }

    if (region == this->m_highlightTexture) {
        this->m_highlightTexture = NULL;
    }

    CSimpleFrame::UnregisterRegion(region);
}

// OFFSET: %0x005247B0
void CSimpleButton::SetHighlight(CSimpleTexture* texture, EGxBlend blend) {
    if (texture == this->m_highlightTexture) {
        return;
    }

    if (this->m_highlightTexture) {
        delete this->m_highlightTexture;
    }

    if (texture) {
        texture->SetFrame(this, 4, 0, true);
        texture->SetBlendMode(blend);
    }

    this->m_highlightTexture = texture;
}

// OFFSET: %0x00524800
void CSimpleButton::OnLayerHide() {
    if (this->m_state != BUTTONSTATE_DISABLED && !this->m_stateLocked) {
        this->SetButtonState(BUTTONSTATE_NORMAL, 0);
    }

    CSimpleFrame::OnLayerHide();
}

// OFFSET: %0x00524830
void CSimpleButton::OnDragStart(const CMouseEvent& evt) {
    if (this->m_state != BUTTONSTATE_DISABLED && !this->m_stateLocked) {
        this->SetButtonState(BUTTONSTATE_NORMAL, 0);
    }

    if (this->m_state != BUTTONSTATE_DISABLED) {
        CSimpleFrame::OnDragStart(evt);
    }
}

// OFFSET: %0x00524870
void CSimpleButton::SetClickAction(uint64_t action) {
    this->m_clickAction = action;
}

// OFFSET: %0x00524890
void CSimpleButton::UpdateTextState(CSimpleButtonState state) {
    if (!this->m_fontString) {
        return;
    }

    if (state == BUTTONSTATE_PUSHED) {
        this->m_fontString->SetJustificationOffset(this->m_pushedTextOffset.x, this->m_pushedTextOffset.y);
    } else {
        this->m_fontString->SetJustificationOffset(0.0f, 0.0f);

        if (state == BUTTONSTATE_DISABLED) {
            this->m_fontString->SetFontObject(this->m_disabledFont);
            return;
        }
    }

    if (this->m_drawLayerEnabled & 0x10) {
        this->m_fontString->SetFontObject(this->m_highlightFont);
    } else {
        this->m_fontString->SetFontObject(this->m_normalFont);
    }
}

// OFFSET: %0x00827C60
const char* GetMouseButtonName(uint32_t button) {
    switch (button) {
        case 0x1:
            return "LeftButton";
        case 0x2:
            return "MiddleButton";
        case 0x4:
            return "RightButton";
        case 0x8:
            return "Button4";
        case 0x10:
            return "Button5";
        case 0x20:
            return "Button6";
        case 0x40:
            return "Button7";
        case 0x80:
            return "Button8";
        case 0x100:
            return "Button9";
        case 0x200:
            return "Button10";
        case 0x400:
            return "Button11";
        case 0x800:
            return "Button12";
        case 0x1000:
            return "Button13";
        case 0x2000:
            return "Button14";
        case 0x4000:
            return "Button15";
        case 0x8000:
            return "Button16";
        case 0x10000:
            return "Button17";
        case 0x20000:
            return "Button18";
        case 0x40000:
            return "Button19";
        case 0x80000:
            return "Button20";
        case 0x100000:
            return "Button21";
        case 0x200000:
            return "Button22";
        case 0x400000:
            return "Button23";
        case 0x800000:
            return "Button24";
        case 0x1000000:
            return "Button25";
        case 0x2000000:
            return "Button26";
        case 0x4000000:
            return "Button27";
        case 0x8000000:
            return "Button28";
        case 0x10000000:
            return "Button29";
        case 0x20000000:
            return "Button30";
        case 0x40000000:
            return "Button31";
        default:
            return "UNKNOWN";
    }
}

// OFFSET: %0x00524B50
void CSimpleButton::SetFontString(CSimpleFontString* fontString) {
    if (fontString == this->m_fontString) {
        return;
    }

    if (this->m_fontString) {
        delete this->m_fontString;
    }

    if (fontString) {
        fontString->SetFrame(this, 2, 0, true);

        if (!fontString->HasAnyFramepoints()) {
            uint32_t styleFlags = this->m_normalFont ? this->m_normalFont->m_attributes.m_styleFlags : 0;

            if (styleFlags & 0x1) {
                fontString->SetPoint(FRAMEPOINT_LEFT, this, FRAMEPOINT_LEFT, 0.0f, 0.0f, 1);
            } else if (styleFlags & 0x4) {
                fontString->SetPoint(FRAMEPOINT_RIGHT, this, FRAMEPOINT_RIGHT, 0.0f, 0.0f, 1);
            } else {
                fontString->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_CENTER, 0.0f, 0.0f, 1);
            }
        }
    }

    this->m_fontString = fontString;
    this->UpdateTextState(this->m_state);
}

// OFFSET: %0x00524C00
void CSimpleButton::SetText(const char* text) {
    if ((!text || !*text) && !this->m_fontString) {
        return;
    }

    if (!this->m_fontString) {
        void* data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        this->SetFontString(data ? new (data) CSimpleFontString(this, 2, true) : NULL);
    }

    this->m_fontString->SetText(text, 1);
}

// OFFSET: %0x00524C70
void CSimpleButton::SetNormalFontColor(const CImVector& color) {
    CSimpleFont* font = this->m_normalFont;

    if (!font) {
        return;
    }

    if (*reinterpret_cast<const uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&font->m_attributes.m_color) || !(font->m_attributes.m_flags & 0x400)) {
        font->m_attributes.m_color = color;
        font->m_attributes.m_flags |= 0x404;
    }

    this->m_normalFont->m_fontableFlags &= ~0x4u;
    this->m_normalFont->UpdateObjects();
}

// OFFSET: %0x00524CC0
void CSimpleButton::SetDisabledFontColor(const CImVector& color) {
    CSimpleFont* font = this->m_disabledFont;

    if (!font) {
        return;
    }

    if (*reinterpret_cast<const uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&font->m_attributes.m_color) || !(font->m_attributes.m_flags & 0x400)) {
        font->m_attributes.m_color = color;
        font->m_attributes.m_flags |= 0x404;
    }

    this->m_disabledFont->m_fontableFlags &= ~0x4u;
    this->m_disabledFont->UpdateObjects();
}

// OFFSET: %0x00524D10
void CSimpleButton::SetHighlightFontColor(const CImVector& color) {
    CSimpleFont* font = this->m_highlightFont;

    if (!font) {
        return;
    }

    if (*reinterpret_cast<const uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&font->m_attributes.m_color) || !(font->m_attributes.m_flags & 0x400)) {
        font->m_attributes.m_color = color;
        font->m_attributes.m_flags |= 0x404;
    }

    this->m_highlightFont->m_fontableFlags &= ~0x4u;
    this->m_highlightFont->UpdateObjects();
}

// OFFSET: %0x00524D60
void CSimpleButton::SetStateTexture(CSimpleButtonState state, CSimpleTexture* texture) {
    if (texture == this->m_stateTextures[state]) {
        return;
    }

    if (this->m_activeTexture == this->m_stateTextures[state]) {
        this->m_activeTexture = NULL;
    }

    if (this->m_stateTextures[state]) {
        delete this->m_stateTextures[state];
    }

    if (texture) {
        texture->SetFrame(this, 2, 0, false);
    }

    this->m_stateTextures[state] = texture;

    if (texture && state == this->m_state) {
        this->m_activeTexture = texture;
        texture->Show();
    }
}

// OFFSET: %0x00524DF0
int32_t CSimpleButton::SetHighlight(const char* fileName, EGxBlend blend) {
    CSimpleTexture* texture = this->m_highlightTexture;

    if (texture) {
        texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texture->SetBlendMode(blend);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    texture = data ? new (data) CSimpleTexture(this, 4, 0, true) : NULL;

    if (!texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        if (texture) {
            delete texture;
        }

        return 0;
    }

    texture->SetAllPoints(this, 1);
    texture->SetBlendMode(blend);
    this->m_highlightTexture = texture;
    return 1;
}

// OFFSET: %0x00524EC0
int32_t CSimpleButton::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    int32_t result = CSimpleFrame::OnLayerMouseDown(evt, btn);

    if (result) {
        return result;
    }

    if (!btn) {
        C2Vector pt;
        pt.x = evt.m_data.m_mousePosition.x;
        pt.y = evt.m_data.m_mousePosition.y;

        if (!this->TestHitRect(pt)) {
            return 0;
        }
    }

    if (this->m_state == BUTTONSTATE_DISABLED) {
        return 1;
    }

    uint32_t button = btn ? 0x7FFFFFFF : evt.m_data.m_button;

    if (this->m_clickAction & button) {
        if (!btn) {
            btn = GetMouseButtonName(evt.m_data.m_button);
        }

        this->m_simpleTop->unk_1260 = 1;
        this->OnClick(btn, 1);
        this->m_simpleTop->unk_1260 = 0;
    }

    if (!this->m_stateLocked && this->m_state != BUTTONSTATE_DISABLED) {
        this->SetButtonState(BUTTONSTATE_PUSHED, 0);
    }

    return 1;
}

// OFFSET: %0x00524FB0
int32_t CSimpleButton::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    int32_t result = CSimpleFrame::OnLayerMouseUp(evt, btn);

    if (result) {
        return result;
    }

    if (this->m_state != BUTTONSTATE_PUSHED) {
        return 1;
    }

    uint64_t button = 0;

    if (btn) {
        button = 0xFFFFFFFF80000000ULL;
    } else {
        C2Vector pt;
        pt.x = evt.m_data.m_mousePosition.x;
        pt.y = evt.m_data.m_mousePosition.y;

        if (this->TestHitRect(pt)) {
            button = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(evt.m_data.m_button)) << 31);
        }
    }

    if (this->m_clickAction & button) {
        if (!btn) {
            btn = GetMouseButtonName(evt.m_data.m_button);
        }

        this->m_simpleTop->unk_1260 = 1;
        uint32_t time = OsGetAsyncTimeMs();

        if (this->HasScript(this->m_onDoubleClick) && this->m_lastClickTime && time - this->m_lastClickTime <= 300) {
            this->OnDoubleClick(btn);
            this->m_lastClickTime = 0;
        } else {
            this->OnClick(btn, 0);
            this->m_lastClickTime = time;
        }

        this->m_simpleTop->unk_1260 = 0;
    }

    if (!this->m_stateLocked && this->m_state != BUTTONSTATE_DISABLED) {
        this->SetButtonState(BUTTONSTATE_NORMAL, 0);
    }

    return 1;
}

// OFFSET: %0x00525100
void CSimpleButton::OnLayerCursorEnter(int32_t a2) {
    if (this->m_state != BUTTONSTATE_DISABLED) {
        CSimpleFrame::OnLayerCursorEnter(a2);
        this->UpdateTextState(this->m_state);
        return;
    }

    if (this->GetMotionScriptsWhileDisabled()) {
        if (this->m_hoverFrame) {
            this->m_hoverFrame->Show();
        }

        this->RunOnEnterScript(a2);
    }
}

// OFFSET: %0x00525160
void CSimpleButton::OnLayerCursorExit(int32_t a2, int32_t a3) {
    if (this->m_state != BUTTONSTATE_DISABLED) {
        CSimpleFrame::OnLayerCursorExit(a2, a3);
        this->UpdateTextState(this->m_state);
        return;
    }

    if (this->GetMotionScriptsWhileDisabled()) {
        if (this->m_hoverFrame) {
            this->m_hoverFrame->Hide();
        }

        this->RunOnLeaveScript(a2);
    }
}

// OFFSET: %0x005251D0
void CSimpleButton::OnClick(const char* btn, int32_t down) {
    if (this->m_inClick || this->m_state == BUTTONSTATE_DISABLED) {
        return;
    }

    const char* savedButtonName = this->m_simpleTop->m_mouseButtonName;
    this->m_simpleTop->m_mouseButtonName = btn;
    this->m_inClick = 1;
    this->RunOnClickScript(btn, down);
    this->m_inClick = 0;
    this->m_simpleTop->m_mouseButtonName = savedButtonName;
}

// OFFSET: %0x00525230
void CSimpleButton::OnDoubleClick(const char* btn) {
    if (this->m_inClick || this->m_state == BUTTONSTATE_DISABLED) {
        return;
    }

    const char* savedButtonName = this->m_simpleTop->m_mouseButtonName;
    this->m_simpleTop->m_mouseButtonName = btn;
    this->m_inClick = 1;
    this->RunOnDoubleClickScript(btn);
    this->m_inClick = 0;
    this->m_simpleTop->m_mouseButtonName = savedButtonName;
}

// OFFSET: %0x00525290
void CSimpleButton::SetButtonState(CSimpleButtonState state, int32_t stateLocked) {
    this->m_stateLocked = stateLocked;

    if (state == this->m_state) {
        return;
    }

    if (this->m_activeTexture && (this->m_stateTextures[state] || state == BUTTONSTATE_NORMAL)) {
        this->m_activeTexture->Hide();
        this->m_activeTexture = NULL;
    }

    if (this->m_stateTextures[state]) {
        this->m_activeTexture = this->m_stateTextures[state];
        this->m_activeTexture->Show();
    }

    this->UpdateTextState(state);
    this->m_state = state;
}

// OFFSET: %0x005253A0
void CSimpleButton::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x4)) {
        return;
    }

    CSimpleFontString* fontString = NULL;

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_NORMALTEXTURE:
                this->SetStateTexture(BUTTONSTATE_NORMAL, LoadXML_Texture(child, this, status));
                break;

            case XML_PUSHEDTEXTURE:
                this->SetStateTexture(BUTTONSTATE_PUSHED, LoadXML_Texture(child, this, status));
                break;

            case XML_DISABLEDTEXTURE:
                this->SetStateTexture(BUTTONSTATE_DISABLED, LoadXML_Texture(child, this, status));
                break;

            case XML_HIGHLIGHTTEXTURE: {
                CSimpleTexture* texture = LoadXML_Texture(child, this, status);

                if (texture) {
                    this->SetHighlight(texture, texture->m_alphaMode);
                }

                break;
            }

            case XML_BUTTONTEXT:
                fontString = LoadXML_String(child, this, status);
                break;

            case XML_DISABLEDFONT: {
                FontStyle* style = static_cast<FontStyle*>(child);

                if (style->m_styleFlags & 1) {
                    this->m_disabledFont = CSimpleFont::GetFont(style->m_style.m_string, 0);

                    if (!this->m_disabledFont) {
                        status->Add(STATUS_WARNING, "Couldn't find DISABLED font: %s", style->m_style.m_string);
                    }
                }

                break;
            }

            case XML_NORMALFONT: {
                FontStyle* style = static_cast<FontStyle*>(child);

                if (style->m_styleFlags & 1) {
                    this->m_normalFont = CSimpleFont::GetFont(style->m_style.m_string, 0);

                    if (!this->m_normalFont) {
                        status->Add(STATUS_WARNING, "Couldn't find NORMAL font: %s", style->m_style.m_string);
                    }
                }

                break;
            }

            case XML_HIGHLIGHTFONT: {
                FontStyle* style = static_cast<FontStyle*>(child);

                if (style->m_styleFlags & 1) {
                    this->m_highlightFont = CSimpleFont::GetFont(style->m_style.m_string, 0);

                    if (!this->m_highlightFont) {
                        status->Add(STATUS_WARNING, "Couldn't find HIGHLIGHT font: %s", style->m_style.m_string);
                    }
                }

                break;
            }

            case XML_NORMALCOLOR: {
                CImVector color(0, 0, 0, 0);
                LoadXML_Color(child, color, status);
                this->SetNormalFontColor(color);
                break;
            }

            case XML_DISABLEDCOLOR: {
                CImVector color(0, 0, 0, 0);
                LoadXML_Color(child, color, status);
                this->SetDisabledFontColor(color);
                break;
            }

            case XML_HIGHLIGHTCOLOR: {
                CImVector color(0, 0, 0, 0);
                LoadXML_Color(child, color, status);
                this->SetHighlightFontColor(color);
                break;
            }

            case XML_PUSHEDTEXTOFFSET: {
                float x;
                float y;

                if (LoadXML_Dimensions(child, x, y, status)) {
                    this->m_pushedTextOffset.x = x;
                    this->m_pushedTextOffset.y = y;
                }

                break;
            }
        }
    }

    if (fontString) {
        this->SetFontString(fontString);
    } else {
        this->UpdateTextState(this->m_state);
    }

    Button* button = static_cast<Button*>(node);

    if (button->m_buttonFlags & 0x1) {
        const char* localized = FrameScript_GetText(button->m_text.m_string, -1, GENDER_NOT_APPLICABLE);

        if (!localized || !*localized) {
            localized = button->m_text.m_string;
        }

        this->SetText(localized);
    }

    if (button->m_buttonFlags & 0x2) {
        uint32_t clickAction = 0;
        const char* registerForClicks = button->m_registerForClicks.m_string;
        char token[1024];

        SStrTokenize(&registerForClicks, token, sizeof(token), " ,", NULL);

        while (token[0]) {
            clickAction |= static_cast<uint32_t>(MouseButtonStringToBitmask(token));
            SStrTokenize(&registerForClicks, token, sizeof(token), " ,", NULL);
        }

        this->m_clickAction = clickAction;
    }

    if (button->m_buttonFlags & 0x4) {
        this->SetMotionScriptsWhileDisabled((button->m_buttonFlags >> 3) & 1);
    }
}

// OFFSET: %0x00525740
int32_t CSimpleButton::SetStateTexture(CSimpleButtonState state, const char* fileName) {
    if (this->m_stateTextures[state]) {
        this->m_stateTextures[state]->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
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

    texture->SetAllPoints(this, 1);
    this->SetStateTexture(state, texture);
    return 1;
}

// OFFSET: %0x00525810
void CSimpleButton::Enable(int32_t enabled) {
    if (enabled) {
        if (this->m_state != BUTTONSTATE_DISABLED) {
            return;
        }

        this->SetButtonState(BUTTONSTATE_NORMAL, 0);

        if (this == this->m_simpleTop->m_mouseFocus) {
            this->OnLayerCursorEnter(0);
        }

        if (this->m_highlightLocked) {
            this->EnableDrawLayer(4);
        }
    } else {
        if (this->m_state == BUTTONSTATE_DISABLED) {
            return;
        }

        if (this == this->m_simpleTop->m_mouseFocus) {
            this->OnLayerCursorExit(0, 1);
        }

        this->DisableDrawLayer(4);
        this->SetButtonState(BUTTONSTATE_DISABLED, 0);
    }

    this->RunOnEnableScript(enabled == 0);
}

// OFFSET: %0x00525340
void CSimpleButton::RunOnEnableScript(int32_t disabled) {
    this->SetFrameFlag(0x400, disabled);

    FrameScript_Object::ScriptIx& script = disabled ? this->m_onDisable : this->m_onEnable;

    if (this->HasScript(script) && !this->m_isLoading) {
        this->RunScript(script, 0, NULL);
    }
}

// OFFSET: %0x005258C0
CSimpleButton::CSimpleButton(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_state = BUTTONSTATE_DISABLED;
    this->m_inClick = 0;
    this->m_stateLocked = 0;
    this->m_motionScriptsWhileDisabled = 0;
    this->m_lastClickTime = 0;
    this->m_clickAction = 0x80000000;
    this->m_fontString = NULL;
    this->m_normalFont = NULL;
    this->m_highlightFont = NULL;
    this->m_disabledFont = NULL;
    this->m_activeTexture = NULL;
    this->m_highlightTexture = NULL;
    this->m_pushedTextOffset.x = 0.001f;
    this->m_pushedTextOffset.y = -0.001f;

    for (uint32_t i = 0; i < NUM_BUTTONSTATES; i++) {
        this->m_stateTextures[i] = NULL;
    }

    this->Enable(1);
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    this->SetFrameFlag(0x10000, 1);
}
