#include "WoW/Source/DB/Rec/ObjectEffectPackageElemRec.h"

// STATIC: %0x00B436D8
const DBCMeta ObjectEffectPackageElemRec::s_meta = { "DBFilesClient\\ObjectEffectPackageElem.dbc", 4, 16, ObjectEffectPackageElemRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void ObjectEffectPackageElemRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ObjectEffectPackageElemRec::ConvertRow(ObjectEffectPackageElemRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_objectEffectPackageID, &row[0x0004], sizeof(rec->m_objectEffectPackageID));
    memcpy(&rec->m_objectEffectGroupID, &row[0x0008], sizeof(rec->m_objectEffectGroupID));
    memcpy(&rec->m_stateType, &row[0x000C], sizeof(rec->m_stateType));
}
#endif
