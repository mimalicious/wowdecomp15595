#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CBackdropGenerator.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/UIBindings.h"

// OFFSET: 0x0050B050/0x1401534B0
static int32_t Script_GetTitleRegion(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CScriptRegion* titleRegion = frame->m_titleRegion;

    if (!titleRegion) {
        lua_pushnil(L);
        return 1;
    }

    if (!titleRegion->m_luaRegistered) {
        titleRegion->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, titleRegion->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x0050B0C0/0x140153540
static int32_t Script_CreateTitleRegion(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CScriptRegion* titleRegion = frame->m_titleRegion;

    if (titleRegion) {
        titleRegion->MarkFramePointsUnused();
    } else {
        void* data = SMemAlloc(sizeof(CSimpleTitleRegion), __FILE__, __LINE__, 0);
        titleRegion = data ? new (data) CSimpleTitleRegion : NULL;
        titleRegion->SetParent(frame);
        frame->m_titleRegion = titleRegion;
        titleRegion->SetDeferredResize(frame->m_layoutFlags & 2);
    }

    if (!titleRegion->m_luaRegistered) {
        titleRegion->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, titleRegion->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x0050E960/0x140157950
static int32_t Script_CreateTexture(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    int8_t subLevel = 0;
    const char* name = NULL;

    if (lua_isstring(L, 2)) {
        name = lua_tolstring(L, 2, NULL);
    }

    uint32_t drawLayer = 2;

    if (lua_isstring(L, 3)) {
        StringToDrawLayer(lua_tolstring(L, 3, NULL), drawLayer);
    }

    XMLHashNodeRef ref;

    if (lua_type(L, 4) == LUA_TSTRING) {
        const char* inherits = lua_tolstring(L, 4, NULL);

        if (!ref.Acquire(inherits)) {
            const char* frameName = frame->GetName();
            luaL_error(L, "%s:CreateTexture(): Couldn't find inherited node \"%s\"", frameName ? frameName : "<unnamed>", inherits);
            return 0;
        }

        if (ref.m_locked) {
            const char* frameName = frame->GetName();
            luaL_error(L, "%s:CreateTexture(): Recursively inherited node \"%s\"", frameName ? frameName : "<unnamed>", inherits);
            return 0;
        }
    }

    if (lua_isnumber(L, 5)) {
        int32_t level = static_cast<int32_t>(lua_tointeger(L, 5));
        subLevel = static_cast<int8_t>(level);

        if (level > 7 || level < -8) {
            const char* frameName = frame->GetName();
            luaL_error(L, "%s:CreateTexture(): sublevel must be between %d and %d", frameName ? frameName : "<unnamed>", -8, 7);
            return 0;
        }
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, drawLayer, subLevel, true) : NULL;

    if (name && *name) {
        texture->SetName(name);
    }

    if (ref.m_node) {
        CStatus status;
        texture->LoadXML(ref.m_node, &status);
        texture->PostLoadXML(ref.m_node, &status);
    }

    if (texture->HasActiveAnimGroup()) {
        int32_t alpha = texture->HasActiveAlphaAnim();
        int32_t position = texture->HasActivePositionAnim();
        texture->AnimActivated(NULL, alpha, position);
    }

    if (!texture->m_luaRegistered) {
        texture->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, texture->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x0050EBF0/0x140157CD0
static int32_t Script_CreateFontString(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    const char* name = NULL;

    if (lua_isstring(L, 2)) {
        name = lua_tolstring(L, 2, NULL);
    }

    uint32_t drawLayer = 2;

    if (lua_isstring(L, 3)) {
        StringToDrawLayer(lua_tolstring(L, 3, NULL), drawLayer);
    }

    CSimpleFont* font = NULL;
    XMLHashNodeRef ref;

    if (lua_type(L, 4) == LUA_TSTRING) {
        const char* inherits = lua_tolstring(L, 4, NULL);
        font = CSimpleFont::GetFont(inherits, 0);

        if (!font) {
            ref.Acquire(inherits);

            if (!ref.m_node) {
                const char* frameName = frame->GetName();
                luaL_error(L, "%s:CreateFontString(): Couldn't find inherited node \"%s\"", frameName ? frameName : "<unnamed>", inherits);
                return 0;
            }

            if (ref.m_locked) {
                const char* frameName = frame->GetName();
                luaL_error(L, "%s:CreateFontString(): Recursively inherited node \"%s\"", frameName ? frameName : "<unnamed>", inherits);
                return 0;
            }
        }
    }

    void* data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
    CSimpleFontString* string = data ? new (data) CSimpleFontString(frame, drawLayer, true) : NULL;

    if (name && *name) {
        string->SetName(name);
    }

    if (font) {
        string->SetFontObject(font);
    } else if (ref.m_node) {
        CStatus status;
        string->LoadXML(ref.m_node, &status);
        string->PostLoadXML(ref.m_node, &status);
    }

    if (string->HasActiveAnimGroup()) {
        int32_t alpha = string->HasActiveAlphaAnim();
        int32_t position = string->HasActivePositionAnim();
        string->AnimActivated(NULL, alpha, position);
    }

    if (!string->m_luaRegistered) {
        string->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, string->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x0050B180/0x140153630
static int32_t Script_GetBoundsRect(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CRect rect = { 3.402823466e38f, 3.402823466e38f, 0.0f, 0.0f };

    if (!frame->GetBoundsRect(rect)) {
        return 0;
    }

    float scale = 1.0f / frame->m_layoutScale;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scale * rect.minX));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scale * rect.minY));
    float width = rect.maxX - rect.minX;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scale * width));
    float height = rect.maxY - rect.minY;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scale * height));

    return 4;
}

