#include "WoW/Source/DB/Rec/FactionGroupRec.h"

// STATIC: 0x00C1E408/0x140A12630
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1E418/0x140A12640
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1E428/0x140A12650
static const int32_t s_fieldTypes[4] = { 0, 0, 2, 2 };

// STATIC: 0x00C1E438/0x140A12660
const DBCMeta FactionGroupRec::s_meta = { "DBFilesClient\\FactionGroup.dbc", 4, 16, FactionGroupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6ff3adc3 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EC50/0x1407D8590
void FactionGroupRec::FixupStrings(void* record, const char* strings) {
    FactionGroupRec* rec = static_cast<FactionGroupRec*>(record);
    DBCFixupString(rec->m_internalName, strings);
    DBCFixupString(rec->m_name, strings);
}
