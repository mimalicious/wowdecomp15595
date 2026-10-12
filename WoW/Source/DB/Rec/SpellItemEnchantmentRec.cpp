#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"

// STATIC: 0x00C21174/0x140A15C48
static const int32_t s_fieldOffsets[15] = { 0, 4, 8, 20, 32, 44, 56, 60, 64, 68, 72, 76, 80, 84, 88 };

// STATIC: 0x00C211B0/0x140A15C88
static const int32_t s_fieldSizes[15] = { 4, 4, 12, 12, 12, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C211EC/0x140A15CC8
static const int32_t s_fieldTypes[15] = { 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C21228/0x140A15D08
const DBCMeta SpellItemEnchantmentRec::s_meta = { "DBFilesClient\\SpellItemEnchantment.dbc", 15, 92, SpellItemEnchantmentRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 4ba7add9 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E9D0/0x1407D7E60
void SpellItemEnchantmentRec::FixupStrings(void* record, const char* strings) {
    SpellItemEnchantmentRec* rec = static_cast<SpellItemEnchantmentRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