// OFFSET: 0x0050E670/0x140157600
static int32_t Script_GetNumRegions(lua_State* L) {
    uint32_t count = 0;
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    for (CSimpleRegion* region = frame->m_regions.Head(); region; region = frame->m_regions.Next(region)) {
        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0050E700/0x140157690
static int32_t Script_GetRegions(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    int32_t count = 0;

    for (CSimpleRegion* region = frame->m_regions.Head(); region; region = frame->m_regions.Next(region)) {
        count++;
    }

    if (!lua_checkstack(L, count)) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetRegions(): Stack overflow", name ? name : "<unnamed>");
        return 0;
    }

    for (CSimpleRegion* region = frame->m_regions.Head(); region; region = frame->m_regions.Next(region)) {
        if (!region->m_luaRegistered) {
            region->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, region->m_luaObjectRef);
    }

    return count;
}

// OFFSET: 0x0050E800/0x1401577C0
static int32_t Script_GetNumChildren(lua_State* L) {
    uint32_t count = 0;
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    for (SIMPLEFRAMENODE* node = frame->m_children.Head(); node; node = node->m_link.Next()) {
        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0050E880/0x140157840
static int32_t Script_GetChildren(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    int32_t count = 0;

    for (SIMPLEFRAMENODE* node = frame->m_children.Head(); node; node = node->m_link.Next()) {
        count++;
    }

    if (!lua_checkstack(L, count)) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetChildren(): Stack overflow", name ? name : "<unnamed>");
        return 0;
    }

    for (SIMPLEFRAMENODE* node = frame->m_children.Head(); node; node = node->m_link.Next()) {
        CSimpleFrame* child = node->m_frame;

        if (!child->m_luaRegistered) {
            child->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, child->m_luaObjectRef);
    }

    return count;
}

// PROVEN: 954b252e IDENTICAL/NONE
// OFFSET: 0x0050B2C0/0x1401537B0
static int32_t Script_GetFrameStrata(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    lua_pushstring(L, FrameStrataToString(static_cast<FRAME_STRATA>(frame->m_strata)));
    return 1;
}

// OFFSET: 0x0050B310/0x140153810
static int32_t Script_SetFrameStrata(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (!lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetFrameStrata(level)", name ? name : "<unnamed>");
        return 0;
    }

    FRAME_STRATA strata;

    if (!StringToFrameStrata(lua_tolstring(L, 2, NULL), strata)) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:SetFrameStrata(): Unknown frame strata: %s", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
        return 0;
    }

    frame->SetFrameStrata(strata);
    return 0;
}

// OFFSET: 0x0050B410/0x140153940
static int32_t Script_GetFrameLevel(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    lua_pushnumber(L, frame->m_level);
    return 1;
}

// OFFSET: 0x0050B460/0x140153990
static int32_t Script_SetFrameLevel(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetFrameLevel(level)", name ? name : "<unnamed>");
        return 0;
    }

    int32_t level = static_cast<int32_t>(lua_tonumber(L, 2));

    if (level < 0) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:SetFrameLevel(): Passed negative frame level: %d", name ? name : "<unnamed>", level);
        return 0;
    }

    frame->SetFrameLevel(level, 1);
    return 0;
}

