#include "WoW/Source/DB/Rec/AnimationDataRec.h"

// STATIC: %0x00B40B68
const DBCMeta AnimationDataRec::s_meta = { "DBFilesClient\\AnimationData.dbc", 8, 32, AnimationDataRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void AnimationDataRec::FixupStrings(void* record, const char* strings) {
    AnimationDataRec* rec = static_cast<AnimationDataRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void AnimationDataRec::ConvertRow(AnimationDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_weaponflags, &row[0x0008], sizeof(rec->m_weaponflags));
    memcpy(&rec->m_bodyflags, &row[0x000C], sizeof(rec->m_bodyflags));
    memcpy(&rec->m_flags, &row[0x0010], sizeof(rec->m_flags));
    memcpy(&rec->m_fallback, &row[0x0014], sizeof(rec->m_fallback));
    memcpy(&rec->m_behaviorID, &row[0x0018], sizeof(rec->m_behaviorID));
    memcpy(&rec->m_behaviorTier, &row[0x001C], sizeof(rec->m_behaviorTier));
}
#endif
