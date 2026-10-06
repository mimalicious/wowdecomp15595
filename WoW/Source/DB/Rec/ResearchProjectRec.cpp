#include "WoW/Source/DB/Rec/ResearchProjectRec.h"

// STATIC: 0x00C22AD4/0x140A17AA8
const DBCMeta ResearchProjectRec::s_meta = { "DBFilesClient\\ResearchProject.dbc", 9, 36, ResearchProjectRec::FixupStrings, 0 };

// PROVEN: 926c9a01 IDENTICAL/NONE
// OFFSET: 0x00A2EA40/0x1407D8050
void ResearchProjectRec::FixupStrings(void* record, const char* strings) {
    ResearchProjectRec* rec = static_cast<ResearchProjectRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
    DBCFixupString(rec->unk_0008, strings);
    DBCFixupString(rec->unk_001C, strings);
}
