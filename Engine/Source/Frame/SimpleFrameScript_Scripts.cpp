#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/SimpleFrameScript.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/Frame/CSimpleColorSelect.h"
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/Frame/CSimpleEditBox.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"
#include "Engine/Source/Frame/CSimpleHTML.h"
#include "Engine/Source/Frame/CSimpleMessageFrame.h"
#include "Engine/Source/Frame/CSimpleMessageScrollFrame.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
#include "Engine/Source/Frame/CSimpleScaleAnim.h"
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/Frame/CSimpleSlider.h"
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"

// STATIC: 0x00DD3918
const char* s_createFrameTaint;

// OFFSET: 0x0083F590/0x14056B130
static int32_t Script_GetText(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    int32_t ordinal = -1;
    FRAMESCRIPT_GENDER gender = GENDER_NOT_APPLICABLE;

    if (!token) {
        luaL_error(L, "Usage: GetText(\"token\" [,gender] [,ordinal])");
    }

    if (lua_isnumber(L, 2)) {
        gender = static_cast<FRAMESCRIPT_GENDER>(lua_tointeger(L, 2));
    }

    if (lua_isnumber(L, 3)) {
        ordinal = lua_tointeger(L, 3);
    }

    lua_pushstring(L, FrameScript_GetText(token, ordinal, gender));
    return 1;
}

// PROVEN: 4547cc46 IDENTICAL/UNPROVEN
// OFFSET: 0x0083F620/0x14056B1F0
static int32_t Script_CreateFont(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CreateFont(\"name\")");
        return 0;
    }

    CSimpleFont* font = CSimpleFont::GetFont(lua_tolstring(L, 1, NULL), 1);

    if (!font->m_luaRegistered) {
        font->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, font->m_luaObjectRef);
    return 1;
}

// PROVEN: aaa48d54 IDENTICAL/UNPROVEN
// OFFSET: 0x0083F690/0x14056B280
static int32_t Script_GetCurrentKeyBoardFocus(lua_State* L) {
    CSimpleEditBox* focus = CSimpleEditBox::s_currentFocus;

    if (!focus) {
        lua_pushnil(L);
        return 1;
    }

    if (!focus->m_luaRegistered) {
        focus->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, focus->m_luaObjectRef);
    return 1;
}

// PROVEN: d5778d0d IDENTICAL/NONE
// OFFSET: 0x0083F6E0/0x14056B2E0
static int32_t Script_SetMaxAnimFramerate(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetMaxFramerate(framesPerSec)");
        return 0;
    }

    CSimpleAnim::s_frameInterval = 1.0f / static_cast<float>(lua_tonumber(L, 1));
    return 0;
}

// PROVEN: 0346a59f IDENTICAL/NONE
// OFFSET: 0x0083F730/0x14056B340
static int32_t Script_GetMaxAnimFramerate(lua_State* L) {
    lua_pushnumber(L, 1.0f / CSimpleAnim::s_frameInterval);
    return 1;
}

