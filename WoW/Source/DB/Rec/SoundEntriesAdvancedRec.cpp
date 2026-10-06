#include "WoW/Source/DB/Rec/SoundEntriesAdvancedRec.h"

// STATIC: %0x00B40A94
const DBCMeta SoundEntriesAdvancedRec::s_meta = { "DBFilesClient\\SoundEntriesAdvanced.dbc", 24, 96, SoundEntriesAdvancedRec::FixupStrings, 0 };

// OFFSET: %0x0097BC70
void SoundEntriesAdvancedRec::FixupStrings(void* record, const char* strings) {
    SoundEntriesAdvancedRec* rec = static_cast<SoundEntriesAdvancedRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SoundEntriesAdvancedRec::ConvertRow(SoundEntriesAdvancedRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_soundEntryID, &row[0x0004], sizeof(rec->m_soundEntryID));
    memcpy(&rec->m_innerRadius2d, &row[0x0008], sizeof(rec->m_innerRadius2d));
    memcpy(&rec->m_timeA, &row[0x000C], sizeof(rec->m_timeA));
    memcpy(&rec->m_timeB, &row[0x0010], sizeof(rec->m_timeB));
    memcpy(&rec->m_timeC, &row[0x0014], sizeof(rec->m_timeC));
    memcpy(&rec->m_timeD, &row[0x0018], sizeof(rec->m_timeD));
    memcpy(&rec->m_randomOffsetRange, &row[0x001C], sizeof(rec->m_randomOffsetRange));
    memcpy(&rec->m_usage, &row[0x0020], sizeof(rec->m_usage));
    memcpy(&rec->m_timeIntervalMin, &row[0x0024], sizeof(rec->m_timeIntervalMin));
    memcpy(&rec->m_timeIntervalMax, &row[0x0028], sizeof(rec->m_timeIntervalMax));
    memcpy(&rec->m_volumeSliderCategory, &row[0x002C], sizeof(rec->m_volumeSliderCategory));
    memcpy(&rec->m_duckToSfx, &row[0x0030], sizeof(rec->m_duckToSfx));
    memcpy(&rec->m_duckToMusic, &row[0x0034], sizeof(rec->m_duckToMusic));
    memcpy(&rec->m_duckToAmbience, &row[0x0038], sizeof(rec->m_duckToAmbience));
    memcpy(&rec->m_innerRadiusOfInfluence, &row[0x003C], sizeof(rec->m_innerRadiusOfInfluence));
    memcpy(&rec->m_outerRadiusOfInfluence, &row[0x0040], sizeof(rec->m_outerRadiusOfInfluence));
    memcpy(&rec->m_timeToDuck, &row[0x0044], sizeof(rec->m_timeToDuck));
    memcpy(&rec->m_timeToUnduck, &row[0x0048], sizeof(rec->m_timeToUnduck));
    memcpy(&rec->m_insideAngle, &row[0x004C], sizeof(rec->m_insideAngle));
    memcpy(&rec->m_outsideAngle, &row[0x0050], sizeof(rec->m_outsideAngle));
    memcpy(&rec->m_outsideVolume, &row[0x0054], sizeof(rec->m_outsideVolume));
    memcpy(&rec->m_outerRadius2d, &row[0x0058], sizeof(rec->m_outerRadius2d));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x005C])];
}
#endif
