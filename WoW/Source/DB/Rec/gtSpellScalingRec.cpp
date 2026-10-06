#include "WoW/Source/DB/Rec/gtSpellScalingRec.h"

// STATIC: 0x00C24BD8/0x140A1A4B8
const DBCMeta gtSpellScalingRec::s_meta = { "DBFilesClient\\gtSpellScaling.dbc", 1, 8, gtSpellScalingRec::FixupStrings, 0 };

// PROVEN: 286b55bc IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtSpellScalingRec::FixupStrings(void* record, const char* strings) {
}
