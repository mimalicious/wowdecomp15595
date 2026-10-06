#include "WoW/Source/DB/Rec/MountCapabilityRec.h"

// STATIC: 0x00C23380/0x140A185B0
const DBCMeta MountCapabilityRec::s_meta = { "DBFilesClient\\MountCapability.dbc", 8, 32, MountCapabilityRec::FixupStrings, 0 };

// PROVEN: 921506f4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MountCapabilityRec::FixupStrings(void* record, const char* strings) {
}
