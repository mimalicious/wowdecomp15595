#include "WoW/Source/DB/Rec/SpellVisualKitModelAttachRec.h"

// STATIC: 0x00C205A8/0x140A14E28
static const int32_t s_fieldOffsets[12] = { 0, 4, 8, 12, 16, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C205D8/0x140A14E58
static const int32_t s_fieldSizes[12] = { 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C20608/0x140A14E88
static const int32_t s_fieldTypes[12] = { 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0 };

// STATIC: 0x00C20638/0x140A14EB8
const DBCMeta SpellVisualKitModelAttachRec::s_meta = { "DBFilesClient\\SpellVisualKitModelAttach.dbc", 12, 56, SpellVisualKitModelAttachRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellVisualKitModelAttachRec::FixupStrings(void* record, const char* strings) {
}
