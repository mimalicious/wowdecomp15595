#include "WoW/Source/DB/Rec/SpellScalingRec.h"

// STATIC: 0x00C20BD0/0x140A15560
const DBCMeta SpellScalingRec::s_meta = { "DBFilesClient\\SpellScaling.dbc", 10, 64, SpellScalingRec::FixupStrings, 0 };

// PROVEN: b62f4f38 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellScalingRec::FixupStrings(void* record, const char* strings) {
}
