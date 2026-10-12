#include "WoW/Source/DB/Rec/SpellRangeRec.h"

// STATIC: 0x00C20D90/0x140A15798
static const int32_t s_fieldOffsets[6] = { 0, 4, 12, 20, 24, 28 };

// STATIC: 0x00C20DA8/0x140A157B0
static const int32_t s_fieldSizes[6] = { 4, 8, 8, 4, 4, 4 };

// STATIC: 0x00C20DC0/0x140A157C8
static const int32_t s_fieldTypes[6] = { 0, 3, 3, 0, 2, 2 };

// STATIC: 0x00C20DD8/0x140A157E0
const DBCMeta SpellRangeRec::s_meta = { "DBFilesClient\\SpellRange.dbc", 6, 32, SpellRangeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 09fa2c9e IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E990/0x1407D7E20
void SpellRangeRec::FixupStrings(void* record, const char* strings) {
    SpellRangeRec* rec = static_cast<SpellRangeRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_displayNameShort, strings);
}
