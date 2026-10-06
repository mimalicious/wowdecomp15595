#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"

// STATIC: 0x00C1D5A8/0x140A11500
static const int32_t s_fieldOffsets[27] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 80, 92, 96, 100, 104, 108, 112, 116, 120 };

// STATIC: 0x00C1D618/0x140A11570
static const int32_t s_fieldSizes[27] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 12, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1D688/0x140A115E0
static const int32_t s_fieldTypes[27] = { 0, 0, 2, 0, 3, 0, 0, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

// STATIC: 0x00C1D6F4/0x140A11650
const DBCMeta CreatureModelDataRec::s_meta = { "DBFilesClient\\CreatureModelData.dbc", 27, 124, CreatureModelDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7d549e20 IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void CreatureModelDataRec::FixupStrings(void* record, const char* strings) {
    CreatureModelDataRec* rec = static_cast<CreatureModelDataRec*>(record);
    DBCFixupString(rec->m_modelName, strings);
}
