#include "WoW/Source/DB/Rec/SpellVisualRec.h"

// STATIC: 0x00C2087C/0x140A15158
const DBCMeta SpellVisualRec::s_meta = { "DBFilesClient\\SpellVisual.dbc", 29, 132, SpellVisualRec::FixupStrings, 0 };

// PROVEN: 9742ae4a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellVisualRec::FixupStrings(void* record, const char* strings) {
}
