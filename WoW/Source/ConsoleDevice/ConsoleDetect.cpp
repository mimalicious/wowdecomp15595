#include <cstdint>
#include <cstdlib>
#include "ConsoleDetect.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Hardware.h"

struct VideoDeviceID {
    /* 0000 */ uint16_t m_vendorID;
    /* 0002 */ uint16_t m_deviceID;
};

// STATIC: 0x00CC90B8/0x140B695A0
static const char* s_hwRegNames[] = { "HWCpuIdx", "HWMemIdx", "HWVideoID", "HWSoundIdx" };

// STATIC: 0x00CC90C8
static const VideoDeviceID s_highEndDevices[] = {
    { 0x10DE, 0x06C0 },
    { 0x1002, 0x6898 },
    { 0x1002, 0x689C },
    { 0, 0 },
};

// STATIC: 0x00CC90D8
static const VideoDeviceID s_midEndDevices[] = {
    { 0x10DE, 0x05E0 },
    { 0x10DE, 0x05E1 },
    { 0x10DE, 0x05E3 },
    { 0x1002, 0x6899 },
    { 0x1002, 0x689E },
    { 0, 0 },
};

// PROVEN: 4c38f067 IDENTICAL/IDENTICAL
// OFFSET: 0x00637440/0x1402CE080
static int CompareResolutions(const void* a, const void* b) {
    const C2iVector* left = static_cast<const C2iVector*>(a);
    const C2iVector* right = static_cast<const C2iVector*>(b);

    if (left->x < right->x) {
        return -1;
    }

    if (left->x > right->x) {
        return 1;
    }

    if (left->y < right->y) {
        return -1;
    }

    return left->y > right->y;
}

// OFFSET: 0x00637480/0x1402CE0B0
void ConsoleDetectSetDefaults(DefaultSettings& defaults, const Hardware& hardware) {
    defaults.m_graphicsQuality = 2;

    int32_t shaderModel = g_theGxDevicePtr->Caps().m_shaderModel;

    if (shaderModel < 4) {
        defaults.m_graphicsQuality = 0;
    } else if (shaderModel == 4) {
        defaults.m_graphicsQuality = 1;
    } else if (shaderModel == 5) {
        defaults.m_graphicsQuality = 2;
    } else {
        defaults.m_graphicsQuality = 3;
    }

    uint32_t vendorID = g_theGxDevicePtr->Caps().m_vendorId;
    uint32_t deviceID = g_theGxDevicePtr->Caps().m_deviceId;
    bool found = false;

    for (const VideoDeviceID* device = s_highEndDevices; device->m_vendorID; device++) {
        if (vendorID == device->m_vendorID && deviceID == device->m_deviceID) {
            defaults.m_graphicsQuality = 4;
            found = true;
            break;
        }
    }

    if (!found) {
        for (const VideoDeviceID* device = s_midEndDevices; device->m_vendorID; device++) {
            if (vendorID == device->m_vendorID && deviceID == device->m_deviceID) {
                defaults.m_graphicsQuality = 3;
                found = true;
                break;
            }
        }
    }

    if (!found && vendorID == 0x8086) {
        int32_t vendor = 0;
        OsGetProcessorFeaturesEx(vendor);

        if (vendor == 1 && (OsGetProcessorFeatures() & 0x200)) {
            defaults.m_graphicsQuality = 2;
        }
    }

    if (OsGetPhysicalMemory() <= 0x40000000 && defaults.m_graphicsQuality > 2) {
        defaults.m_graphicsQuality = 2;
    }
}

// OFFSET: 0x006375B0/0x1402CE1E0
void ConsoleDetectDestroy() {
    g_videoHardwareDB.Unload(&VideoHardwareRec::s_meta);
}

// OFFSET: 0x006375C0/0x1402CE200
static void SetVideoIdx(Hardware& hardware) {
    for (int32_t i = 1; i < g_videoHardwareDB.m_numRecords; i++) {
        const VideoHardwareRec* rec = g_videoHardwareDB.GetRecordByIndex(i);

        if (hardware.m_videoDevice.m_vendorID == rec->m_vendorID && hardware.m_videoDevice.m_deviceID == rec->m_deviceID) {
            hardware.m_videoID = rec->m_id;
            return;
        }
    }
}

