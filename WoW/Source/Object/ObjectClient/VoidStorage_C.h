#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDSTORAGE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDSTORAGE_C_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

class VoidItem_C : public TSHashObject<VoidItem_C, HASHKEY_NONE> {
    public:
    /* 0018 */ WOWGUID m_guid;
    /* 0020 */ int32_t m_entryID;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t m_propertySeed;
    /* 002C */ int32_t m_randomPropertiesId;
    /* 0030 */ WOWGUID m_creator;
};

class CGVoidStorage_C {
    public:
    /* 0000 */ uint32_t m_state;
    /* 0004 */ TSHashTable<VoidItem_C, HASHKEY_NONE>* m_items;
    /* 0008 */ TSGrowableArray<WOWGUID> unk_0008;

    CGVoidStorage_C();
    ~CGVoidStorage_C();
    VoidItem_C* GetItemBySlot(uint32_t slot);
    VoidItem_C* GetItem(WOWGUID guid);
};

void VoidStorage_C_Initialize();

#endif
