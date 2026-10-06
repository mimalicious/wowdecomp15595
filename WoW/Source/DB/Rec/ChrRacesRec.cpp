#include "WoW/Source/DB/Rec/ChrRacesRec.h"

// STATIC: 0x00C1E594/0x140A127E0
const DBCMeta ChrRacesRec::s_meta = { "DBFilesClient\\ChrRaces.dbc", 23, 96, ChrRacesRec::FixupStrings, 0 };

// PROVEN: eea3b520 IDENTICAL/NONE
// OFFSET: 0x00A2E890/0x1407D7A30
void ChrRacesRec::FixupStrings(void* record, const char* strings) {
    ChrRacesRec* rec = static_cast<ChrRacesRec*>(record);
    DBCFixupString(rec->m_clientPrefix, strings);
    DBCFixupString(rec->m_clientFileString, strings);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameFemale, strings);
    DBCFixupString(rec->m_nameMale, strings);
    DBCFixupString(rec->m_facialHairCustomization[0], strings);
    DBCFixupString(rec->m_facialHairCustomization[1], strings);
    DBCFixupString(rec->m_hairCustomization, strings);
}
