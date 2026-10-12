#include "WoW/Source/DB/Rec/PowerDisplayRec.h"

// STATIC: 0x00C22EC8/0x140A17FA8
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 13, 14 };

// STATIC: 0x00C22EE0/0x140A17FC0
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 1, 1, 1 };

// STATIC: 0x00C22EF8/0x140A17FD8
static const int32_t s_fieldTypes[6] = { 0, 0, 2, 0, 0, 0 };

// STATIC: 0x00C22F10/0x140A17FF0
const DBCMeta PowerDisplayRec::s_meta = { "DBFilesClient\\PowerDisplay.dbc", 6, 16, PowerDisplayRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECA0/0x1407D8230
void PowerDisplayRec::FixupStrings(void* record, const char* strings) {
    PowerDisplayRec* rec = static_cast<PowerDisplayRec*>(record);
    DBCFixupString(rec->m_globalStringBaseTag, strings);
}
