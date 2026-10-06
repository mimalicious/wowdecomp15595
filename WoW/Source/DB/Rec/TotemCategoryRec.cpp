#include "WoW/Source/DB/Rec/TotemCategoryRec.h"

// STATIC: 0x00C2002C/0x140A14768
const DBCMeta TotemCategoryRec::s_meta = { "DBFilesClient\\TotemCategory.dbc", 4, 16, TotemCategoryRec::FixupStrings, 0 };

// PROVEN: f4176b26 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void TotemCategoryRec::FixupStrings(void* record, const char* strings) {
    TotemCategoryRec* rec = static_cast<TotemCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
