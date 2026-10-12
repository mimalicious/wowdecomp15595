#include "WoW/Source/DB/Rec/ServerMessagesRec.h"

// STATIC: 0x00C22638/0x140A17528
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22640/0x140A17530
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C22648/0x140A17538
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C22650/0x140A17540
const DBCMeta ServerMessagesRec::s_meta = { "DBFilesClient\\ServerMessages.dbc", 2, 8, ServerMessagesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ServerMessagesRec::FixupStrings(void* record, const char* strings) {
    ServerMessagesRec* rec = static_cast<ServerMessagesRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
