#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"

// STATIC: 0x00C1D808/0x140A117A8
static const int32_t s_fieldOffsets[11] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 76, 80 };

// STATIC: 0x00C1D834/0x140A117D8
static const int32_t s_fieldSizes[11] = { 4, 4, 4, 4, 4, 4, 4, 4, 44, 4, 4 };

// STATIC: 0x00C1D860/0x140A11808
static const int32_t s_fieldTypes[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 };

// STATIC: 0x00C1D88C/0x140A11838
const DBCMeta CreatureDisplayInfoExtraRec::s_meta = { "DBFilesClient\\CreatureDisplayInfoExtra.dbc", 11, 84, CreatureDisplayInfoExtraRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E840/0x1407D7950
void CreatureDisplayInfoExtraRec::FixupStrings(void* record, const char* strings) {
    CreatureDisplayInfoExtraRec* rec = static_cast<CreatureDisplayInfoExtraRec*>(record);
    DBCFixupString(rec->m_bakeName, strings);
}
