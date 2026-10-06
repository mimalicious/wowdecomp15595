#include "WoW/Source/DB/Rec/SoundEntriesRec.h"

// STATIC: %0x00B40A58
const DBCMeta SoundEntriesRec::s_meta = { "DBFilesClient\\SoundEntries.dbc", 16, 136, SoundEntriesRec::FixupStrings, 0 };

// OFFSET: %0x0097BC40
void SoundEntriesRec::FixupStrings(void* record, const char* strings) {
    SoundEntriesRec* rec = static_cast<SoundEntriesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_file[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[0])];
    rec->m_file[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[1])];
    rec->m_file[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[2])];
    rec->m_file[3] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[3])];
    rec->m_file[4] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[4])];
    rec->m_file[5] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[5])];
    rec->m_file[6] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[6])];
    rec->m_file[7] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[7])];
    rec->m_file[8] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[8])];
    rec->m_file[9] = &strings[reinterpret_cast<uintptr_t>(rec->m_file[9])];
    rec->m_directoryBase = &strings[reinterpret_cast<uintptr_t>(rec->m_directoryBase)];
}

#if defined(WOWDECOMP_ARCH_64)
void SoundEntriesRec::ConvertRow(SoundEntriesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_soundType, &row[0x0004], sizeof(rec->m_soundType));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_file[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_file[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    rec->m_file[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_file[3] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->m_file[4] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    rec->m_file[5] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0020])];
    rec->m_file[6] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
    rec->m_file[7] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0028])];
    rec->m_file[8] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    rec->m_file[9] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0030])];
    memcpy(rec->m_freq, &row[0x0034], sizeof(rec->m_freq));
    rec->m_directoryBase = &strings[*reinterpret_cast<const uint32_t*>(&row[0x005C])];
    memcpy(&rec->m_volumeFloat, &row[0x0060], sizeof(rec->m_volumeFloat));
    memcpy(&rec->m_flags, &row[0x0064], sizeof(rec->m_flags));
    memcpy(&rec->m_minDistance, &row[0x0068], sizeof(rec->m_minDistance));
    memcpy(&rec->m_distanceCutoff, &row[0x006C], sizeof(rec->m_distanceCutoff));
    memcpy(&rec->m_eaxdef, &row[0x0070], sizeof(rec->m_eaxdef));
    memcpy(&rec->m_soundEntriesAdvancedID, &row[0x0074], sizeof(rec->m_soundEntriesAdvancedID));
    memcpy(&rec->m_volumeVariationPlus, &row[0x0078], sizeof(rec->m_volumeVariationPlus));
    memcpy(&rec->m_volumeVariationMinus, &row[0x007C], sizeof(rec->m_volumeVariationMinus));
    memcpy(&rec->m_pitchVariationPlus, &row[0x0080], sizeof(rec->m_pitchVariationPlus));
    memcpy(&rec->m_pitchVariationMinus, &row[0x0084], sizeof(rec->m_pitchVariationMinus));
}
#endif
