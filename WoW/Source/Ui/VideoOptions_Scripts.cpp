#include "Engine/Source/Base/BlzAlgorithm.h"
#include "VideoOptions.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3iVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/ConsoleDevice/ConsoleDetect.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldParam.h"

// STATIC: 0x00BD37F8/0x1409C83F8
static const int32_t s_colorBits[CGxFormat::Formats_Last] = { 16, 24, 24, 30, 0, 0, 0, 0 };

// STATIC: 0x00BD3818/0x1409C8418
static const int32_t s_depthBits[CGxFormat::Formats_Last] = { 0, 0, 0, 0, 16, 24, 24, 32 };

// STATIC: 0x00D171E0/0x140BD1DB0
static uint32_t s_resolutionMonitor = 0xFFFFFFFF;

// STATIC: 0x00D171E4/0x140BD1DB4
static uint32_t s_refreshMonitor = 0xFFFFFFFF;

// STATIC: 0x00D171E8
static int32_t s_refreshHeight = -1;

// STATIC: 0x00D171EC
static int32_t s_refreshWidth = -1;

// STATIC: 0x00F76A80/0x140E6CB28
static int32_t s_selectedResolutionIndex;

// STATIC: 0x00F76A84/0x140E6CB30
static TSGrowableArray<C2iVector> s_resolutions;

// STATIC: 0x00F76A94/0x140E6CB48
static TSGrowableArray<C3iVector> s_multisampleFormats;

// STATIC: 0x00F76AA4/0x140E6CB60
static TSGrowableArray<C2iVector> s_refreshRates;

// OFFSET: 0x009F80B0/0x140795010
static bool CompareRefreshRates(const C2iVector& a, const C2iVector& b) {
    return a.x * b.y < b.x * a.y;
}

// OFFSET: 0x009F80D0/0x140795030
static int32_t Script_GetMonitorCount(lua_State* L) {
    lua_pushnumber(L, GxAdapterMonitorCount());
    return 1;
}

// OFFSET: 0x009F8110/0x140795060
static int32_t Script_GetMonitorName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    char name[128];
    int32_t monitor = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(monitor - 1) > GxAdapterMonitorCount() || !GxAdapterMonitorName(monitor - 1, name, sizeof(name))) {
        return 0;
    }

    lua_pushstring(L, name);
    return 1;
}

// OFFSET: 0x009F81A0/0x1407950F0
static int32_t Script_SetGamma(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGamma(value)");
        return 0;
    }

    CVar* gamma = CVar::Lookup("gamma");
    char value[16];
    SStrPrintf(value, sizeof(value), "%f", 1.0 - lua_tonumber(L, 1));
    gamma->Set(value, true, false, false, true);
    return 0;
}

// OFFSET: 0x009F8220/0x140795190
static uint32_t ValidateVideoOption(const char* name, int32_t value) {
    if (!_stricmp(name, "componentTextureLevel")) {
        return CCharacterComponent::ValidateComponentTextureLevel(value);
    }

    if (!_stricmp(name, "farClip")) {
        return World::ValidateFarClip(static_cast<float>(value));
    }

    if (!_stricmp(name, "groundEffectDensity")) {
        return World::ValidateDetailDoodadDensity(value);
    }

    if (!_stricmp(name, "groundEffectDist")) {
        return World::ValidateDetailDoodadDist(value);
    }

    if (!_stricmp(name, "shadowMode")) {
        return CShadowCache::ValidateShadowMode(static_cast<EShadowMode>(value));
    }

    if (!_stricmp(name, "shadowTextureSize")) {
        return CShadowCache::ValidateTextureSize(value);
    }

    if (!_stricmp(name, "sunShafts")) {
        return SunShafts::ValidateMode(value);
    }

    if (!_stricmp(name, "textureFilteringMode")) {
        return CWorldParam::ValidateTextureFilteringMode(value);
    }

    if (!_stricmp(name, "waterDetail")) {
        return Liquid::ValidateWaterDetail(value);
    }

    if (!_stricmp(name, "rippleDetail")) {
        return Liquid::ValidateRippleDetail(value);
    }

    if (!_stricmp(name, "reflectionMode")) {
        return Liquid::ValidateReflectionMode(value);
    }

    if (!_stricmp(name, "reflectionDownscale")) {
        return Liquid::ValidateReflectionDownscale(value);
    }

    return 0;
}

