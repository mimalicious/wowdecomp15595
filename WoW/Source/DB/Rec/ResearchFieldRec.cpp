#include "WoW/Source/DB/Rec/ResearchFieldRec.h"

// STATIC: 0x00C22B38/0x140A17B30
const DBCMeta ResearchFieldRec::s_meta = { "DBFilesClient\\ResearchField.dbc", 3, 12, ResearchFieldRec::FixupStrings, 0 };

// PROVEN: eecfd0a8 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ResearchFieldRec::FixupStrings(void* record, const char* strings) {
    ResearchFieldRec* rec = static_cast<ResearchFieldRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
