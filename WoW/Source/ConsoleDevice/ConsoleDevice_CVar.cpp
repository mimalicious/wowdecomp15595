#include <cstdint>
#include <cstdio>
#include <cstring>
#include "ConsoleDevice.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"

// STATIC: 0x00D084EC
const char* s_colorBitsNames[8] = { "16", "24", "24", "30", "0", "0", "0", "0" };

// STATIC: 0x00D0850C/0x140BB99F0
const char* s_depthBitsNames[8] = { "0", "0", "0", "0", "16", "24", "24", "32" };

// STATIC: 0x00D0852C/0x140BB9A30
const char* s_apiNames[4] = { "OpenGL", "D3D9", "D3D11", "GLL" };

// STATIC: 0x00E288C0/0x140D104D0
CVar* s_cvGxStereoEnabled;

// STATIC: 0x00E288C8/0x140D104E0
CVar* s_cvGxRefresh;

// STATIC: 0x00E288CC/0x140D104E8
CVar* s_cvGxMaximize;

// STATIC: 0x00E288D0/0x140D104F0
CVar* s_cvGxMultisample;

// STATIC: 0x00E288D4/0x140D104F8
CVar* s_cvGxCursor;

// STATIC: 0x00E288D8/0x140D10500
CVar* s_cvGxStereoSeparation;

// STATIC: 0x00E288DC/0x140D10508
CVar* s_cvGxMultisampleQuality;

// STATIC: 0x00E288E0/0x140D10510
CVar* s_cvGxResolution;

// STATIC: 0x00E288E4/0x140D10518
CVar* s_cvHwDetect;

// STATIC: 0x00E288E8
CVar* s_cvGraphicsQuality;

// STATIC: 0x00E288EC/0x140D10528
CVar* s_cvGxOverride;

// STATIC: 0x00E288F0/0x140D10530
CVar* s_cvGxFixLag;

// STATIC: 0x00E2891C/0x140D10560
CVar* s_cvGxVSync;

// STATIC: 0x00E28920/0x140D10568
CVar* s_cvGxMonitor;

// STATIC: 0x00E28924/0x140D10570
CVar* s_cvVideoOptionsVersion;

// STATIC: 0x00E28928/0x140D10578
CVar* s_cvGxStereoConvergence;

// STATIC: 0x00E28958/0x140D105B0
CVar* s_cvGxTripleBuffer;

// STATIC: 0x00E2895C/0x140D105B8
CVar* s_cvGxDepthBits;

// STATIC: 0x00E28960/0x140D105C0
CVar* s_cvGxColorBits;

// STATIC: 0x00E28988/0x140D105F0
CVar* s_cvGxApi;

// STATIC: 0x00E2898C/0x140D105F8
CVar* s_cvGxAspect;

// STATIC: 0x00E28994
CVar* s_cvFixedFunction;

// STATIC: 0x00E28998/0x140D10608
CVar* s_cvGxWindow;

// STATIC: 0x00E2899C
CVar* s_cvWindowResizeLock;

// OFFSET: 0x006352D0/0x1402CB6D0
static bool CVGxRestartPendingCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x006352F0/0x1402CB6F0
static bool CVClientRestartPendingCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ConsoleWrite("set pending client restart", DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x00635310/0x1402CB710
static bool CVGxApiCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    for (uint32_t api = GxApi_OpenGl; api < 4; api++) {
        if (GxApiSupported(static_cast<EGxApi>(api)) && !_stricmp(value, s_apiNames[api])) {
            ConsoleWrite("GxApi set pending client restart", DEFAULT_COLOR);
            return true;
        }
    }

    if (!_stricmp(value, "d3d9ex")) {
        ConsoleWrite("GxApi set pending client restart", DEFAULT_COLOR);
        return true;
    }

    char message[1024] = "unsupported api, must be one of ";
    bool first = true;

    for (uint32_t api = GxApi_OpenGl; api < 4; api++) {
        if (!GxApiSupported(static_cast<EGxApi>(api))) {
            continue;
        }

        if (!first) {
            strcat(message, ", ");
        }

        strcat(message, "'");
        strcat(message, s_apiNames[api]);
        strcat(message, "'");
        first = false;
    }

    ConsoleWrite(message, DEFAULT_COLOR);

    return false;
}

// OFFSET: 0x00635530/0x1402CB9A0
static bool CVGxOverrideCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ConsoleGxOverride(value);
    return true;
}

// OFFSET: 0x00635550/0x1402CB9C0
static bool CVWindowResizeLockCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    OsInputSetWindowResizeLock(SStrToInt(value));
    return true;
}

// OFFSET: 0x00635570/0x1402CB9E0
void SetGxCVars(const CGxFormat& format) {
    EGxApi api = GxGetRequestedApi();
    char value[1024];

    s_cvGxColorBits->Set(s_colorBitsNames[format.m_colorFormat], true, false, false, true);
    s_cvGxDepthBits->Set(s_depthBitsNames[format.m_depthFormat], true, false, false, true);
    s_cvGxApi->Set(s_apiNames[api], true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_window);
    s_cvGxWindow->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%dx%d", format.m_size.x, format.m_size.y);
    s_cvGxResolution->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d/%d", format.m_refreshRate, format.m_refreshRateDenom);
    s_cvGxRefresh->Set(value, true, false, false, true);

    s_cvGxTripleBuffer->Set(format.m_backBufferCount > 1 ? "1" : "0", true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_monitor);
    s_cvGxMonitor->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_vsync);
    s_cvGxVSync->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_aspect);
    s_cvGxAspect->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_maximize);
    s_cvGxMaximize->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_hwCursor);
    s_cvGxCursor->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_multisampleCount);
    s_cvGxMultisample->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%f", format.m_multisampleQuality);
    s_cvGxMultisampleQuality->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_fixLag);
    s_cvGxFixLag->Set(value, true, false, false, true);
}

