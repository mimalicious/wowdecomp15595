#include "WoW/Source/DB/Rec/ChrClassesRec.h"

// STATIC: %0x00B43E68
const DBCMeta ChrClassesRec::s_meta = { "DBFilesClient\\ChrClasses.dbc", 14, 56, ChrClassesRec::FixupStrings, 0 };

// OFFSET: %0x0097C0D0
void ChrClassesRec::FixupStrings(void* record, const char* strings) {
    ChrClassesRec* rec = static_cast<ChrClassesRec*>(record);
    rec->m_petNameToken = &strings[reinterpret_cast<uintptr_t>(rec->m_petNameToken)];
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_nameFemale = &strings[reinterpret_cast<uintptr_t>(rec->m_nameFemale)];
    rec->m_nameMale = &strings[reinterpret_cast<uintptr_t>(rec->m_nameMale)];
    rec->m_filename = &strings[reinterpret_cast<uintptr_t>(rec->m_filename)];
}

#if defined(WOWDECOMP_ARCH_64)
void ChrClassesRec::ConvertRow(ChrClassesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    rec->m_petNameToken = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_nameFemale = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    rec->m_nameMale = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_filename = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->m_spellClassSet, &row[0x001C], sizeof(rec->m_spellClassSet));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
}
#endif
