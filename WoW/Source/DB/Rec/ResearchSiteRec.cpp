#include "WoW/Source/DB/Rec/ResearchSiteRec.h"

// STATIC: 0x00C22A24/0x140A179D0
const DBCMeta ResearchSiteRec::s_meta = { "DBFilesClient\\ResearchSite.dbc", 5, 20, ResearchSiteRec::FixupStrings, 0 };

// PROVEN: a8f77e39 IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void ResearchSiteRec::FixupStrings(void* record, const char* strings) {
    ResearchSiteRec* rec = static_cast<ResearchSiteRec*>(record);
    DBCFixupString(rec->unk_000C, strings);
}
