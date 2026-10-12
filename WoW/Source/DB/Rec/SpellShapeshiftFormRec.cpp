#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"

// STATIC: 0x00C20A20/0x140A15370
static const int32_t s_fieldOffsets[11] = { 0, 4, 8, 12, 16, 20, 24, 28, 44, 76, 80 };

// STATIC: 0x00C20A4C/0x140A153A0
static const int32_t s_fieldSizes[11] = { 4, 4, 4, 4, 4, 4, 4, 16, 32, 4, 4 };

// STATIC: 0x00C20A78/0x140A153D0
static const int32_t s_fieldTypes[11] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C20AA4/0x140A15400
const DBCMeta SpellShapeshiftFormRec::s_meta = { "DBFilesClient\\SpellShapeshiftForm.dbc", 11, 84, SpellShapeshiftFormRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7e1267de IDENTICAL/DIFFERENT
// OFFSET: 0x00A2ECA0/0x1407D8230
void SpellShapeshiftFormRec::FixupStrings(void* record, const char* strings) {
    SpellShapeshiftFormRec* rec = static_cast<SpellShapeshiftFormRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
