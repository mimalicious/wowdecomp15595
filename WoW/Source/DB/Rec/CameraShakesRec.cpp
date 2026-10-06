#include "WoW/Source/DB/Rec/CameraShakesRec.h"

// STATIC: 0x00C26658/0x140A1C5D8
const DBCMeta CameraShakesRec::s_meta = { "DBFilesClient\\CameraShakes.dbc", 9, 36, CameraShakesRec::FixupStrings, 0 };

// PROVEN: 2b4e458b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CameraShakesRec::FixupStrings(void* record, const char* strings) {
}
