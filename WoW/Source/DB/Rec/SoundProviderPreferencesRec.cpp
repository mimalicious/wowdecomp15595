#include "WoW/Source/DB/Rec/SoundProviderPreferencesRec.h"

// STATIC: 0x00C1DDE8/0x140A11E60
const DBCMeta SoundProviderPreferencesRec::s_meta = { "DBFilesClient\\SoundProviderPreferences.dbc", 24, 96, SoundProviderPreferencesRec::FixupStrings, 0 };

// PROVEN: 6434dfa6 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SoundProviderPreferencesRec::FixupStrings(void* record, const char* strings) {
    SoundProviderPreferencesRec* rec = static_cast<SoundProviderPreferencesRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
