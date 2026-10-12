#include "WoW/Source/DB/Rec/ChatChannelsRec.h"

// STATIC: 0x00C26480/0x140A1C390
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C26494/0x140A1C3A8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C264A8/0x140A1C3C0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 2, 2 };

// STATIC: 0x00C264BC/0x140A1C3D8
const DBCMeta ChatChannelsRec::s_meta = { "DBFilesClient\\ChatChannels.dbc", 5, 20, ChatChannelsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 5158ccd3 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EC20/0x1407D8550
void ChatChannelsRec::FixupStrings(void* record, const char* strings) {
    ChatChannelsRec* rec = static_cast<ChatChannelsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_shortcut, strings);
}
