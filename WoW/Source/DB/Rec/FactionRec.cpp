#include "WoW/Source/DB/Rec/FactionRec.h"

// STATIC: 0x00C2559C/0x140A1B180
static const int32_t s_fieldOffsets[12] = { 0, 4, 8, 24, 40, 56, 72, 76, 84, 92, 96, 100 };

// STATIC: 0x00C255CC/0x140A1B1B0
static const int32_t s_fieldSizes[12] = { 4, 4, 16, 16, 16, 16, 4, 8, 8, 4, 4, 4 };

// STATIC: 0x00C255FC/0x140A1B1E0
static const int32_t s_fieldTypes[12] = { 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 2, 0 };

// STATIC: 0x00C2562C/0x140A1B210
const DBCMeta FactionRec::s_meta = { "DBFilesClient\\Faction.dbc", 12, 104, FactionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 492a6bd0 IDENTICAL/NONE
// OFFSET: 0x00A2EBD0/0x1407D8470
void FactionRec::FixupStrings(void* record, const char* strings) {
    FactionRec* rec = static_cast<FactionRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
