#include "WoW/Source/DB/Rec/NamesReservedRec.h"

// STATIC: 0x00C1DFC8/0x140A120C8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1DFD4/0x140A120D8
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1DFE0/0x140A120E8
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C1DFEC/0x140A120F8
const DBCMeta NamesReservedRec::s_meta = { "DBFilesClient\\NamesReserved.dbc", 3, 12, NamesReservedRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void NamesReservedRec::FixupStrings(void* record, const char* strings) {
    NamesReservedRec* rec = static_cast<NamesReservedRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
