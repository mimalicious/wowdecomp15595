#include "WoW/Source/DB/Rec/PlayerConditionRec.h"

// STATIC: 0x00C22F54/0x140A18050
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22F5C/0x140A18058
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C22F64/0x140A18060
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C22F6C/0x140A18068
const DBCMeta PlayerConditionRec::s_meta = { "DBFilesClient\\PlayerCondition.dbc", 2, 8, PlayerConditionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 877033b7 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void PlayerConditionRec::FixupStrings(void* record, const char* strings) {
    PlayerConditionRec* rec = static_cast<PlayerConditionRec*>(record);
    DBCFixupString(rec->m_failureDescription, strings);
}
