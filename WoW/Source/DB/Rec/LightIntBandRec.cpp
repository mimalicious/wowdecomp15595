#include "WoW/Source/DB/Rec/LightIntBandRec.h"

// STATIC: 0x00C2845C/0x140A1EC28
const DBCMeta LightIntBandRec::s_meta = { "DBFilesClient\\LightIntBand.dbc", 4, 136, LightIntBandRec::FixupStrings, 0 };

// PROVEN: 68653e19 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightIntBandRec::FixupStrings(void* record, const char* strings) {
}