// OFFSET: 0x0083F8B0/0x14056B4F0
static int32_t Script_EnumerateFrames(lua_State* L) {
    CSimpleFrame* frame;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        CSimpleFrame* current = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!current) {
            luaL_error(L, "EnumerateFrames: Couldn't find 'this' in current object");
            return 0;
        }

        if (!current->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "EnumerateFrames: Wrong current object type, expected frame");
            return 0;
        }

        frame = current->m_framesLink.Next();
    } else {
        frame = CSimpleTop::s_instance->m_frames.Head();
    }

    while (frame && frame->IsA(CSimpleHyperlinkButton::GetObjectType())) {
        frame = frame->m_framesLink.Next();
    }

    if (!frame) {
        lua_pushnil(L);
        return 1;
    }

    if (!frame->m_luaRegistered) {
        frame->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, frame->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x0083F9E0/0x14056B640
static int32_t Script_GetNumFrames(lua_State* L) {
    uint32_t count = 0;

    for (CSimpleFrame* frame = CSimpleTop::s_instance->m_frames.Head(); frame; frame = CSimpleTop::s_instance->m_frames.Next(frame)) {
        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0083FB40/0x14056B7C0
static int32_t Script_CreateFrame(lua_State* L) {
    if (!lua_isstring(L, 1) || (lua_type(L, 3) != LUA_TNONE && lua_type(L, 3) != LUA_TNIL && lua_type(L, 3) != LUA_TTABLE)) {
        luaL_error(L, "Usage: CreateFrame(\"frameType\" [, \"name\"] [, parent] [, \"template\"] [, id])");
        return 0;
    }

    CSimpleFrame* parent = NULL;
    const char* frameType = lua_tolstring(L, 1, NULL);
    const char* name = lua_tolstring(L, 2, NULL);
    const char* inherits = lua_tolstring(L, 4, NULL);
    const char* savedTaint = s_createFrameTaint;
    s_createFrameTaint = lua_tainted;

    if (lua_type(L, 3) == LUA_TTABLE) {
        lua_rawgeti(L, 3, 0);
        parent = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!parent) {
            luaL_error(L, "CreateFrame: Couldn't find 'this' in parent object");
            s_createFrameTaint = savedTaint;
            return 0;
        }

        if (!parent->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "CreateFrame: Wrong parent object type, expected frame");
            s_createFrameTaint = savedTaint;
            return 0;
        }
    }

    if (lua_type(L, 4) == LUA_TSTRING) {
        inherits = lua_tolstring(L, 4, NULL);

        char inheritName[1024];

        do {
            SStrTokenize(&inherits, inheritName, sizeof(inheritName), " ,", NULL);

            if (!inheritName[0]) {
                break;
            }

            XMLHashNodeRef ref;

            if (!ref.Acquire(inheritName)) {
                luaL_error(L, "CreateFrame(): Couldn't find inherited node \"%s\"", inheritName);
                s_createFrameTaint = savedTaint;
                return 0;
            }

            if (ref.m_locked) {
                luaL_error(L, "CreateFrame(): Recursively inherited node \"%s\"", inheritName);
                s_createFrameTaint = savedTaint;
                return 0;
            }
        } while (inheritName[0]);

        inherits = lua_tolstring(L, 4, NULL);
    }

    CStatus status;
    CSimpleFrame* frame = NULL;

    {
        XMLNode node(NULL, frameType);

        if (name) {
            node.SetAttribute("name", name);
        }

        if (parent) {
            node.SetAttribute("parent", "");
        }

        if (inherits) {
            node.SetAttribute("inherits", inherits);
        }

        if (lua_isstring(L, 5)) {
            node.SetAttribute("id", lua_tolstring(L, 5, NULL));
        } else if (lua_isnumber(L, 5)) {
            char id[256];
            SStrPrintf(id, sizeof(id), "%d", lua_tointeger(L, 5));
            node.SetAttribute("id", id);
        }

        linkObject* compiled = FrameXML_CompileXML_Region(&node);

        if (compiled) {
            frame = FrameXML_CreateFrame(compiled, parent, &status);
            FrameXML_FreeCompiled(compiled, false);
        }
    }

    if (!frame) {
        luaL_error(L, "CreateFrame: Unknown frame type '%s'", frameType);
        s_createFrameTaint = savedTaint;
        return 0;
    }

    if (!frame->ProtectedFunctionsAllowed()) {
        luaL_error(L, "CreateFrame: Can't create '%s' now", frameType);
        s_createFrameTaint = savedTaint;
        return 0;
    }

    if (!frame->m_luaRegistered) {
        frame->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, frame->m_luaObjectRef);
    s_createFrameTaint = savedTaint;
    return 1;
}

// OFFSET: 0x0083FA50/0x14056B6A0
static int32_t Script_GetFramesRegisteredForEvent(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetFramesRegisteredForEvent(\"event\")");
        return 0;
    }

    TSList<EVENTLISTENERNODE>* listeners = FrameScript_GetEventListeners(lua_tolstring(L, 1, NULL));

    if (!listeners) {
        return 0;
    }

    int32_t count = 0;

    for (EVENTLISTENERNODE* node = listeners->Head(); node; node = listeners->Next(node)) {
        count++;
    }

    if (!lua_checkstack(L, count)) {
        luaL_error(L, "GetFramesRegisteredForEvent(%s): Stack overflow", lua_tolstring(L, 1, NULL));
        return 0;
    }

    for (EVENTLISTENERNODE* node = listeners->Head(); node; node = listeners->Next(node)) {
        FrameScript_Object* listener = node->m_listener;

        if (!listener->m_luaRegistered) {
            listener->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, listener->m_luaObjectRef);
    }

    return count;
}

