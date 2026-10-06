#include "WoW/Source/DB/Rec/AreaPOIRec.h"

// STATIC: 0x00C26C98/0x140A1CD68
const DBCMeta AreaPOIRec::s_meta = { "DBFilesClient\\AreaPOI.dbc", 13, 88, AreaPOIRec::FixupStrings, 0 };

// PROVEN: 8d6aaa81 IDENTICAL/NONE
// OFFSET: 0x00A2EC80/0x1407D8610
void AreaPOIRec::FixupStrings(void* record, const char* strings) {
    AreaPOIRec* rec = static_cast<AreaPOIRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
