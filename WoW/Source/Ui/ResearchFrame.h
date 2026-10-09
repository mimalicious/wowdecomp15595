#ifndef WOW_SOURCE_UI_RESEARCHFRAME_H
#define WOW_SOURCE_UI_RESEARCHFRAME_H

#include <cstdint>

class CGResearchFrame {
    public:
    static void EnterWorld();
    static void LeaveWorld();
    static uint8_t s_enteredWorld;
    static void InitializeGame();
    static bool IsInsideResearchSite(int32_t siteId);
    static bool CanScanResearchSite(int32_t researchField);
};

#endif
