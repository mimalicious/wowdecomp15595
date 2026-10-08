#ifndef WOW_SOURCE_UI_SHAREDSCRIPTS_H
#define WOW_SOURCE_UI_SHAREDSCRIPTS_H

#include <cstddef>
#include <cstdint>

extern char s_visitableURLs[4][1024];

const char* GetVisitableURL(int32_t index);
void FormatVisitableURL(int32_t index, char* buffer, uint32_t size);
void SetVisitableURL(int32_t index, const char* url);
int32_t GetPortalLocales(int32_t* locales, int32_t* count);

void RegisterSharedScriptFunctions();
void UnregisterSharedScriptFunctions();

#endif
