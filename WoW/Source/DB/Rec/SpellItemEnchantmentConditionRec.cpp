#include "WoW/Source/DB/Rec/SpellItemEnchantmentConditionRec.h"

// STATIC: %0x00B44F14
const DBCMeta SpellItemEnchantmentConditionRec::s_meta = { "DBFilesClient\\SpellItemEnchantmentCondition.dbc", 7, 72, SpellItemEnchantmentConditionRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SpellItemEnchantmentConditionRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellItemEnchantmentConditionRec::ConvertRow(SpellItemEnchantmentConditionRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_ltOperandType, &row[0x0004], sizeof(rec->m_ltOperandType));
    memcpy(rec->m_ltOperand, &row[0x000C], sizeof(rec->m_ltOperand));
    memcpy(rec->m_operator, &row[0x0020], sizeof(rec->m_operator));
    memcpy(rec->m_rtOperandType, &row[0x0025], sizeof(rec->m_rtOperandType));
    memcpy(rec->m_rtOperand, &row[0x002C], sizeof(rec->m_rtOperand));
    memcpy(rec->m_logic, &row[0x0040], sizeof(rec->m_logic));
}
#endif
