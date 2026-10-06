#include "WoW/Source/DB/Rec/ObjectEffectRec.h"

// STATIC: 0x00C1D02C/0x140A10E38
const DBCMeta ObjectEffectRec::s_meta = { "DBFilesClient\\ObjectEffect.dbc", 10, 48, ObjectEffectRec::FixupStrings, 0 };

// PROVEN: 41bc806e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ObjectEffectRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectRec* rec = static_cast<ObjectEffectRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
