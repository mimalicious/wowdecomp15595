#include "SharedScripts.h"
#include <cstdint>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/Movie.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x00481540/0x1400A00E0
static int32_t Script_IsMovieLocal(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsMovieLocal(movieId)");
        return 0;
    }

    lua_pushboolean(L, MovieStreaming::IsMovieLocal(static_cast<int32_t>(lua_tointeger(L, 1))));
    return 1;
}

// OFFSET: 0x00481590/0x1400A0140
static int32_t Script_PreloadMovie(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PreloadMovie(movieId)");
        return 0;
    }

    MovieStreaming::PreloadMovie(static_cast<int32_t>(lua_tointeger(L, 1)));
    return 0;
}

// OFFSET: 0x004815D0/0x1400A0190
static int32_t Script_CancelPreloadingMovie(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CancelPreloadingMovie(movieId)");
        return 0;
    }

    MovieStreaming::CancelPreloadingMovie(static_cast<int32_t>(lua_tointeger(L, 1)));
    return 0;
}

// OFFSET: 0x00481610/0x1400A01E0
static int32_t Script_GetMovieDownloadProgress(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMovieDownloadProgress(movieId)");
        return 0;
    }

    int32_t movieID = static_cast<int32_t>(lua_tointeger(L, 1));
    int64_t local = 0;
    int64_t total = 0;
    lua_pushboolean(L, MovieStreaming::GetMovieDownloadProgress(movieID, local, total));
    lua_pushnumber(L, static_cast<lua_Number>(local));
    lua_pushnumber(L, static_cast<lua_Number>(total));
    return 3;
}

// OFFSET: 0x004816A0/0x1400A0280
static int32_t Script_IsMoviePlayable(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMovieDownloadProgress(movieId)");
        return 0;
    }

    lua_pushboolean(L, MovieStreaming::IsMoviePlayable(static_cast<int32_t>(lua_tointeger(L, 1))));
    return 1;
}

// OFFSET: 0x004816F0/0x1400A02E0
static int32_t Script_IsWindowsClient(lua_State* L) {
    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00481710/0x1400A0300
static int32_t Script_IsMacClient(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00481730/0x1400A0320
static int32_t Script_GetExistingLocales(lua_State* L) {
    int32_t count = 0;

    for (int32_t i = 0; i < 15; i++) {
        if (g_hasIsoLocale[i]) {
            lua_pushstring(L, g_localeNames[i]);
            count++;
        }
    }

    return count;
}

// OFFSET: 0x00481770/0x1400A0380
static int32_t Script_GetAvailableLocales(lua_State* L) {
    int32_t locales[15];
    int32_t count = 15;
    GetPortalLocales(locales, &count);

    for (int32_t i = 0; i < count; i++) {
        lua_pushstring(L, g_localeNames[locales[i]]);
    }

    return count;
}

// OFFSET: 0x004817D0/0x1400A0410
static int32_t Script_GetSecondsUntilParentalControlsKick(lua_State* L) {
    if (!ClientServices::Connection()) {
        lua_pushnil(L);
        return 1;
    }

    int32_t remaining = static_cast<int32_t>(ClientServices::Connection()->GetTimeRemaining());

    if (!(remaining >= 0)) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, remaining);
    }

    return 1;
}

// OFFSET: 0x004818C0/0x1400A0540
static int32_t Script_IsTrialAccount(lua_State* L) {
    CVar* converted = CVar::Lookup("converted");
    Login* login = ClientServices::LoginConnection();
    int32_t trial;

    if (login) {
        trial = login->IsTrialAccount();
    } else {
        if (!converted) {
            lua_pushnil(L);
            return 1;
        }

        trial = converted->m_intValue == 0;
    }

    if (!trial) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00481930/0x1400A05D0
static int32_t Script_CanUpgradeExpansion(lua_State* L) {
    lua_pushboolean(L, ClientServices::Connection()->m_activeExpansion < 3);
    return 1;
}

// OFFSET: 0x00481960/0x1400A0600
static int32_t Script_GetOSLocale(lua_State* L) {
    const char* locale;

    if (g_installLocaleVar && (locale = g_installLocaleVar->m_stringValue.m_str) != NULL && *locale) {
        lua_pushstring(L, locale);
        return 1;
    }

    lua_pushstring(L, g_localeNames[GetCurrentLocaleIndex()]);
    return 1;
}

// OFFSET: 0x004819B0/0x1400A0650
static int32_t Script_GetExpansionLevel(lua_State* L) {
    lua_pushnumber(L, ClientServices::Connection()->m_accountExpansion);
    return 1;
}

// OFFSET: 0x004819F0/0x1400A0680
static int32_t Script_GetAccountExpansionLevel(lua_State* L) {
    lua_pushnumber(L, ClientServices::Connection()->m_activeExpansion);
    return 1;
}

// STATIC: 0x00C9A510
static FrameScript_Method s_scriptFunctions[] = {
    { "IsMovieLocal", &Script_IsMovieLocal },
    { "PreloadMovie", &Script_PreloadMovie },
    { "CancelPreloadingMovie", &Script_CancelPreloadingMovie },
    { "GetMovieDownloadProgress", &Script_GetMovieDownloadProgress },
    { "IsMoviePlayable", &Script_IsMoviePlayable },
    { "IsTrialAccount", &Script_IsTrialAccount },
    { "CanUpgradeExpansion", &Script_CanUpgradeExpansion },
    { "IsWindowsClient", &Script_IsWindowsClient },
    { "IsMacClient", &Script_IsMacClient },
    { "IsLinuxClient", &Script_IsMacClient },
    { "Is64BitClient", &Script_IsMacClient },
    { "GetExistingLocales", &Script_GetExistingLocales },
    { "GetAvailableLocales", &Script_GetAvailableLocales },
    { "GetOSLocale", &Script_GetOSLocale },
    { "GetExpansionLevel", &Script_GetExpansionLevel },
    { "GetAccountExpansionLevel", &Script_GetAccountExpansionLevel },
    { "GetSecondsUntilParentalControlsKick", &Script_GetSecondsUntilParentalControlsKick },
};

// OFFSET: 0x00481830
void RegisterSharedScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00481860/0x1400A04B0
void UnregisterSharedScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
