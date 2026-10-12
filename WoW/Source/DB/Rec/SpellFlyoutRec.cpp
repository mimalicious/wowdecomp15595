#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"

// STATIC: 0x00C21400/0x140A15F68
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C2141C/0x140A15F88
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C21438/0x140A15FA8
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 2, 2 };

// STATIC: 0x00C21454/0x140A15FC8
const DBCMeta SpellFlyoutRec::s_meta = { "DBFilesClient\\SpellFlyout.dbc", 7, 28, SpellFlyoutRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EB30/0x1407D7E90
void SpellFlyoutRec::FixupStrings(void* record, const char* strings) {
    SpellFlyoutRec* rec = static_cast<SpellFlyoutRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