// OFFSET: 0x009F8380/0x140795360
static int32_t Script_GetToolTipInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    int32_t numCVars = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!lua_isnumber(L, 2)) {
        return 0;
    }

    int32_t numResults = static_cast<int32_t>(lua_tonumber(L, 2));
    const char* names[64];
    int32_t index = 3;

    for (int32_t i = 0; i < numCVars; i++) {
        if (!lua_isstring(L, index)) {
            return 0;
        }

        names[i] = lua_tolstring(L, index++, NULL);
    }

    for (int32_t result = 0; result < numResults; result++) {
        uint32_t flags = 0;

        for (int32_t i = 0; i < numCVars; i++) {
            if (!lua_isnumber(L, index)) {
                return 0;
            }

            int32_t value = static_cast<uint32_t>(lua_tonumber(L, index++));
            flags |= ValidateVideoOption(names[i], value);
        }

        lua_pushnumber(L, flags);
    }

    return numResults;
}

// OFFSET: 0x009F84F0/0x1407954F0
static int32_t Script_SetDefaultVideoOptions(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetDefaultVideoOptions(value)");
        return 0;
    }

    uint32_t page = static_cast<int32_t>(lua_tonumber(L, 1));

    if (page <= OPTION_PAGE_2) {
        ConsoleDeviceDefaults(static_cast<OPTION_PAGE>(page));
    }

    return 0;
}

// OFFSET: 0x009F8540/0x140795540
static int32_t Script_GetDefaultVideoOptions(lua_State* L) {
    const CWorldParam::DefaultSetting* setting = CWorldParam::GetCurrentDefaultSettings();
    int32_t count = 0;

    for (; setting->m_name; setting++) {
        lua_pushstring(L, setting->m_name);
        lua_pushnumber(L, setting->m_value);
        count += 2;
    }

    return count;
}

// OFFSET: 0x009F8590/0x1407955B0
static int32_t Script_GetVideoOptions(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetVideoOptions(value)");
        return 0;
    }

    uint32_t level = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (level >= 5) {
        return 0;
    }

    const CWorldParam::DefaultSetting* setting = CWorldParam::GetDefaultSettings(level);
    int32_t count = 0;

    for (; setting->m_name; setting++) {
        lua_pushstring(L, setting->m_name);
        lua_pushnumber(L, setting->m_value);
        count += 2;
    }

    return count;
}

// OFFSET: 0x009F8630/0x140795660
static int32_t Script_GetGraphicsAPIs(lua_State* L) {
    int32_t count = 0;

    for (int32_t api = 0; api < 4; api++) {
        if (GxApiSelectable(static_cast<EGxApi>(api))) {
            lua_pushstring(L, s_apiNames[api]);
            count++;
        }
    }

    return count;
}

// OFFSET: 0x009F8670/0x1407956C0
static int32_t Script_SetSelectedScreenResolutionIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedScreenResolutionIndex(integer index)");
        return 0;
    }

    s_selectedResolutionIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    return 0;
}

// OFFSET: 0x009F88D0/0x140795890
static int32_t Script_SetScreenResolution(lua_State* L) {
    int32_t width = 0;
    int32_t height = 0;

    if (lua_isnumber(L, 1)) {
        width = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    if (lua_isnumber(L, 2)) {
        height = static_cast<int32_t>(lua_tonumber(L, 2));
    }

    CVar* gxResolution = CVar::Lookup("gxResolution");

    if (gxResolution && gxResolution->m_stringValue.m_str) {
        int32_t currentHeight = 0;
        int32_t currentWidth = 0;
        char separator;
        sscanf(gxResolution->m_stringValue.m_str, "%d%c%d", &currentWidth, &separator, &currentHeight);

        if (currentWidth != width || currentHeight != height) {
            char value[32];
            SStrPrintf(value, sizeof(value), "%dx%d", width, height);
            gxResolution->Set(value, true, false, false, true);
        }
    }

    return 0;
}

// OFFSET: 0x009F89D0/0x140795990
static int32_t Script_SetupFullscreenScale(lua_State* L) {
    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        CSimpleFrame* frame = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!frame) {
            luaL_error(L, "SetupFullscreenScale(): Couldn't find 'this' in frame object");
            return 0;
        }

        if (!frame->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "SetupFullscreenScale(): Wrong object type, expected frame");
            return 0;
        }

        float compensation = CoordinateGetAspectCompensation();

        if (compensation < 1.0f) {
            frame->SetFrameScale(compensation, false);
        } else {
            frame->SetFrameScale(1.0f, false);
        }

        return 0;
    }

    luaL_error(L, "Usage: SetupFullscreenScale(frame)");
    return 0;
}

