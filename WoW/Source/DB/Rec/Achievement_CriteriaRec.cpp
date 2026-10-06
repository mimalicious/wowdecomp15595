#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"

// STATIC: 0x00C26EA4/0x140A1CFD0
const DBCMeta Achievement_CriteriaRec::s_meta = { "DBFilesClient\\Achievement_Criteria.dbc", 19, 96, Achievement_CriteriaRec::FixupStrings, 0 };

// OFFSET: 0x00A2ECB0/0x1407D8650
void Achievement_CriteriaRec::FixupStrings(void* record, const char* strings) {
    Achievement_CriteriaRec* rec = static_cast<Achievement_CriteriaRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
