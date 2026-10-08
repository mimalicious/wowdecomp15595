#ifndef ENGINE_SOURCE_OS_LOCALE_H
#define ENGINE_SOURCE_OS_LOCALE_H

#include <cstdint>

uint32_t OsGetCurrentLocale();
uint32_t OsGetLangIdFromLocale(uint32_t locale);
uint32_t OsGetUserDefaultLangID();

#endif