// OFFSET: 0x0050B540/0x140153AC0
static int32_t Script_HasScript(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:HasScript(\"type\")", name ? name : "<unnamed>");
        return 0;
    }

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    if (frame->GetScriptByNameCamelCase(lua_tolstring(L, 2, NULL), data)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: d8839d20 IDENTICAL/NONE
// OFFSET: 0x0050B600
static int32_t Script_GetScript(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    return frame ? frame->GetScript(L) : 0;
}

// PROVEN: 0959de32 IDENTICAL/NONE
// OFFSET: 0x0050B640
static int32_t Script_SetScript(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    return frame ? frame->SetScript(L) : 0;
}

// PROVEN: 587a1098 IDENTICAL/NONE
// OFFSET: 0x0050B680/0x140153C50
static int32_t Script_HookScript(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    return frame ? frame->HookScript(L) : 0;
}

// OFFSET: 0x0050B6C0/0x140153CA0
static int32_t Script_RegisterEvent(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame || !frame->IsA(CScriptObject::GetObjectType()) || !lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:RegisterEvent(\"event\")", name ? name : "<unnamed>");
        return 0;
    }

    frame->RegisterScriptEvent(lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x0050B770/0x140153D70
static int32_t Script_UnregisterEvent(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame || !frame->IsA(CScriptObject::GetObjectType()) || !lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:UnregisterEvent(\"event\")", name ? name : "<unnamed>");
        return 0;
    }

    frame->UnregisterScriptEvent(lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x0050B820/0x140153E40
static int32_t Script_RegisterAllEvents(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    frame->RegisterAllScriptEvents();
    return 0;
}

// OFFSET: 0x0050B860/0x140153E80
static int32_t Script_UnregisterAllEvents(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    frame->UnregisterAllScriptEvents();
    return 0;
}

// OFFSET: 0x0050B8A0/0x140153EC0
static int32_t Script_IsEventRegistered(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:IsEventRegistered(\"event\")", name ? name : "<unnamed>");
        return 0;
    }

    if (frame->IsScriptEventRegistered(lua_tolstring(L, 2, NULL))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: eb2e490e DIFFERENT/IDENTICAL
// OFFSET: 0x0050B950/0x140153F90
static int32_t Script_AllowAttributeChanges(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!lua_tainted) {
        frame->AllowAttributeChanges(1);
    }

    return 0;
}

// OFFSET: 0x0050B990/0x140153FE0
static int32_t Script_CanChangeAttribute(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->ProtectedFunctionsAllowed() || frame->AttributeChangesAllowed()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050BA00/0x140154070
static int32_t Script_GetAttribute(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (lua_gettop(L) == 4 && lua_isstring(L, 3)) {
        size_t prefixLength;
        size_t nameLength;
        size_t suffixLength;
        const char* prefix = lua_tolstring(L, 2, &prefixLength);
        const char* name = lua_tolstring(L, 3, &nameLength);
        const char* suffix = lua_tolstring(L, 4, &suffixLength);

        char buffer[256];
        char* dest = buffer;
        size_t space = sizeof(buffer) - 1;

        if (prefixLength) {
            size_t count = prefixLength > space ? space : prefixLength;
            memcpy(buffer, prefix, count);
            dest = buffer + count;
            space -= count;
        }

        if (nameLength) {
            size_t count = space < nameLength ? space : nameLength;
            memcpy(dest, name, count);
            dest += count;
            space -= count;
        }

        if (suffixLength) {
            size_t count = space < suffixLength ? space : suffixLength;
            memcpy(dest, suffix, count);
            dest += count;
        }

        *dest = '\0';
        int32_t luaRef;

        if (frame->GetAttribute(buffer, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        buffer[0] = '*';
        dest = buffer + 1;
        space = sizeof(buffer) - 2;

        if (nameLength) {
            size_t count = nameLength > space ? space : nameLength;
            memcpy(buffer + 1, name, count);
            dest = buffer + 1 + count;
            space -= count;
        }

        if (suffixLength) {
            size_t count = space < suffixLength ? space : suffixLength;
            memcpy(dest, suffix, count);
            dest += count;
        }

        *dest = '\0';

        if (frame->GetAttribute(buffer, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        dest = buffer;
        space = sizeof(buffer) - 2;

        if (prefixLength) {
            size_t count = prefixLength > space ? space : prefixLength;
            memcpy(buffer, prefix, count);
            dest = buffer + count;
            space -= count;
        }

        if (nameLength) {
            size_t count = space < nameLength ? space : nameLength;
            memcpy(dest, name, count);
            dest += count;
        }

        dest[0] = '*';
        dest[1] = '\0';

        if (frame->GetAttribute(buffer, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        buffer[0] = '*';
        dest = buffer + 1;

        if (nameLength) {
            size_t count = nameLength > sizeof(buffer) - 3 ? sizeof(buffer) - 3 : nameLength;
            memcpy(buffer + 1, name, count);
            dest = buffer + 1 + count;
        }

        dest[0] = '*';
        dest[1] = '\0';

        if (frame->GetAttribute(buffer, luaRef) || frame->GetAttribute(name, luaRef)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    if (!lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:GetAttribute(\"name\")", name ? name : "<unnamed>");
        return 0;
    }

    int32_t luaRef;

    if (frame->GetAttribute(lua_tolstring(L, 2, NULL), luaRef)) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaRef);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050BD40/0x140154470
static int32_t Script_SetAttribute(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed() && !frame->AttributeChangesAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    lua_settop(L, 3);

    if (!lua_isstring(L, 2) || lua_type(L, 3) == LUA_TNONE) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetAttribute(\"name\", value)", name ? name : "<unnamed>");
        return 0;
    }

    const char* name = lua_tolstring(L, 2, NULL);
    int32_t luaRef;

    if (frame->GetAttribute(name, luaRef)) {
        luaL_unref(L, LUA_REGISTRYINDEX, luaRef);
    }

    const char* taint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = NULL;
    }

    lua_settaint(L, 3, NULL);
    luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }

    frame->SetAttribute(name, luaRef);
    return 0;
}

// OFFSET: 0x0050BEC0/0x140154630
static int32_t Script_GetEffectiveScale(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    lua_pushnumber(L, frame->m_layoutScale);
    return 1;
}

// OFFSET: 0x0050BF10/0x140154680
static int32_t Script_GetScale(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    lua_pushnumber(L, frame->m_frameScale);
    return 1;
}

// OFFSET: 0x0050BF70/0x1401546D0
static int32_t Script_SetScale(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetScale(scale)", name ? name : "<unnamed>");
        return 0;
    }

    double scale = lua_tonumber(L, 2);

    if (scale <= 0.0) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:SetScale(): Scale must be > 0", name ? name : "<unnamed>");
        return 0;
    }

    frame->SetFrameScale(static_cast<float>(scale), false);
    return 0;
}

// OFFSET: 0x0050C050/0x1401547C0
static int32_t Script_GetEffectiveAlpha(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    uint8_t alpha = frame->m_alpha * frame->m_parentAlpha / 255;
    lua_pushnumber(L, alpha / 255.0);
    return 1;
}

// OFFSET: 0x0050C0C0/0x140154840
static int32_t Script_GetAlpha(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    lua_pushnumber(L, frame->m_alpha / 255.0);
    return 1;
}

// OFFSET: 0x0050C120/0x1401548A0
static int32_t Script_SetAlpha(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetAlpha(alpha 0 to 1)", name ? name : "<unnamed>");
        return 0;
    }

    float alpha = lua_tonumber(L, 2);

    if (alpha < 0.0f) {
        alpha = 0.0f;
    } else if (alpha >= 1.0f) {
        alpha = 1.0f;
    }

    frame->SetFrameAlpha(static_cast<uint8_t>(alpha * 255.0f));
    return 0;
}

// OFFSET: 0x0050C1E0/0x140154970
static int32_t Script_GetID(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    lua_pushnumber(L, frame->m_id);
    return 1;
}

// OFFSET: 0x0050C230/0x1401549C0
static int32_t Script_SetID(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetID(ID)", name ? name : "<unnamed>");
        return 0;
    }

    frame->m_id = static_cast<int32_t>(lua_tonumber(L, 2));
    return 0;
}

// OFFSET: 0x0050C2E0/0x140154A90
static int32_t Script_SetToplevel(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    frame->SetFrameFlag(0x1, StringToBOOL(L, 2, 1));
    return 0;
}

// PROVEN: eb52d9f8 IDENTICAL/NONE
// OFFSET: 0x0050C350/0x140154B30
static int32_t Script_IsToplevel(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: e18f72c3 IDENTICAL/NONE
// OFFSET: 0x0050C3B0/0x140154B90
static int32_t Script_EnableDrawLayer(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    uint32_t drawLayer = 2;

    if (lua_isstring(L, 2)) {
        StringToDrawLayer(lua_tolstring(L, 2, NULL), drawLayer);
    }

    frame->EnableDrawLayer(drawLayer);
    return 0;
}

// PROVEN: 12f316de IDENTICAL/NONE
// OFFSET: 0x0050C420/0x140154C20
static int32_t Script_DisableDrawLayer(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    uint32_t drawLayer = 2;

    if (lua_isstring(L, 2)) {
        StringToDrawLayer(lua_tolstring(L, 2, NULL), drawLayer);
    }

    frame->DisableDrawLayer(drawLayer);
    return 0;
}

// OFFSET: 0x0050C490/0x140154CB0
static int32_t Script_Show(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    frame->Show();
    return 0;
}

// OFFSET: 0x0050C4F0
static int32_t Script_Hide(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    frame->Hide();
    return 0;
}

// OFFSET: 0x0050C550
static int32_t Script_IsVisible(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_visible) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050C5B0/0x140154E00
static int32_t Script_IsShown(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_shown) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050C610/0x140154E70
static int32_t Script_Raise(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    frame->Raise();
    return 0;
}

// OFFSET: 0x0050C670/0x140154EE0
static int32_t Script_Lower(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    frame->Lower();
    return 0;
}

// OFFSET: 0x0050C6D0/0x140154F50
static int32_t Script_GetHitRectInsets(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    float left;
    float right;
    float top;
    float bottom;
    frame->GetHitRectInsets(left, right, top, bottom);

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * left));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * right));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * top));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bottom));

    return 4;
}

