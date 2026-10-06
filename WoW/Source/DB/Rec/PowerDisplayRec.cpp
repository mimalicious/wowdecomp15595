#include "WoW/Source/DB/Rec/PowerDisplayRec.h"

// STATIC: 0x00C22F10/0x140A17FF0
const DBCMeta PowerDisplayRec::s_meta = { "DBFilesClient\\PowerDisplay.dbc", 6, 16, PowerDisplayRec::FixupStrings, 0 };

// PROVEN: 8b953b0e IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void PowerDisplayRec::FixupStrings(void* record, const char* strings) {
    PowerDisplayRec* rec = static_cast<PowerDisplayRec*>(record);
    DBCFixupString(rec->m_globalStringBaseTag, strings);
}
