#include "WoW/Source/DB/Rec/WorldStateUIRec.h"

// STATIC: 0x00C1F0B8/0x140A13560
const DBCMeta WorldStateUIRec::s_meta = { "DBFilesClient\\WorldStateUI.dbc", 15, 68, WorldStateUIRec::FixupStrings, 0 };

// PROVEN: 6486cbe9 IDENTICAL/NONE
// OFFSET: 0x00A2E900/0x1407D7C00
void WorldStateUIRec::FixupStrings(void* record, const char* strings) {
    WorldStateUIRec* rec = static_cast<WorldStateUIRec*>(record);
    DBCFixupString(rec->m_icon, strings);
    DBCFixupString(rec->m_string, strings);
    DBCFixupString(rec->m_tooltip, strings);
    DBCFixupString(rec->m_dynamicIcon, strings);
    DBCFixupString(rec->m_dynamicTooltip, strings);
    DBCFixupString(rec->m_extendedUi, strings);
}
