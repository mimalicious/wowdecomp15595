#include "WoW/Source/DB/Rec/SpellClassOptionsRec.h"

// STATIC: 0x00C21A0C/0x140A16680
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 20, 24 };

// STATIC: 0x00C21A20/0x140A16698
static const int32_t s_fieldSizes[5] = { 4, 4, 12, 4, 4 };

// STATIC: 0x00C21A34/0x140A166B0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 2 };

// STATIC: 0x00C21A48/0x140A166C8
const DBCMeta SpellClassOptionsRec::s_meta = { "DBFilesClient\\SpellClassOptions.dbc", 5, 28, SpellClassOptionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: bde3ab7f IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E920/0x1407D7F90
void SpellClassOptionsRec::FixupStrings(void* record, const char* strings) {
    SpellClassOptionsRec* rec = static_cast<SpellClassOptionsRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