// OFFSET: 0x00635870/0x1402CBD70
static bool CVGxStereoConvergenceCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    g_theGxDevicePtr->StereoSetConvergence(SStrToFloat(value));
    return true;
}

// OFFSET: 0x006358A0/0x1402CBDA0
static bool CVGxStereoSeparationCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    g_theGxDevicePtr->StereoSetSeparation(SStrToFloat(value));
    return true;
}

// OFFSET: 0x006358D0/0x1402CBDD0
void RegisterStereoCVars() {
    s_cvGxStereoConvergence = CVar::Register("gxStereoConvergence", "Set stereoscopic rendering convergence depth", 1, "1", CVGxStereoConvergenceCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_cvGxStereoSeparation = CVar::Register("gxStereoSeparation", "Set stereoscopic rendering separation percentage", 1, "25", CVGxStereoSeparationCallback, CATEGORY_GRAPHICS, false, NULL, false);
}

// OFFSET: 0x00A3AA30/0x1404BDD20
static bool CVVideoOptionsVersionCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    return true;
}

// OFFSET: 0x00635CA0/0x1402CC260
void RegisterGxCVars() {
    char value[260];

    s_cvGxApi = CVar::Register("gxApi", "graphics api", 1, s_apiNames[GxDefaultApi()], CVGxApiCallback, CATEGORY_GRAPHICS, false, NULL, false);
    EGxApi api = GxDefaultApi();

    if (s_cvGxApi) {
        const char* name = s_cvGxApi->m_stringValue.m_str;

        for (uint32_t i = GxApi_OpenGl; i < 4; i++) {
            if (GxApiSupported(static_cast<EGxApi>(i)) && !_stricmp(name, s_apiNames[i])) {
                api = static_cast<EGxApi>(i);
                break;
            }
        }
    }

    if (CmdLineGetBool(CMD_OPENGL) && GxApiSupported(GxApi_OpenGl)) {
        api = GxApi_OpenGl;
    }

    if (CmdLineGetBool(CMD_D3D) && GxApiSupported(GxApi_D3d9)) {
        api = GxApi_D3d9;
    }

    if (CmdLineGetBool(CMD_D3D9EX) && GxApiSupported(GxApi_D3d11)) {
        api = GxApi_D3d11;
    }

    GxSetRequestedApi(api);

    s_cvFixedFunction = CVar::Register("fixedFunction", "Force fixed function rendering", 1, "0", CVClientRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_shaderModel = s_cvFixedFunction->m_intValue ? 0 : -1;

    bool window = s_locale == 4;
    SStrPrintf(value, sizeof(value), "%d", window);
    s_cvGxWindow = CVar::Register("gxWindow", "toggle fullscreen/window", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);
    SStrPrintf(value, sizeof(value), "%d", window);
    s_cvGxMaximize = CVar::Register("gxMaximize", "maximize game window", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    s_cvGxMonitor = CVar::Register("gxMonitor", "monitor", 1, "0", CVClientRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);
    uint32_t monitor = s_cvGxMonitor->m_intValue;

    if (monitor >= GxAdapterMonitorCount()) {
        monitor = 0;
    }

    s_monitor = monitor;
    GxAdapterMonitorModes(monitor, s_gxMonitorModes);
    SetDesktopFormat();

    SStrPrintf(value, sizeof(value), "%s", s_colorBitsNames[s_desktopFormat.m_colorFormat]);
    s_cvGxColorBits = CVar::Register("gxColorBits", "color bits", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%s", s_depthBitsNames[s_desktopFormat.m_depthFormat]);
    s_cvGxDepthBits = CVar::Register("gxDepthBits", "depth bits", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%dx%d", s_desktopFormat.m_size.x, s_desktopFormat.m_size.y);
    s_cvGxResolution = CVar::Register("gxResolution", "resolution", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d/%d", s_desktopFormat.m_refreshRate, s_desktopFormat.m_refreshRateDenom);
    s_cvGxRefresh = CVar::Register("gxRefresh", "refresh rate", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_backBufferCount > 1);
    s_cvGxTripleBuffer = CVar::Register("gxTripleBuffer", "triple buffer", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_vsync);
    s_cvGxVSync = CVar::Register("gxVSync", "vsync on or off", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_aspect);
    s_cvGxAspect = CVar::Register("gxAspect", "constrain window aspect", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_hwCursor);
    s_cvGxCursor = CVar::Register("gxCursor", "toggle hardware cursor", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_multisampleCount);
    s_cvGxMultisample = CVar::Register("gxMultisample", "multisample", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%f", s_desktopFormat.m_multisampleQuality);
    s_cvGxMultisampleQuality = CVar::Register("gxMultisampleQuality", "multisample quality", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_fixLag);
    s_cvGxFixLag = CVar::Register("gxFixLag", "prevent cursor lag", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    SStrPrintf(value, sizeof(value), "%d", s_desktopFormat.m_stereoEnabled);
    s_cvGxStereoEnabled = CVar::Register("gxStereoEnabled", "Enable stereoscopic rendering", 1, value, CVGxRestartPendingCallback, CATEGORY_GRAPHICS, false, NULL, false);

    s_cvGxOverride = CVar::Register("gxOverride", "gx overrides", 1, "", CVGxOverrideCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_cvVideoOptionsVersion = CVar::Register("videoOptionsVersion", "Video options version", 1, "0", CVVideoOptionsVersionCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_cvWindowResizeLock = CVar::Register("windowResizeLock", "prevent resizing in windowed mode", 1, "0", CVWindowResizeLockCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_cvGraphicsQuality = CVar::Register("graphicsQuality", "save for Graphics Quality Selection", 1, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);
}
