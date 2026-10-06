#include "WoW/Source/DB/Rec/PetPersonalityRec.h"

// STATIC: %0x00B45A48
const DBCMeta PetPersonalityRec::s_meta = { "DBFilesClient\\PetPersonality.dbc", 4, 32, PetPersonalityRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void PetPersonalityRec::FixupStrings(void* record, const char* strings) {
    PetPersonalityRec* rec = static_cast<PetPersonalityRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void PetPersonalityRec::ConvertRow(PetPersonalityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(rec->m_happinessThreshold, &row[0x0008], sizeof(rec->m_happinessThreshold));
    memcpy(rec->m_happinessDamage, &row[0x0014], sizeof(rec->m_happinessDamage));
}
#endif
