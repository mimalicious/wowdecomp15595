#include "WoW/Source/DB/Rec/WorldStateUIRec.h"

// STATIC: 0x00C1F004/0x140A134A0
static const int32_t s_fieldOffsets[15] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56 };

// STATIC: 0x00C1F040/0x140A134E0
static const int32_t s_fieldSizes[15] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12 };

// STATIC: 0x00C1F07C/0x140A13520
static const int32_t s_fieldTypes[15] = { 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 2, 0 };

// STATIC: 0x00C1F0B8/0x140A13560
const DBCMeta WorldStateUIRec::s_meta = { "DBFilesClient\\WorldStateUI.dbc", 15, 68, WorldStateUIRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6486cbe9 IDENTICAL/DIFFERENT
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
