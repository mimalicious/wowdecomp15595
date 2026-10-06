#include "WoW/Source/DB/Rec/ObjectEffectPackageRec.h"

// STATIC: %0x00B43714
const DBCMeta ObjectEffectPackageRec::s_meta = { "DBFilesClient\\ObjectEffectPackage.dbc", 2, 8, ObjectEffectPackageRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ObjectEffectPackageRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectPackageRec* rec = static_cast<ObjectEffectPackageRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ObjectEffectPackageRec::ConvertRow(ObjectEffectPackageRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
