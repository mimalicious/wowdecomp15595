#include "WoW/Source/DB/Rec/TalentTabRec.h"

// STATIC: 0x00C20354/0x140A14B50
const DBCMeta TalentTabRec::s_meta = { "DBFilesClient\\TalentTab.dbc", 10, 44, TalentTabRec::FixupStrings, 0 };

// PROVEN: 10a1d5fb IDENTICAL/NONE
// OFFSET: 0x00A2E970/0x1407D7DC0
void TalentTabRec::FixupStrings(void* record, const char* strings) {
    TalentTabRec* rec = static_cast<TalentTabRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_backgroundFile, strings);
    DBCFixupString(rec->unk_001C, strings);
}
