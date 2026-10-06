#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"

// STATIC: 0x00C20494/0x140A14CC8
const DBCMeta SummonPropertiesRec::s_meta = { "DBFilesClient\\SummonProperties.dbc", 6, 24, SummonPropertiesRec::FixupStrings, 0 };

// PROVEN: 8c994a57 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SummonPropertiesRec::FixupStrings(void* record, const char* strings) {
}
