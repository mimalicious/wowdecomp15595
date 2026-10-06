#include "WoW/Source/DB/Rec/SkillLineRec.h"

// STATIC: 0x00C22438/0x140A172B0
const DBCMeta SkillLineRec::s_meta = { "DBFilesClient\\SkillLine.dbc", 7, 28, SkillLineRec::FixupStrings, 0 };

// OFFSET: 0x00A2EA20/0x1407D7FF0
void SkillLineRec::FixupStrings(void* record, const char* strings) {
    SkillLineRec* rec = static_cast<SkillLineRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_alternateVerb, strings);
}
