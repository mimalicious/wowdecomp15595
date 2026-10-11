#ifndef WOW_SOURCE_UI_RUNEINFO_H
#define WOW_SOURCE_UI_RUNEINFO_H

#include <cstdint>

class CDataStore;
class SpellRec;

class CGRuneInfo {
    public:
    static void LeaveWorld();
    static int32_t m_runes[6];
    static void ConvertRune(uint8_t rune, uint8_t type);
    static void ResyncRunes(CDataStore* msg);
    static void AddRunePower(CDataStore* msg);
    static int32_t m_baseRunes[6];
    static uint32_t m_runeCooldowns[6];
    static uint32_t m_runeLastUsed[6];
    static uint32_t m_numRunes;
    static uint32_t m_predictedRunes;
    static void SetPredictedRunes(uint8_t available, uint8_t used, const uint8_t* cooldowns, bool force);
    static void InitializeGame();
    static void EnterWorld();
    static void ShutdownGame();
    static bool HasRunesForSpell(int32_t spellId, int32_t* needed);
    static uint32_t GetRuneRechargeTime(int32_t index);
    static int32_t CountPredictedRunesOfType(int32_t type);
    static bool ComputeRuneSpellCooldown(const SpellRec* spell, uint32_t* cooldown);
    static bool IsRuneIndexValid(int32_t index);
    static int32_t GetRuneTypeByIndex(int32_t index, bool base);
    static uint32_t GetRuneLastUsedTime(int32_t index);
    static void SetRuneLastUsedTime(int32_t index, uint32_t time);
    static void ComputeRuneRechargeTime(int32_t index, float progress);
};

#endif
