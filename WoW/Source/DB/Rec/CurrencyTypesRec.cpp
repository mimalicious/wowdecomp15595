#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"

// STATIC: %0x00B46E30
const DBCMeta CurrencyTypesRec::s_meta = { "DBFilesClient\\CurrencyTypes.dbc", 10, 44, CurrencyTypesRec::FixupStrings, 0 };

// OFFSET: %0x0097C420
void CurrencyTypesRec::FixupStrings(void* record, const char* strings) {
    CurrencyTypesRec* rec = static_cast<CurrencyTypesRec*>(record);
    rec->unk_0008 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0008)];
    rec->unk_000C = &strings[reinterpret_cast<uintptr_t>(rec->unk_000C)];
    rec->unk_0010 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0010)];
    rec->unk_0028 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0028)];
}

#if defined(WOWDECOMP_ARCH_64)
void CurrencyTypesRec::ConvertRow(CurrencyTypesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    rec->unk_0008 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->unk_000C = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->unk_0010 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    rec->unk_0028 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0028])];
}
#endif
