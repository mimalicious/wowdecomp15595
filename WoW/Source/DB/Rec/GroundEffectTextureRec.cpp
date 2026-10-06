#include "WoW/Source/DB/Rec/GroundEffectTextureRec.h"

// STATIC: %0x00B467C0
const DBCMeta GroundEffectTextureRec::s_meta = { "DBFilesClient\\GroundEffectTexture.dbc", 5, 44, GroundEffectTextureRec::FixupStrings, 0 };

// PROVEN: fcd7636a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GroundEffectTextureRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GroundEffectTextureRec::ConvertRow(GroundEffectTextureRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_doodadID, &row[0x0004], sizeof(rec->m_doodadID));
    memcpy(rec->m_doodadWeight, &row[0x0014], sizeof(rec->m_doodadWeight));
    memcpy(&rec->m_density, &row[0x0024], sizeof(rec->m_density));
    memcpy(&rec->m_sound, &row[0x0028], sizeof(rec->m_sound));
}
#endif
