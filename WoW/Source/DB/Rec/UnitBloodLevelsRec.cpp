#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"

// STATIC: 0x00C1FDFC/0x140A144B8
const DBCMeta UnitBloodLevelsRec::s_meta = { "DBFilesClient\\UnitBloodLevels.dbc", 2, 16, UnitBloodLevelsRec::FixupStrings, 0 };

// PROVEN: f2a7833a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void UnitBloodLevelsRec::FixupStrings(void* record, const char* strings) {
}
