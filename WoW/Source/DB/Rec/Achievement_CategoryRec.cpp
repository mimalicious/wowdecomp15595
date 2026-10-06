#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"

// STATIC: 0x00C26D70/0x140A1CE80
const DBCMeta Achievement_CategoryRec::s_meta = { "DBFilesClient\\Achievement_Category.dbc", 4, 16, Achievement_CategoryRec::FixupStrings, 0 };

// PROVEN: d8bfa3b6 IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void Achievement_CategoryRec::FixupStrings(void* record, const char* strings) {
    Achievement_CategoryRec* rec = static_cast<Achievement_CategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
