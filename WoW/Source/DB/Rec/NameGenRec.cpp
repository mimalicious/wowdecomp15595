#include "WoW/Source/DB/Rec/NameGenRec.h"

// STATIC: 0x00C1D068/0x140A10E90
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1D078/0x140A10EA0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1D088/0x140A10EB0
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C1D098/0x140A10EC0
const DBCMeta NameGenRec::s_meta = { "DBFilesClient\\NameGen.dbc", 4, 16, NameGenRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 8eb17c2d IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void NameGenRec::FixupStrings(void* record, const char* strings) {
    NameGenRec* rec = static_cast<NameGenRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