// OFFSET: 0x009F8AB0/0x140795AA0
static int32_t Script_GetMaxMultisampleFormatOnCvar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetMaxMultisampleFormatOnCvar(\"cvar\", cvarValue [, monitor)");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    uint32_t value = lua_isnumber(L, 2) ? static_cast<int32_t>(lua_tonumber(L, 2)) : 0;

    if (lua_isnumber(L, 3)) {
        lua_tonumber(L, 3);
    }

    uint32_t count = s_multisampleFormats.Count();
    bool needsDepthCache;

    if (_stricmp(name, "waterDetail")) {
        needsDepthCache = !_stricmp(name, "sunShafts") && SunShafts::ModeNeedsDepthCache(value);
    } else {
        needsDepthCache = Liquid::WaterDetailNeedsDepthCache(value) != 0;
    }

    if (needsDepthCache && !g_theGxDevicePtr->Caps().unk_0314) {
        count = 1;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x009F8BE0/0x140795BE0
static int32_t Script_GetVideoCaps(lua_State* L) {
    CGxCaps& caps = g_theGxDevicePtr->Caps();

    if (caps.m_texFilterAnisotropic) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (caps.m_shaderTargets[GxSh_Pixel]) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (caps.m_shaderTargets[GxSh_Vertex]) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (caps.m_texFilterTrilinear) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, caps.unk_001C);

    if (caps.m_maxTexAnisotropy) {
        lua_pushnumber(L, caps.m_maxTexAnisotropy);
    } else {
        lua_pushnil(L);
    }

    if (caps.m_hwCursor) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 7;
}

// OFFSET: 0x009F8D10/0x140795CE0
static int32_t Script_GetGamma(lua_State* L) {
    CVar* gamma = CVar::Lookup("gamma");
    lua_pushnumber(L, 1.0 - gamma->m_floatValue);
    return 1;
}

// OFFSET: 0x009F8D40/0x140795D20
static int32_t Script_IsStereoVideoAvailable(lua_State* L) {
    if (g_theGxDevicePtr->Caps().m_stereoAvailable) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009F8E80/0x140795E00
static int32_t Script_GetMonitorAspectRatio(lua_State* L) {
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        monitor = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    }

    CGxMonitorMode mode;
    mode.m_size.x = 0;
    mode.m_size.y = 0;

    if (GxAdapterDesktopMode(monitor, mode)) {
        lua_pushnumber(L, static_cast<double>(mode.m_size.x) / static_cast<double>(mode.m_size.y));
        return 1;
    }

    return 0;
}

