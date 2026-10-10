#ifndef WOW_SOURCE_UI_FLYOUT_H
#define WOW_SOURCE_UI_FLYOUT_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class SpellFlyoutRec;
class SpellFlyoutItemRec;

struct lua_State;

class CFlyout : public TSHashObject<CFlyout, HASHKEY_NONE> {
    public:
    struct Slot {
        /* 0000 */ const SpellFlyoutItemRec* m_rec;
        /* 0004 */ int32_t m_known;
    };

    /* 0018 */ const SpellFlyoutRec* m_rec;
    /* 001C */ TSGrowableArray<Slot> m_slots;
    /* 002C */ uint32_t m_numKnownSpells;

    ~CFlyout();
    void AddKnownSpellInternal(int32_t spellId);
    void DelKnownSpellInternal(int32_t spellId);
    void AddSlot(const SpellFlyoutItemRec* rec);

    static void InitializeGame();
    static void ShutdownGame();
    static CFlyout* GetByID(int32_t id);
    static int32_t FindFlyoutForSpell(int32_t spellId);
    static bool AddKnownSpell(int32_t spellId);
    static void DelKnownSpell(int32_t spellId);
    static void ClearKnownSpells();
    static void RegisterScriptFunctions();
    static void UnregisterScriptFunctions();
};

struct SpellToFlyoutMapItem : public TSHashObject<SpellToFlyoutMapItem, HASHKEY_NONE> {
    /* 0018 */ TSGrowableArray<CFlyout*> m_flyouts;
};

extern TSHashTable<CFlyout, HASHKEY_NONE> s_flyouts;
extern TSHashTable<SpellToFlyoutMapItem, HASHKEY_NONE> s_spellToFlyout;

#endif
