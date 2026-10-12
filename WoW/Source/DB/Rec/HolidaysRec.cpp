#include "WoW/Source/DB/Rec/HolidaysRec.h"

// STATIC: 0x00C249D8/0x140A1A240
static const int32_t s_fieldOffsets[12] = { 0, 4, 44, 148, 152, 156, 196, 200, 204, 208, 212, 216 };

// STATIC: 0x00C24A08/0x140A1A270
static const int32_t s_fieldSizes[12] = { 4, 40, 104, 4, 4, 40, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C24A38/0x140A1A2A0
static const int32_t s_fieldTypes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0 };

// STATIC: 0x00C24A68/0x140A1A2D0
const DBCMeta HolidaysRec::s_meta = { "DBFilesClient\\Holidays.dbc", 12, 220, HolidaysRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7e16b22d IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EB80/0x1407D8350
void HolidaysRec::FixupStrings(void* record, const char* strings) {
    HolidaysRec* rec = static_cast<HolidaysRec*>(record);
    DBCFixupString(rec->m_textureFileName, strings);
}