// OFFSET: 0x00637620/0x1402CE260
static void ShowFatalMessageBox(int32_t stringID, const char* message) {
    Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
    const char* caption = titleRec ? titleRec->m_message : "World of Warcraft";
    Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(stringID);

    OsGuiMessageBox(OsGuiGetWindow(2), 1, rec ? rec->m_message : message, caption);
}

// OFFSET: 0x006376A0/0x1402CE330
void ConsoleDetectSaveHardware(const Hardware& hardware, bool& changed) {
    uint32_t cpuIdx;
    uint32_t memIdx;
    uint32_t videoID;
    uint32_t soundIdx;

    if (!SRegLoadValue("World of Warcraft\\Client", s_hwRegNames[0], 0, &cpuIdx)) {
        cpuIdx = hardware.m_cpuIdx;
    }

    if (!SRegLoadValue("World of Warcraft\\Client", s_hwRegNames[1], 0, &memIdx)) {
        memIdx = hardware.m_memIdx;
    }

    if (!SRegLoadValue("World of Warcraft\\Client", s_hwRegNames[2], 0, &videoID)) {
        videoID = hardware.m_videoID;
    }

    if (!SRegLoadValue("World of Warcraft\\Client", s_hwRegNames[3], 0, &soundIdx)) {
        soundIdx = hardware.m_soundIdx;
    }

    bool genericVideo = videoID == 1 || videoID == 168 || videoID == 169 || videoID == 170;

    if (hardware.m_cpuIdx == cpuIdx && hardware.m_memIdx == memIdx && (genericVideo || hardware.m_videoID == videoID) && hardware.m_soundIdx == soundIdx) {
        changed = false;
    } else {
        Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
        const char* caption = titleRec ? titleRec->m_message : "World of Warcraft";
        Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(5);
        const char* message = rec ? rec->m_message : "Hardware changed.  Reload default settings?";

        changed = OsGuiMessageBox(OsGuiGetWindow(2), 4, message, caption) == 0;
    }

    SRegSaveValue("World of Warcraft\\Client", s_hwRegNames[0], 0, hardware.m_cpuIdx);
    SRegSaveValue("World of Warcraft\\Client", s_hwRegNames[1], 0, hardware.m_memIdx);
    SRegSaveValue("World of Warcraft\\Client", s_hwRegNames[2], 0, hardware.m_videoID);
    SRegSaveValue("World of Warcraft\\Client", s_hwRegNames[3], 0, hardware.m_soundIdx);
}

// OFFSET: 0x00637910/0x1402CE720
int32_t ConsoleDetectDetectHardware(Hardware& hardware, bool& changed) {
    if (OsIsRemoteSession()) {
        int32_t version = OsGetVersion();

        if (version < 15 || version > 16) {
            ShowFatalMessageBox(12, "Running World of Warcraft through a Remote Desktop connection is not supported on this operating system.  Exiting program.");
            return 0;
        }
    }

    g_videoHardwareDB.Load(&VideoHardwareRec::s_meta);

    int64_t ticks = OsGetProcessorTicksPerSecond();
    hardware.m_cpuIdx = static_cast<double>(ticks) * 0.000001 > 1500.0;
    hardware.m_memIdx = 0;
    hardware.m_videoID = 0;

    if (GxAdapterID(hardware.m_videoDevice.m_vendorID, hardware.m_videoDevice.m_deviceID, hardware.m_videoDevice.m_driverVersionHi, hardware.m_videoDevice.m_driverVersionLo)) {
        SetVideoIdx(hardware);
    }

    if (!hardware.m_videoID) {
        hardware.m_videoDevice.m_vendorID = 0xFFFF;

        if (GxAdapterInfer(hardware.m_videoDevice.m_deviceID)) {
            SetVideoIdx(hardware);
        }

        if (!hardware.m_videoID) {
            int32_t stringID;
            const char* message;

            if (!GxAdapterD3dAvailable()) {
                stringID = 256;
                message = "World of Warcraft requires the latest version of the DirectX Runtime to be installed.  The latest DirectX runtime can be installed through Windows Update or downloaded directly from Microsoft. For more information please see http://us.battle.net/support/en/article/driver-update-information";
            } else if (!GxAdapterHasDriver()) {
                stringID = 257;
                message = "World of Warcraft was unable to find a suitable graphics card, please check that your graphics driver is up to date and that you are not running in safe (VGA) mode. For more information please see http://us.battle.net/support/en/article/driver-update-information";
            } else {
                stringID = 11;
                message = "Failed to find a suitable display device.  Exiting program.";
            }

            ShowFatalMessageBox(stringID, message);
            g_videoHardwareDB.Unload(&VideoHardwareRec::s_meta);
            return 0;
        }
    }

    hardware.m_soundIdx = 0;
    hardware.m_videoHw = g_videoHardwareDB.GetRecord(hardware.m_videoID);

    ConsoleDetectSaveHardware(hardware, changed);

    char line[1024];
    GxLog("ConsoleDetectDetectHardware():");
    SStrPrintf(line, sizeof(line), "\tcpuIdx: %d", hardware.m_cpuIdx);
    GxLog(line);
    SStrPrintf(line, sizeof(line), "\tvideoID: %d", hardware.m_videoID);
    GxLog(line);
    SStrPrintf(line, sizeof(line), "\tsoundIdx: %d", hardware.m_soundIdx);
    GxLog(line);
    SStrPrintf(line, sizeof(line), "\tmemIdx: %d", hardware.m_memIdx);
    GxLog(line);

    return 1;
}

