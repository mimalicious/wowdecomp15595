#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"

// STATIC: 0x00C21530/0x140A160D8
const DBCMeta SpellEffectCameraShakesRec::s_meta = { "DBFilesClient\\SpellEffectCameraShakes.dbc", 2, 16, SpellEffectCameraShakesRec::FixupStrings, 0 };

// PROVEN: b53220ba IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellEffectCameraShakesRec::FixupStrings(void* record, const char* strings) {
}
