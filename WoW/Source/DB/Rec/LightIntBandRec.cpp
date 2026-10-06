#include "WoW/Source/DB/Rec/LightIntBandRec.h"

// STATIC: 0x00C2842C/0x140A1EBF8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 72 };

// STATIC: 0x00C2843C/0x140A1EC08
static const int32_t s_fieldSizes[4] = { 4, 4, 64, 64 };

// STATIC: 0x00C2844C/0x140A1EC18
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C2845C/0x140A1EC28
const DBCMeta LightIntBandRec::s_meta = { "DBFilesClient\\LightIntBand.dbc", 4, 136, LightIntBandRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 68653e19 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightIntBandRec::FixupStrings(void* record, const char* strings) {
}
