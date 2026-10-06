#include "WoW/Source/DB/Rec/ChatChannelsRec.h"

// STATIC: 0x00C264BC/0x140A1C3D8
const DBCMeta ChatChannelsRec::s_meta = { "DBFilesClient\\ChatChannels.dbc", 5, 20, ChatChannelsRec::FixupStrings, 0 };

// PROVEN: 5158ccd3 IDENTICAL/NONE
// OFFSET: 0x00A2EC20/0x1407D8550
void ChatChannelsRec::FixupStrings(void* record, const char* strings) {
    ChatChannelsRec* rec = static_cast<ChatChannelsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_shortcut, strings);
}
