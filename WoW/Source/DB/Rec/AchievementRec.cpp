#include "WoW/Source/DB/Rec/AchievementRec.h"

// STATIC: 0x00C26EE4/0x140A1D028
static const int32_t s_fieldOffsets[14] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C26F1C/0x140A1D060
static const int32_t s_fieldSizes[14] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26F54/0x140A1D098
static const int32_t s_fieldTypes[14] = { 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0 };

// STATIC: 0x00C26F8C/0x140A1D0D0
const DBCMeta AchievementRec::s_meta = { "DBFilesClient\\Achievement.dbc", 14, 56, AchievementRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECC0/0x1407D8670
void AchievementRec::FixupStrings(void* record, const char* strings) {
    AchievementRec* rec = static_cast<AchievementRec*>(record);
    DBCFixupString(rec->m_title, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_reward, strings);
}
