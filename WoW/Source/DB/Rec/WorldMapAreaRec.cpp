#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"

// STATIC: 0x00C1F3B8/0x140A138F8
static const int32_t s_fieldOffsets[14] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C1F3F0/0x140A13930
static const int32_t s_fieldSizes[14] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1F428/0x140A13968
static const int32_t s_fieldTypes[14] = { 0, 0, 0, 2, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1F460/0x140A139A0
const DBCMeta WorldMapAreaRec::s_meta = { "DBFilesClient\\WorldMapArea.dbc", 14, 56, WorldMapAreaRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ee10d6a8 IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void WorldMapAreaRec::FixupStrings(void* record, const char* strings) {
    WorldMapAreaRec* rec = static_cast<WorldMapAreaRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}
