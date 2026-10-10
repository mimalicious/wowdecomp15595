#ifndef WOW_SOURCE_UI_SPELLACTIVATIONOVERLAY_H
#define WOW_SOURCE_UI_SPELLACTIVATIONOVERLAY_H

#include <cstdint>

class SpellRec;

class CGSpellActivationOverlay {
    public:
    static void ShutdownGame();
    static void EnterWorld();
    static void UpdateUsable();
    static uint32_t AddOverlayGlowableSpell(int32_t spellId);
    static void DelOverlayGlowableSpell(int32_t spellId);
    static void AddOverlayOnAvailableSpell(int32_t spellId);
    static void DelOverlayOnAvailableSpell(int32_t spellId);
    static void UpdateActiveMoverOverlays();
    static void OnAuraAdded(const SpellRec* spell);
    static void OnAuraRemoved(const SpellRec* spell);
};

#endif
