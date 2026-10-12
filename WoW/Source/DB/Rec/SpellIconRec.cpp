#include "WoW/Source/DB/Rec/SpellIconRec.h"

// STATIC: 0x00C212D8/0x140A15DF0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C212E0/0x140A15DF8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C212E8/0x140A15E00
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C212F0/0x140A15E08
const DBCMeta SpellIconRec::s_meta = { "DBFilesClient\\SpellIcon.dbc", 2, 8, SpellIconRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f5647291 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellIconRec::FixupStrings(void* record, const char* strings) {
    SpellIconRec* rec = static_cast<SpellIconRec*>(record);
    DBCFixupString(rec->m_textureFilename, strings);
}
