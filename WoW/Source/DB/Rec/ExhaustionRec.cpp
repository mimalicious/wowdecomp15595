#include "WoW/Source/DB/Rec/ExhaustionRec.h"

// STATIC: 0x00C2566C/0x140A1B268
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C25688/0x140A1B288
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C256A4/0x140A1B2A8
static const int32_t s_fieldTypes[7] = { 0, 0, 3, 3, 3, 2, 3 };

// STATIC: 0x00C256C0/0x140A1B2C8
const DBCMeta ExhaustionRec::s_meta = { "DBFilesClient\\Exhaustion.dbc", 7, 28, ExhaustionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBF0/0x1407D84B0
void ExhaustionRec::FixupStrings(void* record, const char* strings) {
    ExhaustionRec* rec = static_cast<ExhaustionRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
