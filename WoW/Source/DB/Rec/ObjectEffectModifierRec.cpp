#include "WoW/Source/DB/Rec/ObjectEffectModifierRec.h"

// STATIC: %0x00B43750
const DBCMeta ObjectEffectModifierRec::s_meta = { "DBFilesClient\\ObjectEffectModifier.dbc", 5, 32, ObjectEffectModifierRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void ObjectEffectModifierRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ObjectEffectModifierRec::ConvertRow(ObjectEffectModifierRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_inputType, &row[0x0004], sizeof(rec->m_inputType));
    memcpy(&rec->m_mapType, &row[0x0008], sizeof(rec->m_mapType));
    memcpy(&rec->m_outputType, &row[0x000C], sizeof(rec->m_outputType));
    memcpy(rec->m_param, &row[0x0010], sizeof(rec->m_param));
}
#endif
