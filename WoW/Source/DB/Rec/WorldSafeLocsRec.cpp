#include "WoW/Source/DB/Rec/WorldSafeLocsRec.h"

// STATIC: 0x00C1F128/0x140A135E8
const DBCMeta WorldSafeLocsRec::s_meta = { "DBFilesClient\\WorldSafeLocs.dbc", 4, 24, WorldSafeLocsRec::FixupStrings, 0 };

// PROVEN: bfaf49e8 IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void WorldSafeLocsRec::FixupStrings(void* record, const char* strings) {
    WorldSafeLocsRec* rec = static_cast<WorldSafeLocsRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}
