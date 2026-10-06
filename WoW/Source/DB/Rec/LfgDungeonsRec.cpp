#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"

// STATIC: %0x00B45DC8
const DBCMeta LfgDungeonsRec::s_meta = { "DBFilesClient\\LfgDungeons.dbc", 17, 68, LfgDungeonsRec::FixupStrings, 0 };

// OFFSET: %0x0097C360
void LfgDungeonsRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonsRec* rec = static_cast<LfgDungeonsRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_textureFilename = &strings[reinterpret_cast<uintptr_t>(rec->m_textureFilename)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void LfgDungeonsRec::ConvertRow(LfgDungeonsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_minLevel, &row[0x0008], sizeof(rec->m_minLevel));
    memcpy(&rec->m_maxLevel, &row[0x000C], sizeof(rec->m_maxLevel));
    memcpy(&rec->m_targetLevel, &row[0x0010], sizeof(rec->m_targetLevel));
    memcpy(&rec->m_targetLevelMin, &row[0x0014], sizeof(rec->m_targetLevelMin));
    memcpy(&rec->m_targetLevelMax, &row[0x0018], sizeof(rec->m_targetLevelMax));
    memcpy(&rec->m_mapID, &row[0x001C], sizeof(rec->m_mapID));
    memcpy(&rec->m_difficulty, &row[0x0020], sizeof(rec->m_difficulty));
    memcpy(&rec->m_flags, &row[0x0024], sizeof(rec->m_flags));
    memcpy(&rec->m_typeID, &row[0x0028], sizeof(rec->m_typeID));
    memcpy(&rec->m_faction, &row[0x002C], sizeof(rec->m_faction));
    rec->m_textureFilename = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0030])];
    memcpy(&rec->m_expansionLevel, &row[0x0034], sizeof(rec->m_expansionLevel));
    memcpy(&rec->m_orderIndex, &row[0x0038], sizeof(rec->m_orderIndex));
    memcpy(&rec->m_groupID, &row[0x003C], sizeof(rec->m_groupID));
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0040])];
}
#endif
