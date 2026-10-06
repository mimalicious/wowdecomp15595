#include "WoW/Source/DB/Rec/PhaseRec.h"

// STATIC: 0x00C23030/0x140A18178
const DBCMeta PhaseRec::s_meta = { "DBFilesClient\\Phase.dbc", 3, 12, PhaseRec::FixupStrings, 0 };

// PROVEN: f375221f IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void PhaseRec::FixupStrings(void* record, const char* strings) {
    PhaseRec* rec = static_cast<PhaseRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