// OFFSET: 0x0050C7B0/0x140155050
static int32_t Script_SetHitRectInsets(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4) || !lua_isnumber(L, 5)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetHitRectInsets(left, right, top, bottom)", name ? name : "<unnamed>");
        return 0;
    }

    float left = lua_tonumber(L, 2);
    left = NDCToDDCWidth(left / (CoordinateGetAspectCompensation() * 1024.0f));
    float right = lua_tonumber(L, 3);
    right = NDCToDDCWidth(right / (CoordinateGetAspectCompensation() * 1024.0f));
    float top = lua_tonumber(L, 4);
    top = NDCToDDCWidth(top / (CoordinateGetAspectCompensation() * 1024.0f));
    float bottom = lua_tonumber(L, 5);
    bottom = NDCToDDCWidth(bottom / (CoordinateGetAspectCompensation() * 1024.0f));

    frame->SetHitRectInsets(left, right, top, bottom);
    return 0;
}

// OFFSET: 0x0050C950/0x140155270
static int32_t Script_GetClampRectInsets(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    float left;
    float right;
    float top;
    float bottom;
    frame->GetClampRectInsets(left, right, top, bottom);

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * left));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * right));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * top));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bottom));

    return 4;
}

// OFFSET: 0x0050CA30/0x140155370
static int32_t Script_SetClampRectInsets(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4) || !lua_isnumber(L, 5)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetClampRectInsets(left, right, top, bottom)", name ? name : "<unnamed>");
        return 0;
    }

    float left = lua_tonumber(L, 2);
    left = NDCToDDCWidth(left / (CoordinateGetAspectCompensation() * 1024.0f));
    float right = lua_tonumber(L, 3);
    right = NDCToDDCWidth(right / (CoordinateGetAspectCompensation() * 1024.0f));
    float top = lua_tonumber(L, 4);
    top = NDCToDDCWidth(top / (CoordinateGetAspectCompensation() * 1024.0f));
    float bottom = lua_tonumber(L, 5);
    bottom = NDCToDDCWidth(bottom / (CoordinateGetAspectCompensation() * 1024.0f));

    frame->SetClampRectInsets(left, right, top, bottom);
    return 0;
}

// OFFSET: 0x0050CBA0/0x140155550
static int32_t Script_GetMinResize(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CRect bounds = frame->m_resizeRect;

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bounds.minX));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bounds.minY));

    return 2;
}

// OFFSET: 0x0050CC40/0x140155600
static int32_t Script_SetMinResize(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CRect bounds = frame->m_resizeRect;

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetMinResize(minWidth, minHeight)", name ? name : "<unnamed>");
        return 0;
    }

    float width = lua_tonumber(L, 2);
    bounds.minX = NDCToDDCWidth(width / (CoordinateGetAspectCompensation() * 1024.0f));
    float height = lua_tonumber(L, 3);
    bounds.minY = NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f));

    frame->m_resizeRect = bounds;
    return 0;
}

// OFFSET: 0x0050CD60/0x140155760
static int32_t Script_GetMaxResize(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CRect bounds = frame->m_resizeRect;

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bounds.maxX));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bounds.maxY));

    return 2;
}

