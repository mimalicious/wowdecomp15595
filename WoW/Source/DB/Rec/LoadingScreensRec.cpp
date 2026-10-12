#include "WoW/Source/DB/Rec/LoadingScreensRec.h"

// STATIC: 0x00C1D0DC/0x140A10F20
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1D0EC/0x140A10F30
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1D0FC/0x140A10F40
static const int32_t s_fieldTypes[4] = { 0, 2, 2, 0 };

// STATIC: 0x00C1D10C/0x140A10F50
const DBCMeta LoadingScreensRec::s_meta = { "DBFilesClient\\LoadingScreens.dbc", 4, 16, LoadingScreensRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a0ead568 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E9B0/0x1407D8110
void LoadingScreensRec::FixupStrings(void* record, const char* strings) {
    LoadingScreensRec* rec = static_cast<LoadingScreensRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_fileName, strings);
}
