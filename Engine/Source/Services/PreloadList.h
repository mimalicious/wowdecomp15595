#ifndef ENGINE_SOURCE_SERVICES_PRELOADLIST_H
#define ENGINE_SOURCE_SERVICES_PRELOADLIST_H

#include <cstdint>

class PreloadList {
    public:
    static PreloadList* StartDownload(int32_t raceID, int32_t classID, int32_t a3);
    static void StopDownload(PreloadList* list);

    int32_t IsComplete();
    int32_t ListNameChanged(int32_t raceID, int32_t classID);
    void Wait(int32_t a2);
};

#endif
