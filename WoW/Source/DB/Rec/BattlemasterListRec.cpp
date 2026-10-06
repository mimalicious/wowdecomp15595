#include "WoW/Source/DB/Rec/BattlemasterListRec.h"

// STATIC: %0x00B470C4
const DBCMeta BattlemasterListRec::s_meta = { "DBFilesClient\\BattlemasterList.dbc", 13, 80, BattlemasterListRec::FixupStrings, 0 };

// OFFSET: %0x0097C110
void BattlemasterListRec::FixupStrings(void* record, const char* strings) {
    BattlemasterListRec* rec = static_cast<BattlemasterListRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void BattlemasterListRec::ConvertRow(BattlemasterListRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_instanceType, &row[0x0024], sizeof(rec->m_instanceType));
    memcpy(&rec->m_groupsAllowed, &row[0x0028], sizeof(rec->m_groupsAllowed));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    memcpy(&rec->m_maxGroupSize, &row[0x0030], sizeof(rec->m_maxGroupSize));
    memcpy(&rec->m_holidayWorldState, &row[0x0034], sizeof(rec->m_holidayWorldState));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->unk_003C, &row[0x003C], sizeof(rec->unk_003C));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
    memcpy(&rec->unk_0044, &row[0x0044], sizeof(rec->unk_0044));
    memcpy(&rec->unk_0048, &row[0x0048], sizeof(rec->unk_0048));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
}
#endif
