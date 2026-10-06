#include "WoW/Source/DB/Rec/LoadingScreensRec.h"

// STATIC: 0x00C1D10C/0x140A10F50
const DBCMeta LoadingScreensRec::s_meta = { "DBFilesClient\\LoadingScreens.dbc", 4, 16, LoadingScreensRec::FixupStrings, 0 };

// PROVEN: a0ead568 IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void LoadingScreensRec::FixupStrings(void* record, const char* strings) {
    LoadingScreensRec* rec = static_cast<LoadingScreensRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_fileName, strings);
}
