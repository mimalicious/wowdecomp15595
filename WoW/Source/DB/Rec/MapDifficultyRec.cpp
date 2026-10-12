#include "WoW/Source/DB/Rec/MapDifficultyRec.h"

// STATIC: 0x00C23438/0x140A186A8
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C23454/0x140A186C8
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23470/0x140A186E8
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 2, 0, 0, 2 };

// STATIC: 0x00C2348C/0x140A18708
const DBCMeta MapDifficultyRec::s_meta = { "DBFilesClient\\MapDifficulty.dbc", 7, 28, MapDifficultyRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 8af59ac2 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EA90/0x1407D80D0
void MapDifficultyRec::FixupStrings(void* record, const char* strings) {
    MapDifficultyRec* rec = static_cast<MapDifficultyRec*>(record);
    DBCFixupString(rec->m_message, strings);
    DBCFixupString(rec->m_difficultystring, strings);
}
