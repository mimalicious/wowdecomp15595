#include "WoW/Source/DB/Rec/ResearchBranchRec.h"

// STATIC: 0x00C22BC4/0x140A17BD8
const DBCMeta ResearchBranchRec::s_meta = { "DBFilesClient\\ResearchBranch.dbc", 6, 24, ResearchBranchRec::FixupStrings, 0 };

// PROVEN: efa2be2a IDENTICAL/NONE
// OFFSET: 0x00A2EA60/0x1407D7ED0
void ResearchBranchRec::FixupStrings(void* record, const char* strings) {
    ResearchBranchRec* rec = static_cast<ResearchBranchRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
    DBCFixupString(rec->unk_0010, strings);
}
