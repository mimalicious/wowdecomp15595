#include "WoW/Source/DB/Rec/FactionRec.h"

// STATIC: %0x00B46AC4
const DBCMeta FactionRec::s_meta = { "DBFilesClient\\Faction.dbc", 12, 104, FactionRec::FixupStrings, 0 };

// OFFSET: %0x0097C3F0
void FactionRec::FixupStrings(void* record, const char* strings) {
    FactionRec* rec = static_cast<FactionRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void FactionRec::ConvertRow(FactionRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_reputationIndex, &row[0x0004], sizeof(rec->m_reputationIndex));
    memcpy(rec->m_reputationRaceMask, &row[0x0008], sizeof(rec->m_reputationRaceMask));
    memcpy(rec->m_reputationClassMask, &row[0x0018], sizeof(rec->m_reputationClassMask));
    memcpy(rec->m_reputationBase, &row[0x0028], sizeof(rec->m_reputationBase));
    memcpy(rec->m_reputationFlags, &row[0x0038], sizeof(rec->m_reputationFlags));
    memcpy(&rec->m_parentFactionID, &row[0x0048], sizeof(rec->m_parentFactionID));
    memcpy(rec->m_parentFactionMod, &row[0x004C], sizeof(rec->m_parentFactionMod));
    memcpy(rec->m_parentFactionCap, &row[0x0054], sizeof(rec->m_parentFactionCap));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x005C])];
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0060])];
    memcpy(&rec->unk_0064, &row[0x0064], sizeof(rec->unk_0064));
}
#endif
