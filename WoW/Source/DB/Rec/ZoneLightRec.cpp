#include "WoW/Source/DB/Rec/ZoneLightRec.h"

// STATIC: 0x00C27924/0x140A1DE98
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C27934/0x140A1DEA8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C27944/0x140A1DEB8
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C27954/0x140A1DEC8
const DBCMeta ZoneLightRec::s_meta = { "DBFilesClient\\ZoneLight.dbc", 4, 16, ZoneLightRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ZoneLightRec::FixupStrings(void* record, const char* strings) {
    ZoneLightRec* rec = static_cast<ZoneLightRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
