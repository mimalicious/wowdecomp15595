#include "WoW/Source/DB/Rec/AreaPOIRec.h"

// STATIC: 0x00C26BFC/0x140A1CCC0
static const int32_t s_fieldOffsets[13] = { 0, 4, 8, 44, 48, 56, 60, 64, 68, 72, 76, 80, 84 };

// STATIC: 0x00C26C30/0x140A1CCF8
static const int32_t s_fieldSizes[13] = { 4, 4, 36, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26C64/0x140A1CD30
static const int32_t s_fieldTypes[13] = { 0, 0, 0, 0, 3, 0, 0, 0, 2, 2, 0, 0, 0 };

// STATIC: 0x00C26C98/0x140A1CD68
const DBCMeta AreaPOIRec::s_meta = { "DBFilesClient\\AreaPOI.dbc", 13, 88, AreaPOIRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EC80/0x1407D8610
void AreaPOIRec::FixupStrings(void* record, const char* strings) {
    AreaPOIRec* rec = static_cast<AreaPOIRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
