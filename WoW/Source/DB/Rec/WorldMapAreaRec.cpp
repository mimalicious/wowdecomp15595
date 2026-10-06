#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"

// STATIC: 0x00C1F460/0x140A139A0
const DBCMeta WorldMapAreaRec::s_meta = { "DBFilesClient\\WorldMapArea.dbc", 14, 56, WorldMapAreaRec::FixupStrings, 0 };

// PROVEN: ee10d6a8 IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void WorldMapAreaRec::FixupStrings(void* record, const char* strings) {
    WorldMapAreaRec* rec = static_cast<WorldMapAreaRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}