// OFFSET: 0x0050CE00/0x140155810
static int32_t Script_SetMaxResize(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CRect bounds = frame->m_resizeRect;

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetMaxResize(maxWidth, maxHeight)", name ? name : "<unnamed>");
        return 0;
    }

    float width = lua_tonumber(L, 2);
    bounds.maxX = NDCToDDCWidth(width / (CoordinateGetAspectCompensation() * 1024.0f));
    float height = lua_tonumber(L, 3);
    bounds.maxY = NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f));

    frame->m_resizeRect = bounds;
    return 0;
}

// PROVEN: 383d27c5 IDENTICAL/IDENTICAL
// OFFSET: 0x0050CF20/0x140155970
static int32_t Script_SetMovable(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    frame->SetFrameFlag(0x100, StringToBOOL(L, 2, 1));
    return 0;
}

// PROVEN: 6e023cb9 IDENTICAL/UNPROVEN
// OFFSET: 0x0050CF70/0x1401559E0
static int32_t Script_IsMovable(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x100) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: e1851ef9 IDENTICAL/IDENTICAL
// OFFSET: 0x0050CFD0/0x140155A50
static int32_t Script_SetDontSavePosition(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    frame->SetFrameFlag(0x80000, StringToBOOL(L, 2, 1));
    return 0;
}

// PROVEN: 099468cb IDENTICAL/NONE
// OFFSET: 0x0050D020/0x140155AC0
static int32_t Script_GetDontSavePosition(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x80000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: ce1aeefd IDENTICAL/IDENTICAL
// OFFSET: 0x0050D080/0x140155B30
static int32_t Script_SetResizable(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    frame->SetFrameFlag(0x200, StringToBOOL(L, 2, 1));
    return 0;
}

// PROVEN: f0df0bae IDENTICAL/UNPROVEN
// OFFSET: 0x0050D0D0/0x140155BA0
static int32_t Script_IsResizable(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x200) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050D130/0x140155C10
static int32_t Script_StartMoving(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!(frame->m_frameFlags & 0x100)) {
        const char* name = frame->GetName();
        luaL_error(L, "Frame %s is not movable", name ? name : "<unnamed>");
        return 0;
    }

    CSimpleTop* top = frame->m_simpleTop;

    if (!top->m_moveResizeFrame) {
        C2Vector position = frame->m_mousePosition;

        if (0.0f == position.x && 0.0f == position.y) {
            top->GetCursorPosition(position);
        }

        top->StartMoveOrResizeFrame(frame, MOVERESIZE_REASON_SCRIPT, position.x, position.y, FRAMEPOINT_CENTER);
    }

    return 0;
}

// OFFSET: 0x0050D210/0x140155D10
static int32_t Script_StartSizing(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!(frame->m_frameFlags & 0x200)) {
        const char* name = frame->GetName();
        luaL_error(L, "Frame %s is not resizable", name ? name : "<unnamed>");
        return 0;
    }

    FRAMEPOINT point = FRAMEPOINT_BOTTOMRIGHT;

    if (lua_isstring(L, 2)) {
        StringToFramePoint(lua_tolstring(L, 2, NULL), point);
    }

    CSimpleTop* top = frame->m_simpleTop;

    if (!top->m_moveResizeFrame) {
        C2Vector position = frame->m_mousePosition;

        if (0.0f == position.x && 0.0f == position.y) {
            top->GetCursorPosition(position);
        }

        top->StartMoveOrResizeFrame(frame, MOVERESIZE_REASON_SCRIPT, position.x, position.y, point);
    }

    return 0;
}

// PROVEN: 1d7e6624 DIFFERENT/IDENTICAL
// OFFSET: 0x0050D320/0x140155E50
static int32_t Script_StopMovingOrSizing(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CSimpleTop* top = frame->m_simpleTop;

    if (top->m_moveResizeFrame == frame) {
        top->StopMoveOrResizeFrame();
    }

    return 0;
}

// OFFSET: 0x0050D370/0x140155EA0
static int32_t Script_SetUserPlaced(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!(frame->m_frameFlags & 0x300)) {
        const char* name = frame->GetName();
        luaL_error(L, "Frame %s is not movable or resizable", name ? name : "<unnamed>");
        return 0;
    }

    frame->SetFrameFlag(0x1000, StringToBOOL(L, 2, 1));
    return 0;
}

