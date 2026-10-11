#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ConsoleDevice.h"
#include "ConsoleDetect.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Hardware.h"
#include "WoW/Source/LiquidSystem/Liquid.h"

// STATIC: 0x00BD37D8
static const uint32_t s_formatToBpp[] = { 16, 24, 24, 32, 16, 24, 24, 32 };

// STATIC: 0x00DD8A10
static int32_t s_consoleAccessEnabled;

// STATIC: 0x00E288C4
int32_t s_shaderModel;

// STATIC: 0x00E288F4
static int32_t s_consoleGxOverrideSet[GxOverrides_Last];

// STATIC: 0x00E28918
DefaultSettings s_defaults;

// STATIC: 0x00E2892C
Hardware s_hardware;

// STATIC: 0x00E28964
static uint32_t s_consoleGxOverrideVal[GxOverrides_Last];

// STATIC: 0x00E28990
uint32_t s_monitor;

// STATIC: 0x00E289A0
bool s_gxRestarting;

// STATIC: 0x00E289A1
static bool s_hardwareDetected;

// STATIC: 0x00E289A2
static bool s_hwChanged;

// STATIC: 0x00E289A3
static bool s_hwDetect;

// STATIC: 0x00E289A4
static int32_t s_defaultsApplied;

// STATIC: 0x00E289A8
char s_windowTitle[256];

static const C2iVector s_size800x600 = { 800, 600 };

static const C2iVector s_size640x480 = { 640, 480 };

// STATIC: 0x00E28AB0
CGxFormat s_desktopFormat(false, s_size800x600, CGxFormat::Fmt_ArgbX888, CGxFormat::Fmt_Ds24X, 60, 1, 1, true, false, true, true, 0, 0);

// STATIC: 0x00E28B08
TSGrowableArray<CGxMonitorMode> s_gxMonitorModes;

// STATIC: 0x00E28B18
static TSGrowableArray<void (*)(OPTION_PAGE)> s_defaultCallbacks;

// STATIC: 0x00E28B38
CGxFormat s_fallbackFormat(false, s_size640x480, CGxFormat::Fmt_Rgb565, CGxFormat::Fmt_Ds160, 60, 1, 1, true, false, true, true, 0, 0);

// STATIC: 0x00E28B90
CGxFormat s_lastGoodFormat;

// OFFSET: 0x0060E270/0x140298000
void ConsoleAccessSetEnabled(int32_t enable) {
    s_consoleAccessEnabled = enable;
}

// INLINED
int32_t IsConsoleEnabled() {
    return s_consoleAccessEnabled;
}

// OFFSET: 0x00635470/0x1402CB8A0
void ConsoleGxOverride(const char* list) {
    while (*list) {
        char key[256];
        char value[256];
        SStrTokenize(&list, key, sizeof(key), " ,", NULL);
        SStrTokenize(&list, value, sizeof(value), " ;", NULL);

        if (!key[0] || !value[0]) {
            continue;
        }

        uint32_t override = atoi(key);
        uint32_t overrideValue = atoi(value);

        if (override && override < GxOverrides_Last) {
            s_consoleGxOverrideSet[override] = 1;
            s_consoleGxOverrideVal[override] = overrideValue;
        }
    }
}

// OFFSET: 0x00635930/0x1402CBE80
Hardware* ConsoleGetHardware() {
    return s_hardwareDetected ? &s_hardware : NULL;
}

// PROVEN: b3fc94d1 IDENTICAL/NONE
// OFFSET: 0x00635950/0x1402CBEA0
int32_t ConsoleDeviceHardwareChanged() {
    return s_hwDetect || s_hwChanged;
}

// OFFSET: 0x00635970/0x1402CBEC0
DefaultSettings* ConsoleDeviceGetDefaultSettings() {
    return &s_defaults;
}

