#include "WoW/Source/DB/Rec/AchievementRec.h"

// STATIC: 0x00C26F8C/0x140A1D0D0
const DBCMeta AchievementRec::s_meta = { "DBFilesClient\\Achievement.dbc", 14, 56, AchievementRec::FixupStrings, 0 };

// PROVEN: 9078d20c IDENTICAL/NONE
// OFFSET: 0x00A2ECC0/0x1407D8670
void AchievementRec::FixupStrings(void* record, const char* strings) {
    AchievementRec* rec = static_cast<AchievementRec*>(record);
    DBCFixupString(rec->m_title, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_reward, strings);
}
