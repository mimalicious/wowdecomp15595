#include "WoW/Source/DB/Rec/LiquidObjectRec.h"

// STATIC: 0x00C27EEC/0x140A1E580
const DBCMeta LiquidObjectRec::s_meta = { "DBFilesClient\\LiquidObject.dbc", 6, 24, LiquidObjectRec::FixupStrings, 0 };

// PROVEN: eefa6274 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LiquidObjectRec::FixupStrings(void* record, const char* strings) {
}
