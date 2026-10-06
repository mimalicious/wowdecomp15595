#include "WoW/Source/DB/Rec/SpellVisualKitAreaModelRec.h"

// STATIC: 0x00C206D8/0x140A14F80
const DBCMeta SpellVisualKitAreaModelRec::s_meta = { "DBFilesClient\\SpellVisualKitAreaModel.dbc", 7, 28, SpellVisualKitAreaModelRec::FixupStrings, 0 };

// PROVEN: ae7a67d9 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellVisualKitAreaModelRec::FixupStrings(void* record, const char* strings) {
    SpellVisualKitAreaModelRec* rec = static_cast<SpellVisualKitAreaModelRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
