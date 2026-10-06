#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"

// STATIC: 0x00C1CB48/0x140A107B8
const DBCMeta SpellVisualKitRec::s_meta = { "DBFilesClient\\SpellVisualKit.dbc", 22, 156, SpellVisualKitRec::FixupStrings, 0 };

// PROVEN: e5b30eb5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellVisualKitRec::FixupStrings(void* record, const char* strings) {
}
