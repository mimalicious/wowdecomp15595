#include "WoW/Source/DB/Rec/DanceMovesRec.h"

// STATIC: 0x00C25E10/0x140A1BBB0
const DBCMeta DanceMovesRec::s_meta = { "DBFilesClient\\DanceMoves.dbc", 8, 32, DanceMovesRec::FixupStrings, 0 };

// PROVEN: 3727bc78 IDENTICAL/NONE
// OFFSET: 0x00A2EB30/0x1407D7E90
void DanceMovesRec::FixupStrings(void* record, const char* strings) {
    DanceMovesRec* rec = static_cast<DanceMovesRec*>(record);
    DBCFixupString(rec->m_internalName, strings);
    DBCFixupString(rec->m_name, strings);
}
