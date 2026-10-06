#include "WoW/Source/DB/Rec/StationeryRec.h"

// STATIC: %0x00B44B24
const DBCMeta StationeryRec::s_meta = { "DBFilesClient\\Stationery.dbc", 4, 16, StationeryRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void StationeryRec::FixupStrings(void* record, const char* strings) {
    StationeryRec* rec = static_cast<StationeryRec*>(record);
    rec->m_texture = &strings[reinterpret_cast<uintptr_t>(rec->m_texture)];
}

#if defined(WOWDECOMP_ARCH_64)
void StationeryRec::ConvertRow(StationeryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_itemID, &row[0x0004], sizeof(rec->m_itemID));
    rec->m_texture = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
}
#endif
