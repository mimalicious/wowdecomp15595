#ifndef WOW_SOURCE_UI_FLYOUT_H
#define WOW_SOURCE_UI_FLYOUT_H

#include <cstdint>

class SpellFlyoutRec;

class CFlyout {
    public:
    /* 0000 */ uint32_t unk_0000[6];
    /* 0018 */ const int32_t* m_spells;
    /* 001C */ uint32_t unk_001C[4];
    /* 002C */ uint32_t m_numKnownSpells;

    static void ClearKnownSpells();
    static bool AddKnownSpell(int32_t spellId);
    static void DelKnownSpell(int32_t spellId);
    static CFlyout* GetByID(int32_t id);
    static CFlyout* FindFlyoutForSpell(int32_t spellId);
    static void ShutdownGame();
    static void InitializeGame();
};

#endif
