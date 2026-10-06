#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"

// STATIC: %0x00B44F50
const DBCMeta SpellItemEnchantmentRec::s_meta = { "DBFilesClient\\SpellItemEnchantment.dbc", 15, 92, SpellItemEnchantmentRec::FixupStrings, 0 };

// OFFSET: %0x0097C210
void SpellItemEnchantmentRec::FixupStrings(void* record, const char* strings) {
    SpellItemEnchantmentRec* rec = static_cast<SpellItemEnchantmentRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellItemEnchantmentRec::ConvertRow(SpellItemEnchantmentRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_charges, &row[0x0004], sizeof(rec->m_charges));
    memcpy(rec->m_effect, &row[0x0008], sizeof(rec->m_effect));
    memcpy(rec->m_effectPointsMin, &row[0x0014], sizeof(rec->m_effectPointsMin));
    memcpy(rec->m_effectPointsMax, &row[0x0020], sizeof(rec->m_effectPointsMax));
    memcpy(rec->m_effectArg, &row[0x002C], sizeof(rec->m_effectArg));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0038])];
    memcpy(&rec->m_itemVisual, &row[0x003C], sizeof(rec->m_itemVisual));
    memcpy(&rec->m_flags, &row[0x0040], sizeof(rec->m_flags));
    memcpy(&rec->m_srcItemID, &row[0x0044], sizeof(rec->m_srcItemID));
    memcpy(&rec->m_conditionID, &row[0x0048], sizeof(rec->m_conditionID));
    memcpy(&rec->m_requiredSkillID, &row[0x004C], sizeof(rec->m_requiredSkillID));
    memcpy(&rec->m_requiredSkillRank, &row[0x0050], sizeof(rec->m_requiredSkillRank));
    memcpy(&rec->m_minLevel, &row[0x0054], sizeof(rec->m_minLevel));
    memcpy(&rec->unk_0058, &row[0x0058], sizeof(rec->unk_0058));
}
#endif