// PROVEN: 05737aa4 IDENTICAL/NONE
// OFFSET: 0x0050D3F0/0x140155F50
static int32_t Script_IsUserPlaced(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x1000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: a7292620 IDENTICAL/UNPROVEN
// OFFSET: 0x0050D450/0x140155FC0
static int32_t Script_SetClampedToScreen(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    frame->SetClampedToScreen(StringToBOOL(L, 2, 1));
    return 0;
}

// PROVEN: 6a8b4f79 IDENTICAL/NONE
// OFFSET: 0x0050D4A0/0x140156020
static int32_t Script_IsClampedToScreen(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_layoutFlags & 0x10) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050D500/0x140156080
static int32_t Script_RegisterForDrag(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    uint32_t buttons = 0;

    for (int32_t i = 2; lua_isstring(L, i); i++) {
        buttons |= StringToMouseButton(lua_tolstring(L, i, NULL));
    }

    frame->m_dragButtons = buttons;
    return 0;
}

// OFFSET: 0x0050D5A0/0x140156120
static int32_t Script_EnableKeyboard(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (StringToBOOL(L, 2, 1)) {
        frame->EnableEvent(SIMPLE_EVENT_KEY, -1);
        frame->EnableEvent(SIMPLE_EVENT_CHAR, -1);
    } else {
        frame->DisableEvent(SIMPLE_EVENT_KEY);
        frame->DisableEvent(SIMPLE_EVENT_CHAR);
    }

    return 0;
}

// OFFSET: 0x0050D640/0x1401561F0
static int32_t Script_IsKeyboardEnabled(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (((frame->m_enabledEventsBitmask >> SIMPLE_EVENT_KEY) & 1) || ((frame->m_enabledEventsBitmask >> SIMPLE_EVENT_CHAR) & 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050D6B0/0x140156260
static int32_t Script_EnableMouse(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (StringToBOOL(L, 2, 1)) {
        frame->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    } else {
        frame->DisableEvent(SIMPLE_EVENT_MOUSE);
    }

    return 0;
}

// OFFSET: 0x0050D740/0x140156310
static int32_t Script_IsMouseEnabled(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if ((frame->m_enabledEventsBitmask >> SIMPLE_EVENT_MOUSE) & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050D7A0/0x140156380
static int32_t Script_EnableMouseWheel(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (StringToBOOL(L, 2, 1)) {
        frame->EnableEvent(SIMPLE_EVENT_MOUSEWHEEL, -1);
    } else {
        frame->DisableEvent(SIMPLE_EVENT_MOUSEWHEEL);
    }

    return 0;
}

// OFFSET: 0x0050D830/0x140156430
static int32_t Script_IsMouseWheelEnabled(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if ((frame->m_enabledEventsBitmask >> SIMPLE_EVENT_MOUSEWHEEL) & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050D890/0x1401564A0
static int32_t Script_EnableJoystick(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!frame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(frame);
        }

        return 0;
    }

    if (StringToBOOL(L, 2, 1)) {
        frame->EnableEvent(SIMPLE_EVENT_4, -1);
    } else {
        frame->DisableEvent(SIMPLE_EVENT_4);
    }

    return 0;
}

// OFFSET: 0x0050D920/0x140156550
static int32_t Script_IsJoystickEnabled(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if ((frame->m_enabledEventsBitmask >> SIMPLE_EVENT_4) & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0050D980/0x1401565C0
static int32_t Script_GetBackdrop(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CBackdropGenerator* backdrop = frame->m_backdropGenerator;

    if (!backdrop) {
        return 0;
    }

    if (lua_type(L, 2) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    const char* bgFile = backdrop->m_bgFile.m_str ? backdrop->m_bgFile.m_str : "";
    const char* edgeFile = backdrop->m_edgeFile.m_str ? backdrop->m_edgeFile.m_str : "";
    uint32_t tile = backdrop->m_tile;

    lua_pushstring(L, "bgFile");
    lua_pushstring(L, bgFile);
    lua_settable(L, -3);

    lua_pushstring(L, "edgeFile");
    lua_pushstring(L, edgeFile);
    lua_settable(L, -3);

    lua_pushstring(L, "tile");

    if (tile) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_settable(L, -3);

    lua_pushstring(L, "tileSize");
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * backdrop->m_tileSize));
    lua_settable(L, -3);

    lua_pushstring(L, "edgeSize");
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * backdrop->m_edgeSize));
    lua_settable(L, -3);

    lua_pushstring(L, "insets");
    lua_gettable(L, -2);

    if (lua_type(L, -1) != LUA_TTABLE) {
        lua_settop(L, -2);
        lua_pushstring(L, "insets");
        lua_createtable(L, 0, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "insets");
        lua_gettable(L, -2);
    }

    float left = backdrop->m_insetLeft;
    float right = backdrop->m_insetRight;
    float top = backdrop->m_insetTop;
    float bottom = backdrop->m_insetBottom;

    lua_pushstring(L, "left");
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * left));
    lua_settable(L, -3);

    lua_pushstring(L, "right");
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * right));
    lua_settable(L, -3);

    lua_pushstring(L, "top");
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * top));
    lua_settable(L, -3);

    lua_pushstring(L, "bottom");
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * bottom));
    lua_settable(L, -3);

    lua_settop(L, -2);
    return 1;
}

