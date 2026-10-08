#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// INLINED
void CSimpleMovieFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleMovieFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleMovieFrame::RegisterScriptMethods);
}

// INLINED
void CSimpleMovieFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleMovieFrame::s_metatable);
    CSimpleMovieFrame::s_metatable = LUA_NOREF;
}

// OFFSET: %0x0054B390
static int32_t Script_StartMovie(lua_State* L) {
    CSimpleMovieFrame* movieFrame = static_cast<CSimpleMovieFrame*>(FrameScript_GetObjectThis(L, CSimpleMovieFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = movieFrame->GetName();
        luaL_error(L, "Usage: %s:StartMovie(movieID)", name ? name : "<unnamed>");
    }

    int32_t result = movieFrame->StartMovie(lua_tointeger(L, 2));
    lua_pushboolean(L, result == 0);
    lua_pushnumber(L, result);
    return 2;
}

// OFFSET: %0x0054B430
static int32_t Script_StopMovie(lua_State* L) {
    CSimpleMovieFrame* movieFrame = static_cast<CSimpleMovieFrame*>(FrameScript_GetObjectThis(L, CSimpleMovieFrame::GetObjectType()));

    movieFrame->StopMovie();
    return 0;
}

// PROVEN: 6c0bbd20 IDENTICAL/NONE
// OFFSET: 0x0053B760/0x14018F570
static int32_t Script_EnableSubtitles(lua_State* L) {
    CSimpleMovieFrame* movieFrame = static_cast<CSimpleMovieFrame*>(FrameScript_GetObjectThis(L, CSimpleMovieFrame::GetObjectType()));

    movieFrame->m_subtitlesEnabled = StringToBOOL(L, 2, 1);
    return 0;
}

// STATIC: %0x00BB9924
static FrameScript_Method s_ScriptMethods[] = {
    { "StartMovie", &Script_StartMovie },
    { "StopMovie", &Script_StopMovie },
    { "EnableSubtitles", &Script_EnableSubtitles },
};

// PROVEN: 2032fcc3 IDENTICAL/NONE
// OFFSET: 0x0053B630
void CSimpleMovieFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
