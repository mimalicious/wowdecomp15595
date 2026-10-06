#include "WoW/Source/DB/Rec/ItemSubClassRec.h"

// STATIC: %0x00B43A88
const DBCMeta ItemSubClassRec::s_meta = { "DBFilesClient\\ItemSubClass.dbc", 12, 52, ItemSubClassRec::FixupStrings, 0 };

// OFFSET: %0x0097BFE0
void ItemSubClassRec::FixupStrings(void* record, const char* strings) {
    ItemSubClassRec* rec = static_cast<ItemSubClassRec*>(record);
    rec->m_displayName = &strings[reinterpret_cast<uintptr_t>(rec->m_displayName)];
    rec->m_verboseName = &strings[reinterpret_cast<uintptr_t>(rec->m_verboseName)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemSubClassRec::ConvertRow(ItemSubClassRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_classID, &row[0x0004], sizeof(rec->m_classID));
    memcpy(&rec->m_subClassID, &row[0x0008], sizeof(rec->m_subClassID));
    memcpy(&rec->m_prerequisiteProficiency, &row[0x000C], sizeof(rec->m_prerequisiteProficiency));
    memcpy(&rec->m_postrequisiteProficiency, &row[0x0010], sizeof(rec->m_postrequisiteProficiency));
    memcpy(&rec->m_flags, &row[0x0014], sizeof(rec->m_flags));
    memcpy(&rec->m_displayFlags, &row[0x0018], sizeof(rec->m_displayFlags));
    memcpy(&rec->m_weaponParrySeq, &row[0x001C], sizeof(rec->m_weaponParrySeq));
    memcpy(&rec->m_weaponReadySeq, &row[0x0020], sizeof(rec->m_weaponReadySeq));
    memcpy(&rec->m_weaponAttackSeq, &row[0x0024], sizeof(rec->m_weaponAttackSeq));
    memcpy(&rec->m_weaponSwingSize, &row[0x0028], sizeof(rec->m_weaponSwingSize));
    rec->m_displayName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    rec->m_verboseName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0030])];
}
#endif
