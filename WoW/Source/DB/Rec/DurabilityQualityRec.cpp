#include "WoW/Source/DB/Rec/DurabilityQualityRec.h"

// STATIC: 0x00C25970/0x140A1B630
const DBCMeta DurabilityQualityRec::s_meta = { "DBFilesClient\\DurabilityQuality.dbc", 2, 8, DurabilityQualityRec::FixupStrings, 0 };

// PROVEN: 53e9c4e7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DurabilityQualityRec::FixupStrings(void* record, const char* strings) {
}
