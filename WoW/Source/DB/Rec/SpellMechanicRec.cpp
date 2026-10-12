#include "WoW/Source/DB/Rec/SpellMechanicRec.h"

// STATIC: 0x00C21000/0x140A15A80
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C21008/0x140A15A88
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C21010/0x140A15A90
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C21018/0x140A15A98
const DBCMeta SpellMechanicRec::s_meta = { "DBFilesClient\\SpellMechanic.dbc", 2, 8, SpellMechanicRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellMechanicRec::FixupStrings(void* record, const char* strings) {
    SpellMechanicRec* rec = static_cast<SpellMechanicRec*>(record);
    DBCFixupString(rec->m_stateName, strings);
}
