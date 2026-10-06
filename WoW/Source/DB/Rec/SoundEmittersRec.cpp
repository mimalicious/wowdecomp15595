#include "WoW/Source/DB/Rec/SoundEmittersRec.h"

// STATIC: %0x00B4545C
const DBCMeta SoundEmittersRec::s_meta = { "DBFilesClient\\SoundEmitters.dbc", 6, 40, SoundEmittersRec::FixupStrings, 0 };

// OFFSET: %0x0097C4B0
void SoundEmittersRec::FixupStrings(void* record, const char* strings) {
    SoundEmittersRec* rec = static_cast<SoundEmittersRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SoundEmittersRec::ConvertRow(SoundEmittersRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_position, &row[0x0004], sizeof(rec->m_position));
    memcpy(rec->m_direction, &row[0x0010], sizeof(rec->m_direction));
    memcpy(&rec->m_soundEntryAdvancedID, &row[0x001C], sizeof(rec->m_soundEntryAdvancedID));
    memcpy(&rec->m_mapID, &row[0x0020], sizeof(rec->m_mapID));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
}
#endif
