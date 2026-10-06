#include "WoW/Source/DB/Rec/ExhaustionRec.h"

// STATIC: 0x00C256C0/0x140A1B2C8
const DBCMeta ExhaustionRec::s_meta = { "DBFilesClient\\Exhaustion.dbc", 7, 28, ExhaustionRec::FixupStrings, 0 };

// PROVEN: a6a689e0 IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void ExhaustionRec::FixupStrings(void* record, const char* strings) {
    ExhaustionRec* rec = static_cast<ExhaustionRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
