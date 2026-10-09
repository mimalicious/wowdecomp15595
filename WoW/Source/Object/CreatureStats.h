#ifndef WOW_SOURCE_OBJECT_CREATURESTATS_H
#define WOW_SOURCE_OBJECT_CREATURESTATS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"

class CDataStore;

class CreatureStats {
    public:
    /* 0004 */ char* m_subName;
    /* 0008 */ char* m_iconName;
    /* 000C */ uint32_t m_typeFlags [2];
    /* 0014 */ uint32_t m_type;
    /* 0018 */ uint32_t m_family;
    /* 001C */ uint32_t m_rank;
    /* 0020 */ uint32_t m_killCredit [2];
    /* 0028 */ uint32_t m_displayId [4];
    /* 0038 */ float m_hpModifier;
    /* 003C */ float m_mpModifier;
    /* 0040 */ bool m_racialLeader;
    /* 0044 */ uint32_t m_questItem [6];
    /* 005C */ uint32_t m_movementId;
    /* 0060 */ uint32_t m_expansion;
    /* 0064 */ char* m_name [4];
    /* 0074 */ char* m_nameAlt [4];

    virtual ~CreatureStats();

    const char* GetName(int32_t index) const;
    void Pack(CDataStore* msg) const;
    void Unpack(CDataStore* msg);
};

#include "WoW/Source/Object/CreatureStats.inl"

#endif
