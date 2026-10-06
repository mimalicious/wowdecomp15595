#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"

// STATIC: 0x00C215E4/0x140A161C0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C215F8/0x140A161D8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C2160C/0x140A161F0
static const int32_t s_fieldTypes[5] = { 0, 2, 0, 0, 2 };

// STATIC: 0x00C21620/0x140A16208
const DBCMeta SpellDispelTypeRec::s_meta = { "DBFilesClient\\SpellDispelType.dbc", 5, 20, SpellDispelTypeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 95edf69d IDENTICAL/NONE
// OFFSET: 0x00A2EA60/0x1407D7ED0
void SpellDispelTypeRec::FixupStrings(void* record, const char* strings) {
    SpellDispelTypeRec* rec = static_cast<SpellDispelTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_internalName, strings);
}
