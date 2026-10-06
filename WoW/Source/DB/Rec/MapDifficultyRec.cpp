#include "WoW/Source/DB/Rec/MapDifficultyRec.h"

// STATIC: 0x00C2348C/0x140A18708
const DBCMeta MapDifficultyRec::s_meta = { "DBFilesClient\\MapDifficulty.dbc", 7, 28, MapDifficultyRec::FixupStrings, 0 };

// PROVEN: 8af59ac2 IDENTICAL/NONE
// OFFSET: 0x00A2EA90/0x1407D80D0
void MapDifficultyRec::FixupStrings(void* record, const char* strings) {
    MapDifficultyRec* rec = static_cast<MapDifficultyRec*>(record);
    DBCFixupString(rec->m_message, strings);
    DBCFixupString(rec->m_difficultystring, strings);
}
