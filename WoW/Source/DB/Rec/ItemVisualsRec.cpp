#include "WoW/Source/DB/Rec/ItemVisualsRec.h"

// STATIC: 0x00C1D1C0/0x140A11038
const DBCMeta ItemVisualsRec::s_meta = { "DBFilesClient\\ItemVisuals.dbc", 2, 24, ItemVisualsRec::FixupStrings, 0 };

// PROVEN: d392dd33 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemVisualsRec::FixupStrings(void* record, const char* strings) {
}
