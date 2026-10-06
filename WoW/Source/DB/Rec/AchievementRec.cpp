#include "WoW/Source/DB/Rec/AchievementRec.h"

// STATIC: %0x00B47354
const DBCMeta AchievementRec::s_meta = { "DBFilesClient\\Achievement.dbc", 14, 56, AchievementRec::FixupStrings, 0 };

// OFFSET: %0x0097C4C0
void AchievementRec::FixupStrings(void* record, const char* strings) {
    AchievementRec* rec = static_cast<AchievementRec*>(record);
    rec->m_title = &strings[reinterpret_cast<uintptr_t>(rec->m_title)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
    rec->m_reward = &strings[reinterpret_cast<uintptr_t>(rec->m_reward)];
}

#if defined(WOWDECOMP_ARCH_64)
void AchievementRec::ConvertRow(AchievementRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_faction, &row[0x0004], sizeof(rec->m_faction));
    memcpy(&rec->m_instanceID, &row[0x0008], sizeof(rec->m_instanceID));
    memcpy(&rec->m_supercedes, &row[0x000C], sizeof(rec->m_supercedes));
    rec->m_title = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    memcpy(&rec->m_category, &row[0x0018], sizeof(rec->m_category));
    memcpy(&rec->m_points, &row[0x001C], sizeof(rec->m_points));
    memcpy(&rec->m_uiOrder, &row[0x0020], sizeof(rec->m_uiOrder));
    memcpy(&rec->m_flags, &row[0x0024], sizeof(rec->m_flags));
    memcpy(&rec->m_iconID, &row[0x0028], sizeof(rec->m_iconID));
    rec->m_reward = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    memcpy(&rec->m_minimumCriteria, &row[0x0030], sizeof(rec->m_minimumCriteria));
    memcpy(&rec->m_sharesCriteria, &row[0x0034], sizeof(rec->m_sharesCriteria));
}
#endif