// OFFSET: 0x009F9100/0x1407960F0
static void SetupFormats(uint32_t monitor) {
    if (s_resolutionMonitor == monitor) {
        return;
    }

    s_resolutionMonitor = monitor;
    ConsoleDetectGetResolutions(s_resolutions, monitor, 1);
    s_multisampleFormats.SetCount(0);

    const CGxFormat& format = g_theGxDevicePtr->DeviceFormat();
    C3iVector current;
    current.x = s_colorBits[format.m_colorFormat];
    current.y = s_depthBits[format.m_depthFormat];
    current.z = format.m_multisampleCount;

    TSGrowableArray<CGxFormat> formats;
    GxAdapterFormats(formats);

    bool foundCurrent = false;

    for (uint32_t i = 0; i < formats.Count(); i++) {
        C3iVector candidate;
        candidate.x = s_colorBits[formats[i].m_colorFormat];
        candidate.y = s_depthBits[formats[i].m_depthFormat];
        candidate.z = formats[i].m_multisampleCount;

        if (candidate.x != current.x || candidate.y != current.y) {
            continue;
        }

        if (current.z == candidate.z) {
            foundCurrent = true;
        }

        uint32_t j;

        for (j = 0; j < s_multisampleFormats.Count(); j++) {
            C3iVector& existing = s_multisampleFormats[j];

            if (existing.x == candidate.x && existing.y == candidate.y && existing.z == candidate.z) {
                break;
            }
        }

        if (j >= s_multisampleFormats.Count()) {
            s_multisampleFormats.New(candidate);
        }
    }

    if (!foundCurrent) {
        s_multisampleFormats.New(current);
    }
}

// OFFSET: 0x009F9260/0x140796430
static int32_t Script_GetScreenResolutions(lua_State* L) {
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        monitor = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    }

    SetupFormats(monitor);

    uint32_t count = s_resolutions.Count();
    lua_checkstack(L, count);

    for (uint32_t i = 0; i < count; i++) {
        char value[64];
        SStrPrintf(value, sizeof(value), "%dx%d", s_resolutions[i].x, s_resolutions[i].y);
        lua_pushstring(L, value);
    }

    return count;
}

