#include "WoW/Source/DB/Rec/CameraShakesRec.h"

// STATIC: %0x00B47058
const DBCMeta CameraShakesRec::s_meta = { "DBFilesClient\\CameraShakes.dbc", 8, 32, CameraShakesRec::FixupStrings, 0 };

// PROVEN: 2b4e458b IDENTICAL
// OFFSET: 0x00401000
void CameraShakesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CameraShakesRec::ConvertRow(CameraShakesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_shakeType, &row[0x0004], sizeof(rec->m_shakeType));
    memcpy(&rec->m_direction, &row[0x0008], sizeof(rec->m_direction));
    memcpy(&rec->m_amplitude, &row[0x000C], sizeof(rec->m_amplitude));
    memcpy(&rec->m_frequency, &row[0x0010], sizeof(rec->m_frequency));
    memcpy(&rec->m_duration, &row[0x0014], sizeof(rec->m_duration));
    memcpy(&rec->m_phase, &row[0x0018], sizeof(rec->m_phase));
    memcpy(&rec->m_coefficient, &row[0x001C], sizeof(rec->m_coefficient));
}
#endif
