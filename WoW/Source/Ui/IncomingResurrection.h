#ifndef WOW_SOURCE_UI_INCOMINGRESURRECTION_H
#define WOW_SOURCE_UI_INCOMINGRESURRECTION_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/WGUID.h"

struct INCOMINGRESURRECTION {
    /* 0000 */ WOWGUID m_caster;
    /* 0008 */ int32_t m_spellID;
    /* 000C */ uint32_t unk_000C;
};

class CGIncomingResurrection {
    public:
    struct INCOMINGRESLIST : public TSHashObject<INCOMINGRESLIST, CHashKeyGUID> {
        /* 0020 */ TSGrowableArray<INCOMINGRESURRECTION> m_resurrections;
    };

    static TSHashTable<INCOMINGRESLIST, CHashKeyGUID> s_incoming;

    static bool HasIncomingResurrection(const WOWGUID& guid);
    static void Reset();
    static void Remove(const WOWGUID& target, const WOWGUID& caster, int32_t spellID);
    static void Add(const WOWGUID& target, const WOWGUID& caster, int32_t spellID);
};


#endif
