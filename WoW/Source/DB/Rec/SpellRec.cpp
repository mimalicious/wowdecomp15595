#include "WoW/Source/DB/Rec/SpellRec.h"

// STATIC: 0x00C21718/0x140A16350
static const int32_t s_fieldOffsets[47] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188 };

// STATIC: 0x00C217D8/0x140A16410
static const int32_t s_fieldSizes[47] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C21898/0x140A164D0
static const int32_t s_fieldTypes[47] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C21954/0x140A16590
const DBCMeta SpellRec::s_meta = { "DBFilesClient\\Spell.dbc", 47, 192, SpellRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E9E0/0x1407D7F10
void SpellRec::FixupStrings(void* record, const char* strings) {
    SpellRec* rec = static_cast<SpellRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameSubtext, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_auraDescription, strings);
}