// OFFSET: 0x00635980/0x1402CBEE0
static int32_t CheckDeviceCaps() {
    CGxCaps& caps = g_theGxDevicePtr->Caps();

    if (caps.m_numTmus < 2) {
        GxLog("ConsoleDeviceInitialize(): output device does not have dual TMUs!");
        return 0;
    }

    if (caps.m_numStreams < 1) {
        GxLog("ConsoleDeviceInitialize(): output device has 0 streams");
        return 0;
    }

    if ((caps.m_texFmt[GxTex_Abgr8888] && caps.m_rttFormat[GxTex_Abgr8888]) || (caps.m_texFmt[GxTex_Argb8888] && caps.m_rttFormat[GxTex_Argb8888]) || (caps.m_texFmt[GxTex_Rgb565] && caps.m_rttFormat[GxTex_Rgb565])) {
        return 1;
    }

    GxLog("ConsoleDeviceInitialize(): output device does not have suitable texture & rt format support");
    return 0;
}

// OFFSET: 0x00635A10/0x1402CBF80
void DisplayChangeCallback(uint32_t monitor) {
    if (!s_gxRestarting) {
        s_monitor = monitor;
    }
}

// OFFSET: 0x00635A30/0x1402CBFA0
void ConsoleDeviceDestroy() {
    g_theGxDevicePtr->RemoveDisplayChangeCallback(DisplayChangeCallback);
    GxDevDestroy();
    GxShutDown();
    GxLogClose();
    ConsoleDetectDestroy();
}

// OFFSET: 0x00635B20/0x1402CC0C0
void SetDesktopFormat() {
    CGxMonitorMode desktop;
    desktop.m_size.x = 0;
    desktop.m_size.y = 0;

    if (!GxAdapterDesktopMode(s_monitor, desktop)) {
        return;
    }

    s_desktopFormat.m_size = desktop.m_size;
    s_desktopFormat.m_refreshRateDenom = desktop.m_refreshRateDenom;
    s_desktopFormat.m_monitor = s_monitor;
    s_desktopFormat.m_refreshRate = desktop.m_refreshRate;
    s_desktopFormat.m_scanlineOrdering = desktop.m_scanlineOrdering;
    s_desktopFormat.m_scaling = desktop.m_scaling;
    s_desktopFormat.m_colorFormat = CGxFormat::Fmt_ArgbX888;

    uint16_t vendorID = 0xFFFF;
    uint16_t deviceID = 0xFFFF;
    uint32_t driverVersionHi = 0;
    uint32_t driverVersionLo = 0;

    if (!GxAdapterID(vendorID, deviceID, driverVersionHi, driverVersionLo) || vendorID != 0x8086) {
        return;
    }

    int32_t maxPixels = 4 * desktop.m_size.y / 3 != desktop.m_size.x ? 1049088 : 1228800;

    if (desktop.m_size.x * desktop.m_size.y <= maxPixels) {
        return;
    }

    uint32_t best = static_cast<uint32_t>(-1);
    int32_t bestPixels = 0;

    for (uint32_t i = 0; i < s_gxMonitorModes.Count(); i++) {
        CGxMonitorMode& mode = s_gxMonitorModes[i];
        int32_t pixels = mode.m_size.y * mode.m_size.x;

        if (desktop.m_size.y * mode.m_size.x == desktop.m_size.x * mode.m_size.y && pixels <= maxPixels && pixels > bestPixels) {
            best = i;
            bestPixels = pixels;
        }
    }

    if (best < s_gxMonitorModes.Count()) {
        s_desktopFormat.m_size.x = s_gxMonitorModes[best].m_size.x;
        s_desktopFormat.m_size.y = s_gxMonitorModes[best].m_size.y;
    }
}

