#include "WoW/Source/DB/Rec/SpellReagentsRec.h"

// STATIC: 0x00C20C8C/0x140A15658
static const int32_t s_fieldOffsets[3] = { 0, 4, 36 };

// STATIC: 0x00C20C98/0x140A15668
static const int32_t s_fieldSizes[3] = { 4, 32, 32 };

// STATIC: 0x00C20CA4/0x140A15678
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C20CB0/0x140A15688
const DBCMeta SpellReagentsRec::s_meta = { "DBFilesClient\\SpellReagents.dbc", 3, 68, SpellReagentsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 539b9cc5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellReagentsRec::FixupStrings(void* record, const char* strings) {
}
