#include "WoW/Source/DB/Rec/ObjectEffectGroupRec.h"

// STATIC: 0x00C1CF74/0x140A10D68
const DBCMeta ObjectEffectGroupRec::s_meta = { "DBFilesClient\\ObjectEffectGroup.dbc", 2, 8, ObjectEffectGroupRec::FixupStrings, 0 };

// PROVEN: dbcbde07 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ObjectEffectGroupRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectGroupRec* rec = static_cast<ObjectEffectGroupRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
