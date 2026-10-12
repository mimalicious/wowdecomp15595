#include "WoW/Source/DB/Rec/SoundProviderPreferencesRec.h"

// STATIC: 0x00C1DCC8/0x140A11D40
static const int32_t s_fieldOffsets[24] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92 };

// STATIC: 0x00C1DD28/0x140A11DA0
static const int32_t s_fieldSizes[24] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1DD88/0x140A11E00
static const int32_t s_fieldTypes[24] = { 0, 2, 0, 0, 3, 3, 3, 0, 0, 3, 0, 3, 0, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3 };

// STATIC: 0x00C1DDE8/0x140A11E60
const DBCMeta SoundProviderPreferencesRec::s_meta = { "DBFilesClient\\SoundProviderPreferences.dbc", 24, 96, SoundProviderPreferencesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6434dfa6 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SoundProviderPreferencesRec::FixupStrings(void* record, const char* strings) {
    SoundProviderPreferencesRec* rec = static_cast<SoundProviderPreferencesRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
