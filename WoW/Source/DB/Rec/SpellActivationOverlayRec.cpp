#include "WoW/Source/DB/Rec/SpellActivationOverlayRec.h"

// STATIC: 0x00C221C0/0x140A16F98
const DBCMeta SpellActivationOverlayRec::s_meta = { "DBFilesClient\\SpellActivationOverlay.dbc", 9, 44, SpellActivationOverlayRec::FixupStrings, 0 };

// PROVEN: aab4ed50 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellActivationOverlayRec::FixupStrings(void* record, const char* strings) {
}
