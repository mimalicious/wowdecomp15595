#include "WoW/Source/DB/Rec/Cfg_ConfigsRec.h"

// STATIC: %0x00B43FE8
const DBCMeta Cfg_ConfigsRec::s_meta = { "DBFilesClient\\Cfg_Configs.dbc", 4, 20, Cfg_ConfigsRec::FixupStrings, 0 };

// PROVEN: b6023f3b IDENTICAL/NONE
// OFFSET: 0x00401000
void Cfg_ConfigsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void Cfg_ConfigsRec::ConvertRow(Cfg_ConfigsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_generatedID, &row[0x0000], sizeof(rec->m_generatedID));
    memcpy(&rec->m_id, &row[0x0004], sizeof(rec->m_id));
    memcpy(&rec->m_realmType, &row[0x0008], sizeof(rec->m_realmType));
    memcpy(&rec->m_playerKillingAllowed, &row[0x000C], sizeof(rec->m_playerKillingAllowed));
    memcpy(&rec->m_roleplaying, &row[0x0010], sizeof(rec->m_roleplaying));
}
#endif
