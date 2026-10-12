#include "WoW/Source/DB/Rec/SpellVisualKitAreaModelRec.h"

// STATIC: 0x00C20684/0x140A14F20
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C206A0/0x140A14F40
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C206BC/0x140A14F60
static const int32_t s_fieldTypes[7] = { 0, 2, 0, 0, 3, 3, 3 };

// STATIC: 0x00C206D8/0x140A14F80
const DBCMeta SpellVisualKitAreaModelRec::s_meta = { "DBFilesClient\\SpellVisualKitAreaModel.dbc", 7, 28, SpellVisualKitAreaModelRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ae7a67d9 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellVisualKitAreaModelRec::FixupStrings(void* record, const char* strings) {
    SpellVisualKitAreaModelRec* rec = static_cast<SpellVisualKitAreaModelRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
