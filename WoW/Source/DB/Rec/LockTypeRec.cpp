#include "WoW/Source/DB/Rec/LockTypeRec.h"

// STATIC: 0x00C2352C/0x140A187E8
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C23540/0x140A18800
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C23554/0x140A18818
static const int32_t s_fieldTypes[5] = { 0, 2, 2, 2, 2 };

// STATIC: 0x00C23568/0x140A18830
const DBCMeta LockTypeRec::s_meta = { "DBFilesClient\\LockType.dbc", 5, 20, LockTypeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 059e0598 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EAB0/0x1407D8150
void LockTypeRec::FixupStrings(void* record, const char* strings) {
    LockTypeRec* rec = static_cast<LockTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_resourceName, strings);
    DBCFixupString(rec->m_verb, strings);
    DBCFixupString(rec->m_cursorName, strings);
}
