#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"

// STATIC: 0x00C1CB90/0x140A108D0
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C1CBB4/0x140A108F8
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 1, 4 };

// STATIC: 0x00C1CBD8/0x140A10920
static const int32_t s_fieldTypes[9] = { 0, 2, 2, 3, 3, 3, 3, 0, 3 };

// STATIC: 0x00C1CBFC/0x140A10948
const DBCMeta SpellVisualEffectNameRec::s_meta = { "DBFilesClient\\SpellVisualEffectName.dbc", 9, 36, SpellVisualEffectNameRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 422f2cd3 IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void SpellVisualEffectNameRec::FixupStrings(void* record, const char* strings) {
    SpellVisualEffectNameRec* rec = static_cast<SpellVisualEffectNameRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_fileName, strings);
}