// OFFSET: 0x006362A0/0x1402CCA30
void ValidateFormatMonitor(CGxFormat& format, bool log) {
    static C2iVector s_resolutions[] = {
        { 1600, 1200 },
        { 1280, 1024 },
        { 1280, 960 },
        { 1152, 864 },
        { 1024, 768 },
        { 800, 600 },
        { 640, 480 },
    };

    uint32_t bpp = s_formatToBpp[format.m_colorFormat];

    if (bpp == 24) {
        bpp = 32;
    }

    CGxMonitorMode desktop;

    if (format.m_window) {
        desktop.m_size.x = 0;
        desktop.m_size.y = 0;

        if (GxAdapterDesktopMode(format.m_monitor, desktop) && desktop.m_size.x >= format.m_size.x && desktop.m_size.y >= format.m_size.y) {
            goto refresh;
        }
    }

    for (uint32_t i = 0; i < s_gxMonitorModes.Count(); i++) {
        if (format.m_size.x == s_gxMonitorModes[i].m_size.x && format.m_size.y == s_gxMonitorModes[i].m_size.y) {
            goto refresh;
        }
    }

    for (uint32_t i = 0; i < sizeof(s_resolutions) / sizeof(s_resolutions[0]); i++) {
        if (s_resolutions[i].x <= format.m_size.x && s_resolutions[i].y <= format.m_size.y) {
            format.m_size.x = s_resolutions[i].x;
            format.m_size.y = s_resolutions[i].y;
            break;
        }
    }

refresh:
    uint32_t minRate = 9999;
    uint32_t refreshRate = 9999;
    uint32_t refreshRateDenom = 1;

    for (uint32_t i = 0; i < s_gxMonitorModes.Count(); i++) {
        CGxMonitorMode& mode = s_gxMonitorModes[i];

        if (format.m_size.x == mode.m_size.x && format.m_size.y == mode.m_size.y && bpp == mode.m_bpp) {
            uint32_t rate = mode.m_refreshRate / mode.m_refreshRateDenom;

            if (rate < minRate) {
                refreshRate = mode.m_refreshRate;
                refreshRateDenom = mode.m_refreshRateDenom;
                minRate = rate;
            }

            if (mode.m_refreshRateDenom * format.m_refreshRate == mode.m_refreshRate * format.m_refreshRateDenom) {
                return;
            }
        }
    }

    if (minRate == 9999) {
        refreshRate = 60;
        refreshRateDenom = 1;
        GxLog("ValidateFormatMonitor(): unable to find monitor refresh");
    }

    if (log) {
        GxLog("ValidateFormatMonitor(): invalid refresh rate %.1f, set to %d", static_cast<double>(format.m_refreshRate) / static_cast<double>(format.m_refreshRateDenom), refreshRate);
    }

    format.m_refreshRate = refreshRate;
    format.m_refreshRateDenom = refreshRateDenom;
}

// OFFSET: 0x006364F0/0x1402CCCB0
int32_t SetDeviceFormat(CGxFormat& format) {
    EGxApi api = GxGetRequestedApi();
    CGxFormat tryFormat = format;

    if (GxDevExists()) {
        if (api == g_theGxDevicePtr->DeviceApi()) {
            return g_theGxDevicePtr->DeviceSetFormat(format);
        }

        GxDevDestroy();
    }

    while (true) {
        bool triedDesktop = false;

        while (true) {
            ValidateFormatMonitor(tryFormat, true);

            if (CreateGxDevice(api, OsWindowProc, tryFormat)) {
                format = tryFormat;
                s_lastGoodFormat = tryFormat;
                return 1;
            }

            if (tryFormat.m_multisampleCount > 1) {
                if (tryFormat.m_multisampleCount - 2 <= 1) {
                    tryFormat.m_multisampleCount = 1;
                } else {
                    tryFormat.m_multisampleCount -= 2;
                }

                continue;
            }

            if (tryFormat.m_colorFormat > CGxFormat::Fmt_Rgb565) {
                if (tryFormat.m_depthFormat <= CGxFormat::Fmt_Ds160) {
                    CGxFormat::Format colorFormat = tryFormat.m_colorFormat;
                    tryFormat.m_colorFormat = static_cast<CGxFormat::Format>(colorFormat - 1);
                    tryFormat.m_depthFormat = colorFormat != CGxFormat::Fmt_ArgbX888 ? CGxFormat::Fmt_Ds320 : CGxFormat::Fmt_Ds160;
                } else {
                    tryFormat.m_depthFormat = static_cast<CGxFormat::Format>(tryFormat.m_depthFormat - 1);
                }

                continue;
            }

            if (triedDesktop) {
                break;
            }

            tryFormat = s_desktopFormat;
            triedDesktop = true;
        }

        if (api != GxApi_D3d11) {
            return 0;
        }

        api = GxApi_D3d9;
        GxSetRequestedApi(GxApi_D3d9);
        tryFormat = format;
    }
}