// OFFSET: 0x00637C00
static void AddResolution(TSGrowableArray<C2iVector>& resolutions, const C2iVector& resolution) {
    for (uint32_t i = 0; i < resolutions.Count(); i++) {
        if (resolutions[i].x == resolution.x && resolutions[i].y == resolution.y) {
            return;
        }
    }

    resolutions.New(resolution);
}

// OFFSET: 0x00637C90/0x1402CEB30
void ConsoleDetectGetResolutions(TSGrowableArray<C2iVector>& resolutions, uint32_t monitor, int32_t sort) {
    resolutions.SetCount(0);

    CGxMonitorMode desktop;
    desktop.m_size.x = 0;
    desktop.m_size.y = 0;
    GxAdapterDesktopMode(monitor, desktop);
    bool portrait = static_cast<float>(desktop.m_size.x) / static_cast<float>(desktop.m_size.y) < 1.0f;

    TSGrowableArray<CGxMonitorMode> modes;
    GxAdapterMonitorModes(monitor, modes);

    int32_t maxWidth = 0;

    for (uint32_t i = 0; i < modes.Count(); i++) {
        if (modes[i].m_size.x > maxWidth) {
            maxWidth = modes[i].m_size.x;
        }
    }

    int32_t lastWidth = 0;
    int32_t lastHeight = 0;
    bool addPortrait = false;

    for (uint32_t i = 0; i < modes.Count(); i++) {
        CGxMonitorMode& mode = modes[i];
        float aspect = static_cast<float>(mode.m_size.x) / static_cast<float>(mode.m_size.y);

        if ((mode.m_size.x == lastWidth && mode.m_size.y == lastHeight) || mode.m_bpp < 16) {
            continue;
        }

        if (aspect >= 1.248f && mode.m_size.x > 640 && mode.m_size.y > 480) {
            resolutions.New(mode.m_size);
        } else if (portrait && aspect < 1.0f && mode.m_size.y <= maxWidth && mode.m_size.y > 640 && mode.m_size.x > 480) {
            addPortrait = true;
            C2iVector rotated = { mode.m_size.y, mode.m_size.x };
            resolutions.New(rotated);
        }

        lastWidth = mode.m_size.x;
        lastHeight = mode.m_size.y;
    }

    if (addPortrait) {
        if (maxWidth >= 800) {
            C2iVector size = { 800, 600 };
            AddResolution(resolutions, size);
        }

        if (maxWidth >= 1024) {
            C2iVector size = { 1024, 768 };
            AddResolution(resolutions, size);
        }

        if (maxWidth >= 1152) {
            C2iVector size = { 1152, 864 };
            AddResolution(resolutions, size);
        }

        if (maxWidth >= 1280) {
            C2iVector size = { 1280, 960 };
            AddResolution(resolutions, size);
            size.x = 1280;
            size.y = 1024;
            AddResolution(resolutions, size);
        }

        if (maxWidth >= 1600) {
            C2iVector size = { 1600, 1200 };
            AddResolution(resolutions, size);
        }
    }

    if (sort) {
        qsort(resolutions.Ptr(), resolutions.Count(), sizeof(C2iVector), CompareResolutions);
    }
}
