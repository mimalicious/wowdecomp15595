#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"

// STATIC: 0x00C1D88C/0x140A11838
const DBCMeta CreatureDisplayInfoExtraRec::s_meta = { "DBFilesClient\\CreatureDisplayInfoExtra.dbc", 11, 84, CreatureDisplayInfoExtraRec::FixupStrings, 0 };

// PROVEN: 408f7792 IDENTICAL/NONE
// OFFSET: 0x00A2E840/0x1407D7950
void CreatureDisplayInfoExtraRec::FixupStrings(void* record, const char* strings) {
    CreatureDisplayInfoExtraRec* rec = static_cast<CreatureDisplayInfoExtraRec*>(record);
    DBCFixupString(rec->m_bakeName, strings);
}