// OFFSET: 0x00636680/0x1402CCE40
bool LoadFormatFromCVars(CGxFormat& format) {
    format = s_desktopFormat;
    format.m_monitor = s_monitor;
    format.m_window = SStrToInt(s_cvGxWindow->m_stringValue.m_str) != 0;

    int32_t width = -1;
    int32_t height = -1;
    char separator;
    sscanf(s_cvGxResolution->m_stringValue.m_str, "%d%c%d", &width, &separator, &height);

    if (format.m_window) {
        format.m_size.x = width;
        format.m_size.y = height;
    } else {
        TSGrowableArray<C2iVector> resolutions;
        ConsoleDetectGetResolutions(resolutions, format.m_monitor, 0);

        uint32_t i;

        for (i = 0; i < resolutions.Count(); i++) {
            if (width == resolutions[i].x && height == resolutions[i].y) {
                break;
            }
        }

        if (i == resolutions.Count()) {
            char message[256] = "invalid resolution, must be one of ";

            for (i = 0; i < resolutions.Count(); i++) {
                if (i) {
                    strcat(message, ", ");
                }

                if (strlen(message) > 100) {
                    ConsoleWrite(message, DEFAULT_COLOR);
                    message[0] = 0;
                }

                char size[32];
                SStrPrintf(size, sizeof(size), "%dx%d", resolutions[i].x, resolutions[i].y);
                strcat(message, size);
            }

            ConsoleWrite(message, DEFAULT_COLOR);
            return false;
        }

        format.m_size.x = width;
        format.m_size.y = height;
    }

    switch (SStrToInt(s_cvGxColorBits->m_stringValue.m_str)) {
        case 16:
            format.m_colorFormat = CGxFormat::Fmt_Rgb565;
            break;

        case 24:
            format.m_colorFormat = CGxFormat::Fmt_ArgbX888;
            break;

        case 30:
            format.m_colorFormat = CGxFormat::Fmt_Argb2101010;
            break;

        default:
            ConsoleWrite("Color bits must be 16, 24, or 30", DEFAULT_COLOR);
            return false;
    }

    switch (SStrToInt(s_cvGxDepthBits->m_stringValue.m_str)) {
        case 16:
            format.m_depthFormat = CGxFormat::Fmt_Ds160;
            break;

        case 24:
            format.m_depthFormat = CGxFormat::Fmt_Ds24X;
            break;

        case 32:
            format.m_depthFormat = CGxFormat::Fmt_Ds320;
            break;

        default:
            ConsoleWrite("Depth bits must be 16, 24, or 32", DEFAULT_COLOR);
            return false;
    }

    const char* refresh = s_cvGxRefresh->m_stringValue.m_str;
    uint32_t refreshRate;
    uint32_t refreshRateDenom;

    if (sscanf(refresh, "%d%c%d", &refreshRate, &separator, &refreshRateDenom) != 3) {
        if (sscanf(refresh, "%d", &refreshRate) != 1) {
            ConsoleWrite("Unsupported syntax in refresh rate", DEFAULT_COLOR);
            return false;
        }

        refreshRateDenom = 1;
    }

    TSGrowableArray<CGxMonitorMode> modes;
    GxAdapterMonitorModes(format.m_monitor, modes);

    uint32_t i;

    for (i = 0; i < modes.Count(); i++) {
        if (refreshRate == modes[i].m_refreshRate && refreshRateDenom == modes[i].m_refreshRateDenom) {
            break;
        }
    }

    if (i == modes.Count()) {
        ConsoleWrite("Unsupported refresh rate", DEFAULT_COLOR);
        return false;
    }

    format.m_refreshRate = modes[i].m_refreshRate;
    format.m_refreshRateDenom = modes[i].m_refreshRateDenom;

    uint32_t tripleBuffer = SStrToInt(s_cvGxTripleBuffer->m_stringValue.m_str);

    if (tripleBuffer >= 2) {
        ConsoleWrite("TripleBuffer must be 0 or 1", DEFAULT_COLOR);
        return false;
    }

    format.m_backBufferCount = (tripleBuffer != 0) + 1;
    format.m_vsync = SStrToInt(s_cvGxVSync->m_stringValue.m_str);
    format.m_aspect = SStrToInt(s_cvGxAspect->m_stringValue.m_str) != 0;
    format.m_maximize = SStrToInt(s_cvGxMaximize->m_stringValue.m_str) != 0;
    format.m_hwCursor = SStrToInt(s_cvGxCursor->m_stringValue.m_str) != 0;

    int32_t multisample = SStrToInt(s_cvGxMultisample->m_stringValue.m_str);

    if (multisample <= 1) {
        multisample = 1;
    } else if (multisample >= 16) {
        multisample = 16;
    }

    format.unk_002C = false;

    if (multisample > 1) {
        CVar* waterDetail = CVar::Lookup("waterDetail");

        if (waterDetail) {
            format.unk_002C |= Liquid::WaterDetailNeedsDepthCache(waterDetail->m_intValue) != 0;
        }

        CVar* sunShafts = CVar::Lookup("sunShafts");

        if (sunShafts) {
            format.unk_002C |= SunShafts::ModeNeedsDepthCache(sunShafts->m_intValue) != 0;
        }
    }

    format.m_multisampleCount = multisample;

    float quality = SStrToFloat(s_cvGxMultisampleQuality->m_stringValue.m_str);

    if (quality >= 0.0f) {
        format.m_multisampleQuality = quality < 1.0f ? quality : 1.0f;
    } else {
        format.m_multisampleQuality = 0.0f;
    }

    format.m_fixLag = SStrToInt(s_cvGxFixLag->m_stringValue.m_str) != 0;
    format.m_shaderModel = s_shaderModel;
    format.m_stereoEnabled = SStrToInt(s_cvGxStereoEnabled->m_stringValue.m_str) == 1;
    format.m_hwTnL = CmdLineGetBool(CMD_SW_TNL) == 0;

    return true;
}

