#include "WoW/Source/DB/Rec/TransportPhysicsRec.h"

// STATIC: %0x00B44874
const DBCMeta TransportPhysicsRec::s_meta = { "DBFilesClient\\TransportPhysics.dbc", 11, 44, TransportPhysicsRec::FixupStrings, 0 };

// PROVEN: 0416542b IDENTICAL
// OFFSET: 0x00401000
void TransportPhysicsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TransportPhysicsRec::ConvertRow(TransportPhysicsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_waveAmp, &row[0x0004], sizeof(rec->m_waveAmp));
    memcpy(&rec->m_waveTimeScale, &row[0x0008], sizeof(rec->m_waveTimeScale));
    memcpy(&rec->m_rollAmp, &row[0x000C], sizeof(rec->m_rollAmp));
    memcpy(&rec->m_rollTimeScale, &row[0x0010], sizeof(rec->m_rollTimeScale));
    memcpy(&rec->m_pitchAmp, &row[0x0014], sizeof(rec->m_pitchAmp));
    memcpy(&rec->m_pitchTimeScale, &row[0x0018], sizeof(rec->m_pitchTimeScale));
    memcpy(&rec->m_maxBank, &row[0x001C], sizeof(rec->m_maxBank));
    memcpy(&rec->m_maxBankTurnSpeed, &row[0x0020], sizeof(rec->m_maxBankTurnSpeed));
    memcpy(&rec->m_speedDampThresh, &row[0x0024], sizeof(rec->m_speedDampThresh));
    memcpy(&rec->m_speedDamp, &row[0x0028], sizeof(rec->m_speedDamp));
}
#endif
