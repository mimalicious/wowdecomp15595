#include "WoW/Source/DB/Rec/PvpDifficultyRec.h"

// STATIC: 0x00C22E40/0x140A17F08
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C22E58/0x140A17F20
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C22E70/0x140A17F38
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C22E88/0x140A17F50
const DBCMeta PvpDifficultyRec::s_meta = { "DBFilesClient\\PvpDifficulty.dbc", 6, 24, PvpDifficultyRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void PvpDifficultyRec::FixupStrings(void* record, const char* strings) {
}
