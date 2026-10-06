#include "WoW/Source/DB/Rec/CharSectionsRec.h"

// STATIC: %0x00B43F48
const DBCMeta CharSectionsRec::s_meta = { "DBFilesClient\\CharSections.dbc", 8, 40, CharSectionsRec::FixupStrings, 0 };

// OFFSET: %0x0097C0F0
void CharSectionsRec::FixupStrings(void* record, const char* strings) {
    CharSectionsRec* rec = static_cast<CharSectionsRec*>(record);
    rec->m_textureName[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureName[0])];
    rec->m_textureName[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureName[1])];
    rec->m_textureName[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureName[2])];
}

#if defined(WOWDECOMP_ARCH_64)
void CharSectionsRec::ConvertRow(CharSectionsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_raceID, &row[0x0004], sizeof(rec->m_raceID));
    memcpy(&rec->m_sexID, &row[0x0008], sizeof(rec->m_sexID));
    memcpy(&rec->m_baseSection, &row[0x000C], sizeof(rec->m_baseSection));
    rec->m_textureName[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    rec->m_textureName[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_textureName[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->m_flags, &row[0x001C], sizeof(rec->m_flags));
    memcpy(&rec->m_variationIndex, &row[0x0020], sizeof(rec->m_variationIndex));
    memcpy(&rec->m_colorIndex, &row[0x0024], sizeof(rec->m_colorIndex));
}
#endif
