#ifndef ENGINE_SOURCE_SERVICES_PRELOADLIST_H
#define ENGINE_SOURCE_SERVICES_PRELOADLIST_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CAsyncObject;

class PreloadList {
    public:
    static void GetListName(char* name, int32_t raceID, int32_t classID);
    static int32_t DownloadAndWait(const char* name, int32_t showProgress, void (*progressCallback)(float, int32_t));
    static void ListFinishedCallback(CAsyncObject* object);
    static PreloadList* StartDownload(const char* name, int32_t waitAll);
    static PreloadList* StartDownload(int32_t raceID, int32_t classID, int32_t waitAll);
    static void StopDownload(PreloadList* list);
    static void WaitAll();
    static void Shutdown();

    /* 0000 */ TSGrowableArray<CAsyncObject*> m_objects;
    /* 0010 */ char m_name[260];
    /* 0114 */ int32_t m_waitAll;

    PreloadList(const char* name, int32_t waitAll);
    PreloadList(int32_t raceID, int32_t classID, int32_t waitAll);
    ~PreloadList();
    void Cancel();
    int32_t IsComplete();
    int32_t ListNameChanged(int32_t raceID, int32_t classID);
    void ParseAndLoad(const char* name);
    void PreloadFile(const char* name);
    int32_t Wait();
};

#endif
