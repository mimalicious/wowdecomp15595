#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"

// STATIC: 0x00C1D6F4/0x140A11650
const DBCMeta CreatureModelDataRec::s_meta = { "DBFilesClient\\CreatureModelData.dbc", 27, 124, CreatureModelDataRec::FixupStrings, 0 };

// PROVEN: 7d549e20 IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void CreatureModelDataRec::FixupStrings(void* record, const char* strings) {
    CreatureModelDataRec* rec = static_cast<CreatureModelDataRec*>(record);
    DBCFixupString(rec->m_modelName, strings);
}
