#ifndef WOW_SOURCE_CONSOLEDEVICE_CONSOLEDEVICE_H
#define WOW_SOURCE_CONSOLEDEVICE_CONSOLEDEVICE_H

#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Types.h"

class CGxFormat;
struct CVar;

enum OPTION_PAGE {
    OPTION_PAGE_0 = 0,
    OPTION_PAGE_1 = 1,
    OPTION_PAGE_2 = 2,
    NUM_OPTION_PAGES = 3,
};

extern DefaultSettings s_defaults;
extern CGxFormat s_desktopFormat;
extern CGxFormat s_fallbackFormat;
extern Hardware s_hardware;
extern CGxFormat s_lastGoodFormat;
extern uint32_t s_monitor;
extern TSGrowableArray<CGxMonitorMode> s_gxMonitorModes;
extern int32_t s_shaderModel;
extern bool s_gxRestarting;
extern CVar* s_cvFixedFunction;
extern CVar* s_cvGraphicsQuality;
extern CVar* s_cvGxApi;
extern CVar* s_cvGxAspect;
extern CVar* s_cvGxColorBits;
extern CVar* s_cvGxCursor;
extern CVar* s_cvGxDepthBits;
extern CVar* s_cvGxFixLag;
extern CVar* s_cvGxMaximize;
extern CVar* s_cvGxMonitor;
extern CVar* s_cvGxMultisample;
extern CVar* s_cvGxMultisampleQuality;
extern CVar* s_cvGxOverride;
extern CVar* s_cvGxRefresh;
extern CVar* s_cvGxResolution;
extern CVar* s_cvGxStereoConvergence;
extern CVar* s_cvGxStereoEnabled;
extern CVar* s_cvGxStereoSeparation;
extern CVar* s_cvGxTripleBuffer;
extern CVar* s_cvGxVSync;
extern CVar* s_cvGxWindow;
extern CVar* s_cvHwDetect;
extern CVar* s_cvVideoOptionsVersion;
extern CVar* s_cvWindowResizeLock;
extern char s_windowTitle[256];
extern const char* s_colorBitsNames[8];
extern const char* s_depthBitsNames[8];
extern const char* s_apiNames[4];

void AddConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page));
int32_t CCGxRestart(const char* command, const char* arguments);
void ConsoleDeviceDefaults(OPTION_PAGE page);
void ConsoleDeviceDestroy();
DefaultSettings* ConsoleDeviceGetDefaultSettings();
int32_t ConsoleDeviceHardwareChanged();
int32_t ConsoleDeviceInitialize(const char* title, bool a2, bool a3);
Hardware* ConsoleGetHardware();
void ConsoleGxOverride(const char* list);
int32_t IsConsoleEnabled();
bool LoadFormatFromCVars(CGxFormat& format);
void RegisterGxCVars();
void RegisterStereoCVars();
void RemoveConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page));
void SetDesktopFormat();
int32_t SetDeviceFormat(CGxFormat& format);
void SetGxCVars(const CGxFormat& format);
void DisplayChangeCallback(uint32_t monitor);
void ValidateFormatMonitor(CGxFormat& format, bool log);

#endif