// OFFSET: 0x00636DA0/0x1402CD650
void AddConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page)) {
    s_defaultCallbacks.New(callback);

    if (s_defaultsApplied) {
        for (uint32_t page = OPTION_PAGE_0; page < NUM_OPTION_PAGES; page++) {
            callback(static_cast<OPTION_PAGE>(page));
        }
    }
}

// OFFSET: 0x00636DE0/0x1402CD730
void RemoveConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page)) {
    uint32_t count = s_defaultCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (s_defaultCallbacks[i] == callback) {
            s_defaultCallbacks[i] = s_defaultCallbacks[--count];
            s_defaultCallbacks.SetCount(count);
        }
    }
}

// OFFSET: 0x00636E70/0x1402CD830
void ConsoleDeviceDefaults(OPTION_PAGE page) {
    ConsoleDetectSetDefaults(s_defaults, s_hardware);

    if (page == OPTION_PAGE_0) {
        uint32_t monitor = s_monitor;
        CGxFormat format;
        format = s_desktopFormat;
        format.m_monitor = monitor;
        format.m_size = s_desktopFormat.m_size;

        if (s_desktopFormat.m_size.x < s_desktopFormat.m_size.y) {
            format.m_size.x = s_desktopFormat.m_size.y;
            format.m_size.y = s_desktopFormat.m_size.x;
        }

        SetGxCVars(format);
        CCGxRestart(NULL, NULL);
        page = OPTION_PAGE_0;
    }

    for (uint32_t i = 0; i < s_defaultCallbacks.Count(); i++) {
        s_defaultCallbacks[i](page);
    }
}

