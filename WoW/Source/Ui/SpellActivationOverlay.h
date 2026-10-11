#ifndef WOW_SOURCE_UI_SPELLACTIVATIONOVERLAY_H
#define WOW_SOURCE_UI_SPELLACTIVATIONOVERLAY_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class SpellActivationOverlayRec;
class SpellRec;

class CGSpellActivationOverlay {
    public:
    struct WatchedSpellInfo {
        /* 0000 */ int32_t m_spellId;
        /* 0004 */ bool m_usable;
    };

    struct ActiveMoverSpell {
        /* 0000 */ int32_t m_spellId;
        /* 0004 */ bool m_prevGlow;
        /* 0005 */ bool m_glow;
        /* 0006 */ bool m_prevUsable;
        /* 0007 */ bool m_usable;
    };

    struct COUNTERNODE : public TSHashObject<COUNTERNODE, HASHKEY_NONE> {
        /* 0018 */ int32_t m_count;
    };

    struct PETAURANODE : public TSHashObject<PETAURANODE, HASHKEY_NONE> {
        /* 0018 */ int32_t m_spellId;
        /* 001C */ bool m_wasActive;
        /* 001D */ bool m_active;
    };

    static ActiveMoverSpell s_activeMoverSpells[10];
    static TSHashTable<COUNTERNODE, HASHKEY_NONE> s_counters;
    static TSGrowableArray<WatchedSpellInfo> s_availableSpells;
    static TSGrowableArray<int32_t> s_glowableSpells;
    static TSHashTable<PETAURANODE, HASHKEY_NONE> s_petAuras;

    static void ShutdownGame();
    static void EnterWorld();
    static void ShowSpellOverlay(const SpellActivationOverlayRec* overlay);
    static void EnableActiveMoverGlow(int32_t classSet, const int32_t* classMask);
    static bool IsSpellOverlayed(int32_t spellId);
    static void ApplyChangeCheckSpell(int32_t spellId, int32_t classSet, const int32_t* classMask, int32_t change);
    static void ApplyChangeCheckSpells(int32_t classSet, const int32_t* classMask, int32_t change);
    static void UpdateUsable();
    static uint32_t AddOverlayGlowableSpell(int32_t spellId);
    static void DelOverlayGlowableSpell(int32_t spellId);
    static void AddOverlayOnAvailableSpell(int32_t spellId);
    static void DelOverlayOnAvailableSpell(int32_t spellId);
    static void UpdateActiveMoverOverlays();
    static void ClearActiveMoverSpells();
    static void AddActiveMoverSpells();
    static void OnAuraAdded(const SpellRec* spell);
    static void OnAuraRemoved(const SpellRec* spell);
};

#endif
