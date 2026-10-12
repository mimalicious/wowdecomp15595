#include "WoW/Source/DB/Rec/MountCapabilityRec.h"

// STATIC: 0x00C23320/0x140A18550
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C23340/0x140A18570
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23360/0x140A18590
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C23380/0x140A185B0
const DBCMeta MountCapabilityRec::s_meta = { "DBFilesClient\\MountCapability.dbc", 8, 32, MountCapabilityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void MountCapabilityRec::FixupStrings(void* record, const char* strings) {
}
