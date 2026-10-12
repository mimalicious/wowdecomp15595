#include "WoW/Source/DB/Rec/UnitPowerBarRec.h"

// STATIC: 0x00C1FC60/0x140A142D0
static const int32_t s_fieldOffsets[17] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 56, 80, 84, 88, 92, 96, 100, 104 };

// STATIC: 0x00C1FCA8/0x140A14320
static const int32_t s_fieldSizes[17] = { 4, 4, 4, 4, 4, 4, 4, 4, 24, 24, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1FCF0/0x140A14370
static const int32_t s_fieldTypes[17] = { 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3 };

// STATIC: 0x00C1FD34/0x140A143B8
const DBCMeta UnitPowerBarRec::s_meta = { "DBFilesClient\\UnitPowerBar.dbc", 17, 108, UnitPowerBarRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E9E0/0x1407D7F10
void UnitPowerBarRec::FixupStrings(void* record, const char* strings) {
    UnitPowerBarRec* rec = static_cast<UnitPowerBarRec*>(record);
    DBCFixupString(rec->unk_0054, strings);
    DBCFixupString(rec->m_costString, strings);
    DBCFixupString(rec->m_outOfError, strings);
    DBCFixupString(rec->unk_0060, strings);
}
