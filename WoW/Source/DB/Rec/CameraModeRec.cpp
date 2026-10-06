#include "WoW/Source/DB/Rec/CameraModeRec.h"

// STATIC: 0x00C26698/0x140A1C630
static const int32_t s_fieldOffsets[13] = { 0, 4, 8, 12, 16, 28, 40, 44, 48, 52, 56, 60, 64 };

// STATIC: 0x00C266CC/0x140A1C668
static const int32_t s_fieldSizes[13] = { 4, 4, 4, 4, 12, 12, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26700/0x140A1C6A0
static const int32_t s_fieldTypes[13] = { 0, 2, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0 };

// STATIC: 0x00C26734/0x140A1C6D8
const DBCMeta CameraModeRec::s_meta = { "DBFilesClient\\CameraMode.dbc", 13, 68, CameraModeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ded4def3 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void CameraModeRec::FixupStrings(void* record, const char* strings) {
    CameraModeRec* rec = static_cast<CameraModeRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
