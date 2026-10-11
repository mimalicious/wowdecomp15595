#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDSTORAGE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDSTORAGE_C_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct JamVoidItem;

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
    /* 0008 */ TSGrowableArray<uint64_t> m_newItems;

    CGVoidStorage_C();
    ~CGVoidStorage_C();
    VoidItem_C* GetItemBySlot(uint32_t slot);
    VoidItem_C* GetItem(WOWGUID guid);
    void Query();
    void OnTransferResult(uint32_t result);
    bool IsNewItem(WOWGUID guid);
    bool SwapItem(WOWGUID guid, uint32_t slot);
    void ClearNewItems();
    bool RemoveItem(WOWGUID guid);
    bool AddItem(const JamVoidItem& item);
    void OnSwapResponse(WOWGUID firstItem, uint32_t firstSlot, WOWGUID secondItem, uint32_t secondSlot);
    void SetContents(const TSGrowableArray<JamVoidItem>& items);
    void OnTransferChanges(const TSGrowableArray<uint64_t>& removed, const TSGrowableArray<JamVoidItem>& added);
};

void VoidStorage_C_Initialize();
void VoidStorage_C_Shutdown();

#endif
