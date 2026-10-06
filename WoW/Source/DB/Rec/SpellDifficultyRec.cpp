#include "WoW/Source/DB/Rec/SpellDifficultyRec.h"

// STATIC: 0x00C21664/0x140A16268
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C2166C/0x140A16270
static const int32_t s_fieldSizes[2] = { 4, 16 };

// STATIC: 0x00C21674/0x140A16278
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C2167C/0x140A16280
const DBCMeta SpellDifficultyRec::s_meta = { "DBFilesClient\\SpellDifficulty.dbc", 2, 20, SpellDifficultyRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 52c001bf IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellDifficultyRec::FixupStrings(void* record, const char* strings) {
}
