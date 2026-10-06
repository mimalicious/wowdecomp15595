#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"

// STATIC: 0x00C21228/0x140A15D08
const DBCMeta SpellItemEnchantmentRec::s_meta = { "DBFilesClient\\SpellItemEnchantment.dbc", 15, 92, SpellItemEnchantmentRec::FixupStrings, 0 };

// PROVEN: 4ba7add9 IDENTICAL/NONE
// OFFSET: 0x00A2E9D0/0x1407D7E60
void SpellItemEnchantmentRec::FixupStrings(void* record, const char* strings) {
    SpellItemEnchantmentRec* rec = static_cast<SpellItemEnchantmentRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
