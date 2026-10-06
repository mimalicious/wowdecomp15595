#include "WoW/Source/DB/Rec/CinematicCameraRec.h"

// STATIC: %0x00B46F84
const DBCMeta CinematicCameraRec::s_meta = { "DBFilesClient\\CinematicCamera.dbc", 5, 28, CinematicCameraRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void CinematicCameraRec::FixupStrings(void* record, const char* strings) {
    CinematicCameraRec* rec = static_cast<CinematicCameraRec*>(record);
    rec->m_model = &strings[reinterpret_cast<uintptr_t>(rec->m_model)];
}

#if defined(WOWDECOMP_ARCH_64)
void CinematicCameraRec::ConvertRow(CinematicCameraRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_model = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_soundID, &row[0x0008], sizeof(rec->m_soundID));
    memcpy(rec->m_origin, &row[0x000C], sizeof(rec->m_origin));
    memcpy(&rec->m_originFacing, &row[0x0018], sizeof(rec->m_originFacing));
}
#endif