// STATIC: 0x00D08478
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetText", &Script_GetText },
    { "GetNumFrames", &Script_GetNumFrames },
    { "EnumerateFrames", &Script_EnumerateFrames },
    { "CreateFont", &Script_CreateFont },
    { "CreateFrame", &Script_CreateFrame },
    { "GetFramesRegisteredForEvent", &Script_GetFramesRegisteredForEvent },
    { "GetCurrentKeyBoardFocus", &Script_GetCurrentKeyBoardFocus },
    { "SetMaxAnimFramerate", &Script_SetMaxAnimFramerate },
    { "GetMaxAnimFramerate", &Script_GetMaxAnimFramerate },
};

// OFFSET: 0x0083F760
void RegisterSimpleFrameScriptMethods() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }

    CSimpleAnim::CreateScriptMetaTable();
    CSimpleTranslationAnim::CreateScriptMetaTable();
    CSimpleRotationAnim::CreateScriptMetaTable();
    CSimpleScaleAnim::CreateScriptMetaTable();
    CSimpleControlPoint::CreateScriptMetaTable();
    CSimplePathAnim::CreateScriptMetaTable();
    CSimpleAlphaAnim::CreateScriptMetaTable();
    CSimpleAnimGroup::CreateScriptMetaTable();
    CSimpleFont::CreateScriptMetaTable();
    CSimpleTexture::CreateScriptMetaTable();
    CSimpleFontString::CreateScriptMetaTable();
    CSimpleTitleRegion::CreateScriptMetaTable();
    CSimpleFrame::CreateScriptMetaTable();
    CSimpleButton::CreateScriptMetaTable();
    CSimpleCheckbox::CreateScriptMetaTable();
    CSimpleEditBox::CreateScriptMetaTable();
    CSimpleHTML::CreateScriptMetaTable();
    CSimpleMessageFrame::CreateScriptMetaTable();
    CSimpleMessageScrollFrame::CreateScriptMetaTable();
    CSimpleModel::CreateScriptMetaTable();
    CSimpleModelFFX::CreateScriptMetaTable();
    CSimpleScrollFrame::CreateScriptMetaTable();
    CSimpleSlider::CreateScriptMetaTable();
    CSimpleStatusBar::CreateScriptMetaTable();
    CSimpleColorSelect::CreateScriptMetaTable();
    CSimpleMovieFrame::CreateScriptMetaTable();
}

// OFFSET: 0x0083F810/0x14056B440
void UnregisterSimpleFrameScriptMethods() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }

    CSimpleAnim::DestroyScriptMetaTable();
    CSimpleTranslationAnim::DestroyScriptMetaTable();
    CSimpleRotationAnim::DestroyScriptMetaTable();
    CSimpleScaleAnim::DestroyScriptMetaTable();
    CSimpleControlPoint::DestroyScriptMetaTable();
    CSimplePathAnim::DestroyScriptMetaTable();
    CSimpleAlphaAnim::DestroyScriptMetaTable();
    CSimpleAnimGroup::DestroyScriptMetaTable();
    CSimpleFont::DestroyScriptMetaTable();
    CSimpleTexture::DestroyScriptMetaTable();
    CSimpleFontString::DestroyScriptMetaTable();
    CSimpleTitleRegion::DestroyScriptMetaTable();
    CSimpleFrame::DestroyScriptMetaTable();
    CSimpleButton::DestroyScriptMetaTable();
    CSimpleCheckbox::DestroyScriptMetaTable();
    CSimpleEditBox::DestroyScriptMetaTable();
    CSimpleHTML::DestroyScriptMetaTable();
    CSimpleMessageFrame::DestroyScriptMetaTable();
    CSimpleMessageScrollFrame::DestroyScriptMetaTable();
    CSimpleModel::DestroyScriptMetaTable();
    CSimpleScrollFrame::DestroyScriptMetaTable();
    CSimpleSlider::DestroyScriptMetaTable();
    CSimpleStatusBar::DestroyScriptMetaTable();
    CSimpleColorSelect::DestroyScriptMetaTable();
    CSimpleMovieFrame::DestroyScriptMetaTable();
}
