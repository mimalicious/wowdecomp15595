#include "WoW/Source/DB/Rec/Startup_StringsRec.h"

// STATIC: 0x00B8CD70/0x14096B740
const DBCMeta Startup_StringsRec::s_meta = { "DBFilesClient\\Startup_Strings.dbc", 3, 12, Startup_StringsRec::FixupStrings, 0 };

// PROVEN: 710439ad IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void Startup_StringsRec::FixupStrings(void* record, const char* strings) {
    Startup_StringsRec* rec = static_cast<Startup_StringsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_message, strings);
}
