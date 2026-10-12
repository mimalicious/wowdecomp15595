#include "WoW/Source/DB/Rec/CinematicCameraRec.h"

// STATIC: 0x00C263A4/0x140A1C278
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 24 };

// STATIC: 0x00C263B8/0x140A1C290
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 12, 4 };

// STATIC: 0x00C263CC/0x140A1C2A8
static const int32_t s_fieldTypes[5] = { 0, 2, 0, 3, 3 };

// STATIC: 0x00C263E0/0x140A1C2C0
const DBCMeta CinematicCameraRec::s_meta = { "DBFilesClient\\CinematicCamera.dbc", 5, 28, CinematicCameraRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void CinematicCameraRec::FixupStrings(void* record, const char* strings) {
    CinematicCameraRec* rec = static_cast<CinematicCameraRec*>(record);
    DBCFixupString(rec->m_model, strings);
}
