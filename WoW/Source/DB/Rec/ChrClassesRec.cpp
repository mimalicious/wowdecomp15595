#include "WoW/Source/DB/Rec/ChrClassesRec.h"

// STATIC: 0x00C1E5D4/0x140A12838
static const int32_t s_fieldOffsets[14] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C1E60C/0x140A12870
static const int32_t s_fieldSizes[14] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1E644/0x140A128A8
static const int32_t s_fieldTypes[14] = { 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1E67C/0x140A128E0
const DBCMeta ChrClassesRec::s_meta = { "DBFilesClient\\ChrClasses.dbc", 14, 56, ChrClassesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E8C0/0x1407D7B10
void ChrClassesRec::FixupStrings(void* record, const char* strings) {
    ChrClassesRec* rec = static_cast<ChrClassesRec*>(record);
    DBCFixupString(rec->m_petNameToken, strings);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameFemale, strings);
    DBCFixupString(rec->m_nameMale, strings);
    DBCFixupString(rec->m_filename, strings);
}
