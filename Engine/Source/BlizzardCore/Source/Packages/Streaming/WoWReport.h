#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_WOWREPORT_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_WOWREPORT_H

#include <cstdint>

namespace WoWReport {

void Initialize(const char* version, const char* language, bool converted, bool trial);
void SetConverted(const char* version, const char* language);
void SetFinalDownloadData();
void SetInitialDownloadData();
void SetStage(const char* stage);
void Shutdown();

}

#endif
