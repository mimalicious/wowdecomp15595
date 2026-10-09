#ifndef WOW_SOURCE_UI_SPELLACTIVATIONOVERLAY_H
#define WOW_SOURCE_UI_SPELLACTIVATIONOVERLAY_H

#include <cstdint>

class SpellRec;

class CGSpellActivationOverlay {
    public:
    static void ShutdownGame();
    static void EnterWorld();
    static void UpdateUsable();
    static void UpdateActiveMoverOverlays();
    static void OnAuraAdded(const SpellRec* spell);
    static void OnAuraRemoved(const SpellRec* spell);
};

#endif
