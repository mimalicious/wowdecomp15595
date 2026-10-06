#include "WoW/Source/DB/Rec/ParticleColorRec.h"

// STATIC: %0x00B43698
const DBCMeta ParticleColorRec::s_meta = { "DBFilesClient\\ParticleColor.dbc", 4, 40, ParticleColorRec::FixupStrings, 0 };

// PROVEN: ab3598b8 IDENTICAL
// OFFSET: 0x00401000
void ParticleColorRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ParticleColorRec::ConvertRow(ParticleColorRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_start, &row[0x0004], sizeof(rec->m_start));
    memcpy(rec->m_mid, &row[0x0010], sizeof(rec->m_mid));
    memcpy(rec->m_end, &row[0x001C], sizeof(rec->m_end));
}
#endif