// OFFSET: 0x0050DC50/0x140156990
static int32_t Script_SetBackdrop(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (lua_type(L, 2) == LUA_TNIL) {
        frame->SetBackdrop(NULL);
        return 0;
    }

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetBackdrop(nil or {bgFile = \"bgFile\", edgeFile = \"edgeFile\", tile = false, tileSize = 0, edgeSize = 32, insets = { left = 0, right = 0, top = 0, bottom = 0 }})", name ? name : "<unnamed>");
        return 0;
    }

    void* data = SMemAlloc(sizeof(CBackdropGenerator), __FILE__, __LINE__, 0);
    CBackdropGenerator* backdrop = data ? new (data) CBackdropGenerator() : NULL;

    lua_pushstring(L, "bgFile");
    lua_gettable(L, -2);
    const char* bgFile = lua_tolstring(L, -1, NULL);
    lua_settop(L, -2);

    lua_pushstring(L, "edgeFile");
    lua_gettable(L, -2);
    const char* edgeFile = lua_tolstring(L, -1, NULL);
    lua_settop(L, -2);

    lua_pushstring(L, "tile");
    lua_gettable(L, -2);
    int32_t tile = StringToBOOL(L, -1, 0);
    lua_settop(L, -2);

    uint32_t pieces = edgeFile && *edgeFile ? 0xFF : 0;
    backdrop->m_bgFile.Copy(bgFile);
    backdrop->m_pieces = pieces;
    backdrop->m_tile = tile;
    backdrop->m_edgeFile.Copy(edgeFile);
    backdrop->m_alphaMode = GxBlend_Alpha;

    lua_pushstring(L, "tileSize");
    lua_gettable(L, -2);

    if (lua_isnumber(L, -1)) {
        float tileSize = lua_tonumber(L, -1);
        backdrop->m_tileSize = NDCToDDCWidth(tileSize / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    lua_settop(L, -2);

    lua_pushstring(L, "edgeSize");
    lua_gettable(L, -2);

    if (lua_isnumber(L, -1)) {
        float edgeSize = lua_tonumber(L, -1);
        backdrop->m_edgeSize = NDCToDDCWidth(edgeSize / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    lua_settop(L, -2);

    lua_pushstring(L, "insets");
    lua_gettable(L, -2);

    if (lua_type(L, -1) == LUA_TTABLE) {
        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;

        lua_pushstring(L, "left");
        lua_gettable(L, -2);

        if (lua_isnumber(L, -1)) {
            left = lua_tonumber(L, -1);
            left = NDCToDDCWidth(left / (CoordinateGetAspectCompensation() * 1024.0f));
        }

        lua_settop(L, -2);

        lua_pushstring(L, "right");
        lua_gettable(L, -2);

        if (lua_isnumber(L, -1)) {
            right = lua_tonumber(L, -1);
            right = NDCToDDCWidth(right / (CoordinateGetAspectCompensation() * 1024.0f));
        }

        lua_settop(L, -2);

        lua_pushstring(L, "top");
        lua_gettable(L, -2);

        if (lua_isnumber(L, -1)) {
            top = lua_tonumber(L, -1);
            top = NDCToDDCWidth(top / (CoordinateGetAspectCompensation() * 1024.0f));
        }

        lua_settop(L, -2);

        lua_pushstring(L, "bottom");
        lua_gettable(L, -2);

        if (lua_isnumber(L, -1)) {
            bottom = lua_tonumber(L, -1);
            bottom = NDCToDDCWidth(bottom / (CoordinateGetAspectCompensation() * 1024.0f));
        }

        lua_settop(L, -2);

        backdrop->m_insetLeft = left;
        backdrop->m_insetRight = right;
        backdrop->m_insetTop = top;
        backdrop->m_insetBottom = bottom;
    }

    lua_settop(L, -2);

    frame->SetBackdrop(backdrop);

    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (frame->GetRect(&rect)) {
        backdrop->Generate(&rect);
    }

    return 0;
}

// OFFSET: 0x0050E0D0/0x140156F70
static int32_t Script_GetBackdropColor(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CBackdropGenerator* backdrop = frame->m_backdropGenerator;

    if (!backdrop) {
        return 0;
    }

    CImVector color = backdrop->m_backgroundColor;
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: 0x0050E1A0/0x140157070
static int32_t Script_SetBackdropColor(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CImVector color;

    FrameScript_GetColor(L, 2, color);

    if (frame->m_backdropGenerator) {
        frame->m_backdropGenerator->SetVertexColor(color);
    }

    return 0;
}

// OFFSET: 0x0050E210/0x1401570E0
static int32_t Script_GetBackdropBorderColor(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CBackdropGenerator* backdrop = frame->m_backdropGenerator;

    if (!backdrop) {
        return 0;
    }

    CImVector color = backdrop->m_borderColor;
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: 0x0050E2E0/0x1401571E0
static int32_t Script_SetBackdropBorderColor(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    CImVector color;

    FrameScript_GetColor(L, 2, color);

    if (frame->m_backdropGenerator) {
        frame->m_backdropGenerator->SetBorderVertexColor(color);
    }

    return 0;
}

// OFFSET: 0x0050E350/0x140157250
static int32_t Script_SetDepth(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetDepth(additiveDepth)", name ? name : "<unnamed>");
        return 0;
    }

    frame->SetFrameDepth(static_cast<float>(lua_tonumber(L, 2)), false);
    return 0;
}

// OFFSET: 0x0050E3D0/0x1401572F0
static int32_t Script_GetDepth(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x40000) {
        return 0;
    }

    lua_pushnumber(L, frame->m_depth);
    return 1;
}

// OFFSET: 0x0050E430/0x140157350
static int32_t Script_GetEffectiveDepth(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_frameFlags & 0x40000) {
        return 0;
    }

    lua_pushnumber(L, frame->m_parentDepth + frame->m_depth);
    return 1;
}

// OFFSET: 0x0050E490/0x1401573C0
static int32_t Script_IgnoreDepth(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (lua_type(L, 2) != LUA_TBOOLEAN) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:IgnoreDepth(ignore)", name ? name : "<unnamed>");
        return 0;
    }

    frame->m_ignoreDepth = lua_toboolean(L, 2);
    return 0;
}

// PROVEN: 4eae8bcc IDENTICAL/NONE
// OFFSET: 0x0050E520/0x140157470
static int32_t Script_IsIgnoringDepth(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_ignoreDepth) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: ec30d630 IDENTICAL/IDENTICAL
// OFFSET: 0x0050E580/0x1401574E0
static int32_t Script_SetPropagateKeyboardInput(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (StringToBOOL(L, 2, 0)) {
        frame->SetFrameFlag(0x200000, 1);
    } else {
        frame->SetFrameFlag(0x200000, 0);
    }

    return 0;
}

// OFFSET: 0x0050E5E0/0x140157560
static int32_t Script_GetPropagateKeyboardInput(lua_State* L) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (!(frame->m_frameFlags & 0x200000) || frame->IsProtected()) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

// PROVEN: 14325063 IDENTICAL/NONE
// OFFSET: 0x0050E650
void CSimpleFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleFrame::RegisterScriptMethods);
}

// PROVEN: d8b12dd4 IDENTICAL/NONE
// OFFSET: 0x0050B000
void CSimpleFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleFrame::s_metatable);
    CSimpleFrame::s_metatable = LUA_NOREF;
}

