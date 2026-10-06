#include "WoW/Source/DB/Rec/CinematicCameraRec.h"

// STATIC: 0x00C263E0/0x140A1C2C0
const DBCMeta CinematicCameraRec::s_meta = { "DBFilesClient\\CinematicCamera.dbc", 5, 28, CinematicCameraRec::FixupStrings, 0 };

// PROVEN: e16db06c IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void CinematicCameraRec::FixupStrings(void* record, const char* strings) {
    CinematicCameraRec* rec = static_cast<CinematicCameraRec*>(record);
    DBCFixupString(rec->m_model, strings);
}
