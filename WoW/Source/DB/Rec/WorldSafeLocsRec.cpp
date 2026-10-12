#include "WoW/Source/DB/Rec/WorldSafeLocsRec.h"

// STATIC: 0x00C1F0F8/0x140A135B8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 20 };

// STATIC: 0x00C1F108/0x140A135C8
static const int32_t s_fieldSizes[4] = { 4, 4, 12, 4 };

// STATIC: 0x00C1F118/0x140A135D8
static const int32_t s_fieldTypes[4] = { 0, 0, 3, 2 };

// STATIC: 0x00C1F128/0x140A135E8
const DBCMeta WorldSafeLocsRec::s_meta = { "DBFilesClient\\WorldSafeLocs.dbc", 4, 24, WorldSafeLocsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBF0/0x1407D84B0
void WorldSafeLocsRec::FixupStrings(void* record, const char* strings) {
    WorldSafeLocsRec* rec = static_cast<WorldSafeLocsRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}
