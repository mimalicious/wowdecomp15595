#include "WoW/Source/DB/Rec/Startup_StringsRec.h"

// STATIC: 0x00B8CD4C/0x14096B710
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00B8CD58/0x14096B720
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00B8CD64/0x14096B730
static const int32_t s_fieldTypes[3] = { 0, 2, 2 };

// STATIC: 0x00B8CD70/0x14096B740
const DBCMeta Startup_StringsRec::s_meta = { "DBFilesClient\\Startup_Strings.dbc", 3, 12, Startup_StringsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E9B0/0x1407D8110
void Startup_StringsRec::FixupStrings(void* record, const char* strings) {
    Startup_StringsRec* rec = static_cast<Startup_StringsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_message, strings);
}
