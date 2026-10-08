#ifndef WOW_SOURCE_DB_REC_ITEMREC_H
#define WOW_SOURCE_DB_REC_ITEMREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB2.h"

class ItemRec {
    public:
    /* 0000 */ int32_t m_ID;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subclassID;
    /* 000C */ int32_t m_soundOverrideSubclassID;
    /* 0010 */ int32_t m_material;
    /* 0014 */ int32_t m_displayInfoID;
    /* 0018 */ int32_t m_inventoryType;
    /* 001C */ int32_t m_sheatheType;
};

extern WowClientDB2<ItemRec> g_itemDB;

#endif