// STATIC: %0x00BB8350
static FrameScript_Method s_ScriptMethods[] = {
    { "GetTitleRegion", &Script_GetTitleRegion },
    { "CreateTitleRegion", &Script_CreateTitleRegion },
    { "CreateTexture", &Script_CreateTexture },
    { "CreateFontString", &Script_CreateFontString },
    { "GetBoundsRect", &Script_GetBoundsRect },
    { "GetNumRegions", &Script_GetNumRegions },
    { "GetRegions", &Script_GetRegions },
    { "GetNumChildren", &Script_GetNumChildren },
    { "GetChildren", &Script_GetChildren },
    { "GetFrameStrata", &Script_GetFrameStrata },
    { "SetFrameStrata", &Script_SetFrameStrata },
    { "GetFrameLevel", &Script_GetFrameLevel },
    { "SetFrameLevel", &Script_SetFrameLevel },
    { "HasScript", &Script_HasScript },
    { "GetScript", &Script_GetScript },
    { "SetScript", &Script_SetScript },
    { "HookScript", &Script_HookScript },
    { "RegisterEvent", &Script_RegisterEvent },
    { "UnregisterEvent", &Script_UnregisterEvent },
    { "RegisterAllEvents", &Script_RegisterAllEvents },
    { "UnregisterAllEvents", &Script_UnregisterAllEvents },
    { "IsEventRegistered", &Script_IsEventRegistered },
    { "AllowAttributeChanges", &Script_AllowAttributeChanges },
    { "CanChangeAttribute", &Script_CanChangeAttribute },
    { "GetAttribute", &Script_GetAttribute },
    { "SetAttribute", &Script_SetAttribute },
    { "GetEffectiveScale", &Script_GetEffectiveScale },
    { "GetScale", &Script_GetScale },
    { "SetScale", &Script_SetScale },
    { "GetEffectiveAlpha", &Script_GetEffectiveAlpha },
    { "GetAlpha", &Script_GetAlpha },
    { "SetAlpha", &Script_SetAlpha },
    { "GetID", &Script_GetID },
    { "SetID", &Script_SetID },
    { "SetToplevel", &Script_SetToplevel },
    { "IsToplevel", &Script_IsToplevel },
    { "EnableDrawLayer", &Script_EnableDrawLayer },
    { "DisableDrawLayer", &Script_DisableDrawLayer },
    { "Show", &Script_Show },
    { "Hide", &Script_Hide },
    { "IsVisible", &Script_IsVisible },
    { "IsShown", &Script_IsShown },
    { "Raise", &Script_Raise },
    { "Lower", &Script_Lower },
    { "GetHitRectInsets", &Script_GetHitRectInsets },
    { "SetHitRectInsets", &Script_SetHitRectInsets },
    { "GetClampRectInsets", &Script_GetClampRectInsets },
    { "SetClampRectInsets", &Script_SetClampRectInsets },
    { "GetMinResize", &Script_GetMinResize },
    { "SetMinResize", &Script_SetMinResize },
    { "GetMaxResize", &Script_GetMaxResize },
    { "SetMaxResize", &Script_SetMaxResize },
    { "SetMovable", &Script_SetMovable },
    { "IsMovable", &Script_IsMovable },
    { "SetDontSavePosition", &Script_SetDontSavePosition },
    { "GetDontSavePosition", &Script_GetDontSavePosition },
    { "SetResizable", &Script_SetResizable },
    { "IsResizable", &Script_IsResizable },
    { "StartMoving", &Script_StartMoving },
    { "StartSizing", &Script_StartSizing },
    { "StopMovingOrSizing", &Script_StopMovingOrSizing },
    { "SetUserPlaced", &Script_SetUserPlaced },
    { "IsUserPlaced", &Script_IsUserPlaced },
    { "SetClampedToScreen", &Script_SetClampedToScreen },
    { "IsClampedToScreen", &Script_IsClampedToScreen },
    { "RegisterForDrag", &Script_RegisterForDrag },
    { "EnableKeyboard", &Script_EnableKeyboard },
    { "IsKeyboardEnabled", &Script_IsKeyboardEnabled },
    { "EnableMouse", &Script_EnableMouse },
    { "IsMouseEnabled", &Script_IsMouseEnabled },
    { "EnableMouseWheel", &Script_EnableMouseWheel },
    { "IsMouseWheelEnabled", &Script_IsMouseWheelEnabled },
    { "EnableJoystick", &Script_EnableJoystick },
    { "IsJoystickEnabled", &Script_IsJoystickEnabled },
    { "GetBackdrop", &Script_GetBackdrop },
    { "SetBackdrop", &Script_SetBackdrop },
    { "GetBackdropColor", &Script_GetBackdropColor },
    { "SetBackdropColor", &Script_SetBackdropColor },
    { "GetBackdropBorderColor", &Script_GetBackdropBorderColor },
    { "SetBackdropBorderColor", &Script_SetBackdropBorderColor },
    { "SetDepth", &Script_SetDepth },
    { "GetDepth", &Script_GetDepth },
    { "GetEffectiveDepth", &Script_GetEffectiveDepth },
    { "IgnoreDepth", &Script_IgnoreDepth },
    { "IsIgnoringDepth", &Script_IsIgnoringDepth },
    { "SetPropagateKeyboardInput", &Script_SetPropagateKeyboardInput },
    { "GetPropagateKeyboardInput", &Script_GetPropagateKeyboardInput },
};

// PROVEN: ce1d91d3 IDENTICAL/UNPROVEN
// OFFSET: 0x0050AFE0/0x140153420
void CSimpleFrame::RegisterScriptMethods(lua_State* L) {
    CScriptRegion::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
