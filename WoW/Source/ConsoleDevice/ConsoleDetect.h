#ifndef WOW_SOURCE_CONSOLEDEVICE_CONSOLEDETECT_H
#define WOW_SOURCE_CONSOLEDEVICE_CONSOLEDETECT_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "WoW/Source/Types.h"

void ConsoleDetectDestroy();
int32_t ConsoleDetectDetectHardware(Hardware& hardware, bool& changed);
void ConsoleDetectGetResolutions(TSGrowableArray<C2iVector>& resolutions, uint32_t monitor, int32_t sort);
void ConsoleDetectSaveHardware(const Hardware& hardware, bool& changed);
void ConsoleDetectSetDefaults(DefaultSettings& defaults, const Hardware& hardware);

#endif