// OFFSET: 0x00636F10/0x1402CD910
int32_t ConsoleDeviceInitialize(const char* title, bool a2, bool a3) {
    GxStartUp();
    GxLogOpen();

    s_cvHwDetect = CVar::Register("hwDetect", "do hardware detection", 1, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);

    if (!ConsoleDetectDetectHardware(s_hardware, s_hwChanged)) {
        ConsoleDetectDestroy();
        GxLogClose();
        return 0;
    }

    s_hardwareDetected = true;

    if (CmdLineGetBool(static_cast<CMDOPT>(CMD_HW_DETECT)) == 1 || s_cvHwDetect->m_intValue) {
        s_hwDetect = true;
        s_cvHwDetect->Set("0", true, false, false, true);
    } else {
        s_hwDetect = false;
    }

    RegisterGxCVars();
    ConsoleCommandRegister("gxRestart", CCGxRestart, CATEGORY_GRAPHICS, NULL, NULL);
    ValidateFormatMonitor(s_fallbackFormat, false);
    GxLog("ConsoleDeviceInitialize(): hwDetect = %d, hwChanged = %d", s_hwDetect, s_hwChanged);

    if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_800x600))) {
        s_cvGxResolution->Set("800x600", true, false, false, true);
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1024x768))) {
        s_cvGxResolution->Set("1024x768", true, false, false, true);
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1280x960))) {
        s_cvGxResolution->Set("1280x960", true, false, false, true);
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1280x1024))) {
        s_cvGxResolution->Set("1280x1024", true, false, false, true);
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1600x1200))) {
        s_cvGxResolution->Set("1600x1200", true, false, false, true);
    }

    if (s_cvVideoOptionsVersion->m_intValue < 4) {
        s_cvGxDepthBits->Set(s_depthBitsNames[s_desktopFormat.m_depthFormat], true, false, false, true);
    }

    if (s_hwDetect || s_hwChanged) {
        CGxFormat format;
        format = s_desktopFormat;
        format.m_window = s_cvGxWindow->m_intValue != 0;
        format.m_maximize = s_cvGxMaximize->m_intValue != 0;
        SetGxCVars(format);
    }

    bool window = s_cvGxWindow->m_intValue != 0;

    if (CmdLineGetBool(CMD_FULL_SCREEN)) {
        s_cvGxWindow->Set("0", true, false, false, true);
        window = false;
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_WINDOWED))) {
        s_cvGxWindow->Set("1", true, false, false, true);
        window = true;
    }

    s_desktopFormat.m_window = window;

    CGxFormat format;

    if (!LoadFormatFromCVars(format)) {
        format = s_desktopFormat;
    }

    if (!SetDeviceFormat(format)) {
        GxLog("ConsoleDeviceInitialize(): no output device available!");

        Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
        const char* caption = titleRec ? titleRec->m_message : "World of Warcraft";
        const char* message;

        if (GxGetRequestedApi() == GxApi_D3d9) {
            Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(2);
            message = rec ? rec->m_message : "World of Warcraft was unable to start up 3D acceleration. Please make sure DirectX 9.0c is installed and your video drivers are up-to-date.";
        } else {
            Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(3);
            message = rec ? rec->m_message : "World of Warcraft was unable to start up 3D acceleration.";
        }

        OsGuiMessageBox(NULL, 1, message, caption);
        ConsoleDetectDestroy();
        GxShutDown();
        GxLogClose();
        return 0;
    }

    ConsoleDetectSetDefaults(s_defaults, s_hardware);
    SetGxCVars(format);

    if (!CheckDeviceCaps()) {
        GxDevDestroy();

        Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
        const char* caption = titleRec ? titleRec->m_message : "World of Warcraft";
        Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(4);
        const char* message = rec ? rec->m_message : "Your 3D accelerator card is not supported by World of Warcraft. Please install a 3D accelerator card with dual-TMU support.";

        OsGuiMessageBox(NULL, 1, message, caption);
        ConsoleDetectDestroy();
        GxShutDown();
        GxLogClose();
        return 0;
    }

    switch (g_theGxDevicePtr->DeviceApi()) {
        case GxApi_OpenGl:
        case GxApi_GLL:
            ConsoleGxOverride(s_hardware.m_videoHw->m_oglOverrides);
            break;

        case GxApi_D3d9:
            ConsoleGxOverride(s_hardware.m_videoHw->m_d3DOverrides);
            break;

        default:
            break;
    }

    ConsoleGxOverride(CmdLineGetString(static_cast<CMDOPT>(CMD_GX_OVERRIDE)));

    for (uint32_t i = 0; i < GxOverrides_Last; i++) {
        if (s_consoleGxOverrideSet[i]) {
            g_theGxDevicePtr->DeviceOverride(static_cast<EGxOverride>(i), s_consoleGxOverrideVal[i]);
        }
    }

    SStrCopy(s_windowTitle, title ? title : "", sizeof(s_windowTitle));

    void* gxWindow = GxDevWindow();

    if (gxWindow) {
        OsGuiSetWindowTitle(gxWindow, title);
    }

    char version[32];
    SStrPrintf(version, sizeof(version), "%d", 4);
    s_cvVideoOptionsVersion->Set(version, true, false, false, true);

    RegisterStereoCVars();
    g_theGxDevicePtr->AddDisplayChangeCallback(DisplayChangeCallback);
    OsSetSleepInBackground(1);
    OsSetBackgroundSleepMs(250);

    return 1;
}
