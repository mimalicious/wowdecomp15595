#include "WoW/Source/DB/Rec/ChatProfanityRec.h"

// STATIC: 0x00C1E6BC/0x140A12938
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1E6C8/0x140A12948
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1E6D4/0x140A12958
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C1E6E0/0x140A12968
const DBCMeta ChatProfanityRec::s_meta = { "DBFilesClient\\ChatProfanity.dbc", 3, 12, ChatProfanityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ChatProfanityRec::FixupStrings(void* record, const char* strings) {
    ChatProfanityRec* rec = static_cast<ChatProfanityRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
