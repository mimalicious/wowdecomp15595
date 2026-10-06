#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"

// STATIC: %0x00B43D88
const DBCMeta CreatureFamilyRec::s_meta = { "DBFilesClient\\CreatureFamily.dbc", 11, 48, CreatureFamilyRec::FixupStrings, 0 };

// OFFSET: %0x0097C050
void CreatureFamilyRec::FixupStrings(void* record, const char* strings) {
    CreatureFamilyRec* rec = static_cast<CreatureFamilyRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_iconFile = &strings[reinterpret_cast<uintptr_t>(rec->m_iconFile)];
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureFamilyRec::ConvertRow(CreatureFamilyRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_minScale, &row[0x0004], sizeof(rec->m_minScale));
    memcpy(&rec->m_minScaleLevel, &row[0x0008], sizeof(rec->m_minScaleLevel));
    memcpy(&rec->m_maxScale, &row[0x000C], sizeof(rec->m_maxScale));
    memcpy(&rec->m_maxScaleLevel, &row[0x0010], sizeof(rec->m_maxScaleLevel));
    memcpy(rec->m_skillLine, &row[0x0014], sizeof(rec->m_skillLine));
    memcpy(&rec->m_petFoodMask, &row[0x001C], sizeof(rec->m_petFoodMask));
    memcpy(&rec->m_petTalentType, &row[0x0020], sizeof(rec->m_petTalentType));
    memcpy(&rec->m_categoryEnumID, &row[0x0024], sizeof(rec->m_categoryEnumID));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0028])];
    rec->m_iconFile = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
}
#endif
