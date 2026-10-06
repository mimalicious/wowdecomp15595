#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"

// STATIC: %0x00B450D8
const DBCMeta SpellEffectCameraShakesRec::s_meta = { "DBFilesClient\\SpellEffectCameraShakes.dbc", 2, 16, SpellEffectCameraShakesRec::FixupStrings, 0 };

// PROVEN: b53220ba IDENTICAL/NONE
// OFFSET: 0x00401000
void SpellEffectCameraShakesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellEffectCameraShakesRec::ConvertRow(SpellEffectCameraShakesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_cameraShake, &row[0x0004], sizeof(rec->m_cameraShake));
}
#endif
