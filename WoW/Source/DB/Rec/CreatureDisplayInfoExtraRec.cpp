#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"

// STATIC: %0x00B43DC8
const DBCMeta CreatureDisplayInfoExtraRec::s_meta = { "DBFilesClient\\CreatureDisplayInfoExtra.dbc", 11, 84, CreatureDisplayInfoExtraRec::FixupStrings, 0 };

// OFFSET: %0x0097C070
void CreatureDisplayInfoExtraRec::FixupStrings(void* record, const char* strings) {
    CreatureDisplayInfoExtraRec* rec = static_cast<CreatureDisplayInfoExtraRec*>(record);
    rec->m_bakeName = &strings[reinterpret_cast<uintptr_t>(rec->m_bakeName)];
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureDisplayInfoExtraRec::ConvertRow(CreatureDisplayInfoExtraRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_displayRaceID, &row[0x0004], sizeof(rec->m_displayRaceID));
    memcpy(&rec->m_displaySexID, &row[0x0008], sizeof(rec->m_displaySexID));
    memcpy(&rec->m_skinID, &row[0x000C], sizeof(rec->m_skinID));
    memcpy(&rec->m_faceID, &row[0x0010], sizeof(rec->m_faceID));
    memcpy(&rec->m_hairStyleID, &row[0x0014], sizeof(rec->m_hairStyleID));
    memcpy(&rec->m_hairColorID, &row[0x0018], sizeof(rec->m_hairColorID));
    memcpy(&rec->m_facialHairID, &row[0x001C], sizeof(rec->m_facialHairID));
    memcpy(rec->m_npcitemDisplay, &row[0x0020], sizeof(rec->m_npcitemDisplay));
    memcpy(&rec->m_flags, &row[0x004C], sizeof(rec->m_flags));
    rec->m_bakeName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0050])];
}
#endif
