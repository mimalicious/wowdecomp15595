#include "WoW/Source/DB/Rec/gtNPCManaCostScalerRec.h"

// STATIC: 0x00C24E1C/0x140A1A7C0
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24E20/0x140A1A7C4
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24E24/0x140A1A7C8
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24E28/0x140A1A7D0
const DBCMeta gtNPCManaCostScalerRec::s_meta = { "DBFilesClient\\gtNPCManaCostScaler.dbc", 1, 8, gtNPCManaCostScalerRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 41825844 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtNPCManaCostScalerRec::FixupStrings(void* record, const char* strings) {
}
