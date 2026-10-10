#ifndef WOW_SOURCE_UI_RESEARCHFRAME_H
#define WOW_SOURCE_UI_RESEARCHFRAME_H

#include <cstdint>

class CDataStore;

class CGResearchFrame {
    public:
    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static uint8_t s_enteredWorld;
    static int32_t m_skillLine;
    static void InitializeGame();
    static bool IsInsideResearchSite(int32_t siteId);
    static bool CanScanResearchSite(int32_t researchField);
    static bool CanCastSpell(int32_t spellId, int32_t& reason);
    static void PutWeightForSpell(CDataStore* msg);
};

#endif
