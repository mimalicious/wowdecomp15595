#include "WoW/Source/DB/Rec/UnitBloodRec.h"

// STATIC: 0x00C1FDA0/0x140A14440
const DBCMeta UnitBloodRec::s_meta = { "DBFilesClient\\UnitBlood.dbc", 4, 40, UnitBloodRec::FixupStrings, 0 };

// PROVEN: e42d2d09 IDENTICAL/NONE
// OFFSET: 0x00A2E950/0x1407D7D30
void UnitBloodRec::FixupStrings(void* record, const char* strings) {
    UnitBloodRec* rec = static_cast<UnitBloodRec*>(record);
    DBCFixupString(rec->m_groundBlood[0], strings);
    DBCFixupString(rec->m_groundBlood[1], strings);
    DBCFixupString(rec->m_groundBlood[2], strings);
    DBCFixupString(rec->m_groundBlood[3], strings);
    DBCFixupString(rec->m_groundBlood[4], strings);
}
