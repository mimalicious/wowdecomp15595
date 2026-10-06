#include "WoW/Source/DB/Rec/GameTipsRec.h"

// STATIC: 0x00C1D534/0x140A11468
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1D544/0x140A11478
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1D554/0x140A11488
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C1D564/0x140A11498
const DBCMeta GameTipsRec::s_meta = { "DBFilesClient\\GameTips.dbc", 4, 16, GameTipsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 621e5ad2 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GameTipsRec::FixupStrings(void* record, const char* strings) {
    GameTipsRec* rec = static_cast<GameTipsRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