// OFFSET: 0x009F9300/0x1407964F0
static int32_t Script_GetCurrentResolution(lua_State* L) {
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        monitor = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    }

    SetupFormats(monitor);

    CVar* gxResolution = CVar::Lookup("gxResolution");

    if (gxResolution && gxResolution->m_stringValue.m_str) {
        int32_t width = 0;
        int32_t height = 0;
        char separator;
        sscanf(gxResolution->m_stringValue.m_str, "%d%c%d", &width, &separator, &height);

        for (uint32_t i = 0; i < s_resolutions.Count(); i++) {
            if (s_resolutions[i].x == width && s_resolutions[i].y == height) {
                lua_pushnumber(L, i + 1);
                return 1;
            }
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x009F93E0/0x1407965D0
static int32_t Script_GetMultisampleFormats(lua_State* L) {
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        monitor = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    }

    SetupFormats(monitor);
    lua_checkstack(L, 3 * s_multisampleFormats.Count());

    for (uint32_t i = 0; i < s_multisampleFormats.Count(); i++) {
        lua_pushnumber(L, s_multisampleFormats[i].x);
        lua_pushnumber(L, s_multisampleFormats[i].y);
        lua_pushnumber(L, s_multisampleFormats[i].z);
    }

    return 3 * s_multisampleFormats.Count();
}

// OFFSET: 0x009F94B0/0x1407966A0
static int32_t Script_GetCurrentMultisampleFormat(lua_State* L) {
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        monitor = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    }

    SetupFormats(monitor);

    int32_t colorBits = 0;
    int32_t depthBits = 0;
    int32_t multisample = 0;
    CVar* var = CVar::Lookup("gxColorBits");

    if (var) {
        colorBits = var->m_intValue;
    }

    var = CVar::Lookup("gxDepthBits");

    if (var) {
        depthBits = var->m_intValue;
    }

    var = CVar::Lookup("gxMultisample");

    if (var) {
        multisample = var->m_intValue;
    }

    for (uint32_t i = 0; i < s_multisampleFormats.Count(); i++) {
        C3iVector& format = s_multisampleFormats[i];

        if (colorBits == format.x && depthBits == format.y && multisample == format.z) {
            lua_pushnumber(L, i + 1);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x009F95C0/0x1407967A0
static int32_t Script_SetMultisampleFormat(lua_State* L) {
    uint32_t monitor = 0;
    uint32_t index = 0;

    if (lua_isnumber(L, 1)) {
        uint32_t count = s_multisampleFormats.Count();

        if (static_cast<int32_t>(count) >= static_cast<uint32_t>(lua_tonumber(L, 1)) - 1) {
            index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        } else {
            index = count;
        }
    }

    if (lua_isnumber(L, 2)) {
        monitor = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    }

    SetupFormats(monitor);

    CVar* colorBits = CVar::Lookup("gxColorBits");
    CVar* depthBits = CVar::Lookup("gxDepthBits");
    CVar* multisample = CVar::Lookup("gxMultisample");

    if (colorBits && depthBits && multisample) {
        C3iVector& format = s_multisampleFormats[index];

        if (colorBits->m_intValue != format.x || depthBits->m_intValue != format.y || multisample->m_intValue != format.z) {
            char value[32];
            SStrPrintf(value, sizeof(value), "%d", format.x);
            colorBits->Set(value, true, false, false, true);
            SStrPrintf(value, sizeof(value), "%d", format.y);
            depthBits->Set(value, true, false, false, true);
            SStrPrintf(value, sizeof(value), "%d", format.z);
            multisample->Set(value, true, false, false, true);
        }
    }

    return 0;
}

// OFFSET: 0x009F9900/0x140796BE0
static void SetupRefreshRates(uint32_t monitor, int32_t width, int32_t height) {
    if (s_refreshWidth == width && s_refreshHeight == height && s_refreshMonitor == monitor) {
        return;
    }

    s_refreshWidth = width;
    s_refreshHeight = height;
    s_refreshMonitor = monitor;
    s_refreshRates.SetCount(0);

    TSGrowableArray<CGxMonitorMode> modes;
    GxAdapterMonitorModes(monitor, modes);

    for (uint32_t i = 0; i < modes.Count(); i++) {
        CGxMonitorMode& mode = modes[i];

        if ((mode.m_size.x != width || mode.m_size.y != height) && (mode.m_size.x != height || mode.m_size.y != width)) {
            continue;
        }

        C2iVector rate = { static_cast<int32_t>(mode.m_refreshRate), static_cast<int32_t>(mode.m_refreshRateDenom) };
        uint32_t j;

        for (j = 0; j < s_refreshRates.Count(); j++) {
            if (s_refreshRates[j].x == rate.x && s_refreshRates[j].y == rate.y) {
                break;
            }
        }

        if (j >= s_refreshRates.Count()) {
            s_refreshRates.New(rate);
        }
    }

    if (!s_refreshRates.Count()) {
        for (uint32_t i = 0; i < modes.Count(); i++) {
            C2iVector rate = { static_cast<int32_t>(modes[i].m_refreshRate), static_cast<int32_t>(modes[i].m_refreshRateDenom) };
            uint32_t j;

            for (j = 0; j < s_refreshRates.Count(); j++) {
                if (s_refreshRates[j].x == rate.x && s_refreshRates[j].y == rate.y) {
                    break;
                }
            }

            if (j >= s_refreshRates.Count()) {
                s_refreshRates.New(rate);
            }
        }
    }

    if (s_refreshRates.Count()) {
        blz::sort(s_refreshRates.Ptr(), s_refreshRates.Ptr() + s_refreshRates.Count(), CompareRefreshRates);
    }
}

// OFFSET: 0x009F9BF0/0x140797110
static int32_t Script_GetRefreshRates(lua_State* L) {
    int32_t width = 0;
    int32_t height = 0;
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        width = static_cast<uint32_t>(lua_tonumber(L, 1));
    }

    if (lua_isnumber(L, 2)) {
        height = static_cast<uint32_t>(lua_tonumber(L, 2));
    }

    if (lua_isnumber(L, 3)) {
        monitor = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    }

    SetupRefreshRates(monitor, width, height);
    lua_checkstack(L, 2 * s_refreshRates.Count());

    for (uint32_t i = 0; i < s_refreshRates.Count(); i++) {
        lua_pushnumber(L, s_refreshRates[i].x);
        lua_pushnumber(L, s_refreshRates[i].y);
    }

    return 2 * s_refreshRates.Count();
}

// OFFSET: 0x009F9D20/0x140797230
static int32_t Script_GetCurrentRefresh(lua_State* L) {
    int32_t width = 0;
    int32_t height = 0;
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        width = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    if (lua_isnumber(L, 2)) {
        height = static_cast<int32_t>(lua_tonumber(L, 2));
    }

    if (lua_isnumber(L, 3)) {
        monitor = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;
    }

    SetupRefreshRates(monitor, width, height);

    CVar* gxRefresh = CVar::Lookup("gxRefresh");

    if (gxRefresh && gxRefresh->m_stringValue.m_str) {
        int32_t rate = 0;
        int32_t denom = 0;
        char separator;
        sscanf(gxRefresh->m_stringValue.m_str, "%d%c%d", &rate, &separator, &denom);

        for (uint32_t i = 0; i < s_refreshRates.Count(); i++) {
            if (s_refreshRates[i].x == rate && s_refreshRates[i].y == denom) {
                lua_pushnumber(L, i + 1);
                return 1;
            }
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x009F9ED0/0x1407973A0
static int32_t Script_SetRefresh(lua_State* L) {
    uint32_t index = 0;
    int32_t width = 0;
    int32_t height = 0;
    uint32_t monitor = 0;

    if (lua_isnumber(L, 1)) {
        uint32_t count = s_multisampleFormats.Count();

        if (static_cast<int32_t>(count) >= static_cast<uint32_t>(lua_tonumber(L, 1)) - 1) {
            index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        } else {
            index = count;
        }
    }

    if (lua_isnumber(L, 2)) {
        width = static_cast<uint32_t>(lua_tonumber(L, 2));
    }

    if (lua_isnumber(L, 3)) {
        height = static_cast<uint32_t>(lua_tonumber(L, 3));
    }

    if (lua_isnumber(L, 4)) {
        monitor = static_cast<uint32_t>(lua_tonumber(L, 4)) - 1;
    }

    SetupRefreshRates(monitor, width, height);

    CVar* gxRefresh = CVar::Lookup("gxRefresh");

    if (gxRefresh) {
        char value[32];
        SStrPrintf(value, sizeof(value), "%d/%d", s_refreshRates[index].x, s_refreshRates[index].y);
        gxRefresh->Set(value, true, false, false, true);
    }

    return 0;
}

// STATIC: 0x00D17108/0x140BD1C10
static FrameScript_Method s_scriptFunctions[] = {
    { "GetScreenResolutions", &Script_GetScreenResolutions },
    { "GetCurrentResolution", &Script_GetCurrentResolution },
    { "SetScreenResolution", &Script_SetScreenResolution },
    { "GetRefreshRates", &Script_GetRefreshRates },
    { "GetCurrentRefresh", &Script_GetCurrentRefresh },
    { "SetRefresh", &Script_SetRefresh },
    { "SetupFullscreenScale", &Script_SetupFullscreenScale },
    { "GetMultisampleFormats", &Script_GetMultisampleFormats },
    { "GetCurrentMultisampleFormat", &Script_GetCurrentMultisampleFormat },
    { "SetMultisampleFormat", &Script_SetMultisampleFormat },
    { "GetMaxMultisampleFormatOnCvar", &Script_GetMaxMultisampleFormatOnCvar },
    { "GetVideoCaps", &Script_GetVideoCaps },
    { "GetGamma", &Script_GetGamma },
    { "SetGamma", &Script_SetGamma },
    { "SetSelectedScreenResolutionIndex", &Script_SetSelectedScreenResolutionIndex },
    { "IsStereoVideoAvailable", &Script_IsStereoVideoAvailable },
    { "GetMonitorCount", &Script_GetMonitorCount },
    { "GetMonitorName", &Script_GetMonitorName },
    { "GetMonitorAspectRatio", &Script_GetMonitorAspectRatio },
    { "GetToolTipInfo", &Script_GetToolTipInfo },
    { "SetDefaultVideoOptions", &Script_SetDefaultVideoOptions },
    { "GetDefaultVideoOptions", &Script_GetDefaultVideoOptions },
    { "GetVideoOptions", &Script_GetVideoOptions },
    { "GetGraphicsAPIs", &Script_GetGraphicsAPIs },
};

// OFFSET: 0x009F86E0/0x140795710
void CGVideoOptions::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009F8710/0x140795760
void CGVideoOptions::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
