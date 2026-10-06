#include "WoW/Source/DB/Rec/WorldStateUIRec.h"

// STATIC: %0x00B44434
const DBCMeta WorldStateUIRec::s_meta = { "DBFilesClient\\WorldStateUI.dbc", 15, 68, WorldStateUIRec::FixupStrings, 0 };

// OFFSET: %0x0097C120
void WorldStateUIRec::FixupStrings(void* record, const char* strings) {
    WorldStateUIRec* rec = static_cast<WorldStateUIRec*>(record);
    rec->m_icon = &strings[reinterpret_cast<uintptr_t>(rec->m_icon)];
    rec->m_string = &strings[reinterpret_cast<uintptr_t>(rec->m_string)];
    rec->m_tooltip = &strings[reinterpret_cast<uintptr_t>(rec->m_tooltip)];
    rec->m_dynamicIcon = &strings[reinterpret_cast<uintptr_t>(rec->m_dynamicIcon)];
    rec->m_dynamicTooltip = &strings[reinterpret_cast<uintptr_t>(rec->m_dynamicTooltip)];
    rec->m_extendedUi = &strings[reinterpret_cast<uintptr_t>(rec->m_extendedUi)];
}

#if defined(WOWDECOMP_ARCH_64)
void WorldStateUIRec::ConvertRow(WorldStateUIRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_areaID, &row[0x0008], sizeof(rec->m_areaID));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    rec->m_icon = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->m_string = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    rec->m_tooltip = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0020])];
    memcpy(&rec->m_stateVariable, &row[0x0024], sizeof(rec->m_stateVariable));
    memcpy(&rec->m_type, &row[0x0028], sizeof(rec->m_type));
    rec->m_dynamicIcon = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    rec->m_dynamicTooltip = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0030])];
    rec->m_extendedUi = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0034])];
    memcpy(rec->m_extendedUistateVariable, &row[0x0038], sizeof(rec->m_extendedUistateVariable));
}
#endif
