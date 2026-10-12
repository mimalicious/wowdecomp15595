#include "WoW/Source/DB/Rec/ZoneIntroMusicTableRec.h"

// STATIC: 0x00C1EF88/0x140A13400
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C1EF9C/0x140A13418
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C1EFB0/0x140A13430
static const int32_t s_fieldTypes[5] = { 0, 2, 0, 0, 0 };

// STATIC: 0x00C1EFC4/0x140A13448
const DBCMeta ZoneIntroMusicTableRec::s_meta = { "DBFilesClient\\ZoneIntroMusicTable.dbc", 5, 20, ZoneIntroMusicTableRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 46979e0c IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void ZoneIntroMusicTableRec::FixupStrings(void* record, const char* strings) {
    ZoneIntroMusicTableRec* rec = static_cast<ZoneIntroMusicTableRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
