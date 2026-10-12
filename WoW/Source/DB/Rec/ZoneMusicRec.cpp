#include "WoW/Source/DB/Rec/ZoneMusicRec.h"

// STATIC: 0x00C1EF04/0x140A13358
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 16, 24 };

// STATIC: 0x00C1EF18/0x140A13370
static const int32_t s_fieldSizes[5] = { 4, 4, 8, 8, 8 };

// STATIC: 0x00C1EF2C/0x140A13388
static const int32_t s_fieldTypes[5] = { 0, 2, 0, 0, 0 };

// STATIC: 0x00C1EF40/0x140A133A0
const DBCMeta ZoneMusicRec::s_meta = { "DBFilesClient\\ZoneMusic.dbc", 5, 32, ZoneMusicRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 747ce2dd IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void ZoneMusicRec::FixupStrings(void* record, const char* strings) {
    ZoneMusicRec* rec = static_cast<ZoneMusicRec*>(record);
    DBCFixupString(rec->m_setName, strings);
}
