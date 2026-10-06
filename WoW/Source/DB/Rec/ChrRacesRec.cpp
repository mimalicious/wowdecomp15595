#include "WoW/Source/DB/Rec/ChrRacesRec.h"

// STATIC: 0x00C1E478/0x140A126C0
static const int32_t s_fieldOffsets[23] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 76, 80, 84, 88, 92 };

// STATIC: 0x00C1E4D8/0x140A12720
static const int32_t s_fieldSizes[23] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1E538/0x140A12780
static const int32_t s_fieldTypes[23] = { 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0 };

// STATIC: 0x00C1E594/0x140A127E0
const DBCMeta ChrRacesRec::s_meta = { "DBFilesClient\\ChrRaces.dbc", 23, 96, ChrRacesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

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
