#include "WoW/Source/DB/Rec/ScreenLocationRec.h"

// STATIC: 0x00C22694/0x140A175A0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C2269C/0x140A175A8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C226A4/0x140A175B0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C226AC/0x140A175B8
const DBCMeta ScreenLocationRec::s_meta = { "DBFilesClient\\ScreenLocation.dbc", 2, 8, ScreenLocationRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 2dbf9922 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ScreenLocationRec::FixupStrings(void* record, const char* strings) {
    ScreenLocationRec* rec = static_cast<ScreenLocationRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
