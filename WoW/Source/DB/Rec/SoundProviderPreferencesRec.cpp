#include "WoW/Source/DB/Rec/SoundProviderPreferencesRec.h"

// STATIC: %0x00B434E8
const DBCMeta SoundProviderPreferencesRec::s_meta = { "DBFilesClient\\SoundProviderPreferences.dbc", 24, 96, SoundProviderPreferencesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SoundProviderPreferencesRec::FixupStrings(void* record, const char* strings) {
    SoundProviderPreferencesRec* rec = static_cast<SoundProviderPreferencesRec*>(record);
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void SoundProviderPreferencesRec::ConvertRow(SoundProviderPreferencesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
    memcpy(&rec->m_eaxenvironmentSelection, &row[0x000C], sizeof(rec->m_eaxenvironmentSelection));
    memcpy(&rec->m_eaxdecayTime, &row[0x0010], sizeof(rec->m_eaxdecayTime));
    memcpy(&rec->m_eax2EnvironmentSize, &row[0x0014], sizeof(rec->m_eax2EnvironmentSize));
    memcpy(&rec->m_eax2EnvironmentDiffusion, &row[0x0018], sizeof(rec->m_eax2EnvironmentDiffusion));
    memcpy(&rec->m_eax2Room, &row[0x001C], sizeof(rec->m_eax2Room));
    memcpy(&rec->m_eax2RoomHf, &row[0x0020], sizeof(rec->m_eax2RoomHf));
    memcpy(&rec->m_eax2DecayHfratio, &row[0x0024], sizeof(rec->m_eax2DecayHfratio));
    memcpy(&rec->m_eax2Reflections, &row[0x0028], sizeof(rec->m_eax2Reflections));
    memcpy(&rec->m_eax2ReflectionsDelay, &row[0x002C], sizeof(rec->m_eax2ReflectionsDelay));
    memcpy(&rec->m_eax2Reverb, &row[0x0030], sizeof(rec->m_eax2Reverb));
    memcpy(&rec->m_eax2ReverbDelay, &row[0x0034], sizeof(rec->m_eax2ReverbDelay));
    memcpy(&rec->m_eax2RoomRolloff, &row[0x0038], sizeof(rec->m_eax2RoomRolloff));
    memcpy(&rec->m_eax2AirAbsorption, &row[0x003C], sizeof(rec->m_eax2AirAbsorption));
    memcpy(&rec->m_eax3RoomLf, &row[0x0040], sizeof(rec->m_eax3RoomLf));
    memcpy(&rec->m_eax3DecayLfratio, &row[0x0044], sizeof(rec->m_eax3DecayLfratio));
    memcpy(&rec->m_eax3EchoTime, &row[0x0048], sizeof(rec->m_eax3EchoTime));
    memcpy(&rec->m_eax3EchoDepth, &row[0x004C], sizeof(rec->m_eax3EchoDepth));
    memcpy(&rec->m_eax3ModulationTime, &row[0x0050], sizeof(rec->m_eax3ModulationTime));
    memcpy(&rec->m_eax3ModulationDepth, &row[0x0054], sizeof(rec->m_eax3ModulationDepth));
    memcpy(&rec->m_eax3Hfreference, &row[0x0058], sizeof(rec->m_eax3Hfreference));
    memcpy(&rec->m_eax3Lfreference, &row[0x005C], sizeof(rec->m_eax3Lfreference));
}
#endif
