#include "WoW/Source/DB/Rec/CameraModeRec.h"

// STATIC: 0x00C26734/0x140A1C6D8
const DBCMeta CameraModeRec::s_meta = { "DBFilesClient\\CameraMode.dbc", 13, 68, CameraModeRec::FixupStrings, 0 };

// PROVEN: ded4def3 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void CameraModeRec::FixupStrings(void* record, const char* strings) {
    CameraModeRec* rec = static_cast<CameraModeRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
