#include "WoW/Source/DB/Rec/DanceMovesRec.h"

// STATIC: 0x00C25DB0/0x140A1BB50
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C25DD0/0x140A1BB70
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C25DF0/0x140A1BB90
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 2, 2, 0 };

// STATIC: 0x00C25E10/0x140A1BBB0
const DBCMeta DanceMovesRec::s_meta = { "DBFilesClient\\DanceMoves.dbc", 8, 32, DanceMovesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 3727bc78 IDENTICAL/NONE
// OFFSET: 0x00A2EB30/0x1407D7E90
void DanceMovesRec::FixupStrings(void* record, const char* strings) {
    DanceMovesRec* rec = static_cast<DanceMovesRec*>(record);
    DBCFixupString(rec->m_internalName, strings);
    DBCFixupString(rec->m_name, strings);
}
