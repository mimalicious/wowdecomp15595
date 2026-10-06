#include "WoW/Source/DB/Rec/ItemSetRec.h"

// STATIC: %0x00B45F08
const DBCMeta ItemSetRec::s_meta = { "DBFilesClient\\ItemSet.dbc", 7, 148, ItemSetRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ItemSetRec::FixupStrings(void* record, const char* strings) {
    ItemSetRec* rec = static_cast<ItemSetRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemSetRec::ConvertRow(ItemSetRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(rec->m_itemID, &row[0x0008], sizeof(rec->m_itemID));
    memcpy(rec->m_setSpellID, &row[0x004C], sizeof(rec->m_setSpellID));
    memcpy(rec->m_setThreshold, &row[0x006C], sizeof(rec->m_setThreshold));
    memcpy(&rec->m_requiredSkill, &row[0x008C], sizeof(rec->m_requiredSkill));
    memcpy(&rec->m_requiredSkillRank, &row[0x0090], sizeof(rec->m_requiredSkillRank));
}
#endif
