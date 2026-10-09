#include <cstdint>
#include "ConsoleDevice.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Services/Texture.h"

// OFFSET: 0x00636BF0/0x1402CD450
int32_t CCGxRestart(const char* command, const char* arguments) {
    s_gxRestarting = true;

    CGxFormat format;
    LoadFormatFromCVars(format);
    GxAdapterMonitorModes(format.m_monitor, s_gxMonitorModes);
    ValidateFormatMonitor(format, true);

    if (!SetDeviceFormat(format)) {
        ConsoleWrite("unable to set requested display mode", DEFAULT_COLOR);
        format = s_lastGoodFormat;
        s_cvGxWindow->Set(format.m_window, true, false, false, true);
        s_cvGxMonitor->Set(format.m_monitor, true, false, false, true);

        if (!SetDeviceFormat(format)) {
            ConsoleWrite("unable to set last good mode", DEFAULT_COLOR);
            format = s_desktopFormat;
            s_cvGxWindow->Set(format.m_window, true, false, false, true);
            s_cvGxMonitor->Set(format.m_monitor, true, false, false, true);

            if (!SetDeviceFormat(format)) {
                ConsoleWrite("unable to set default format", DEFAULT_COLOR);
                format = s_fallbackFormat;
                s_cvGxWindow->Set(format.m_window, true, false, false, true);
                s_cvGxMonitor->Set(format.m_monitor, true, false, false, true);
                SetDeviceFormat(format);
            }
        }
    }

    s_lastGoodFormat = format;
    OsGuiSetWindowTitle(GxDevWindow(), s_windowTitle);
    TextureNotifyGxRestart();
    s_gxRestarting = false;

    return 1;
}
