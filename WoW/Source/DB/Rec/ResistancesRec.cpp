#include "WoW/Source/DB/Rec/ResistancesRec.h"

// STATIC: 0x00C1DF58/0x140A12040
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1DF68/0x140A12050
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1DF78/0x140A12060
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 2 };

// STATIC: 0x00C1DF88/0x140A12070
const DBCMeta ResistancesRec::s_meta = { "DBFilesClient\\Resistances.dbc", 4, 16, ResistancesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6d0da23e IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void ResistancesRec::FixupStrings(void* record, const char* strings) {
    ResistancesRec* rec = static_cast<ResistancesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
