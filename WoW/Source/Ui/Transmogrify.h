#ifndef WOW_SOURCE_UI_TRANSMOGRIFY_H
#define WOW_SOURCE_UI_TRANSMOGRIFY_H

#include <cstdint>
#include <list>
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "WoW/Source/Game/ItemTransmogrify.h"
#include "WoW/Source/Object/WGUID.h"

class CGItem_C;
class ItemRec;
class ItemRecSparse;

extern const uint32_t s_transmogrifySlots[12];

struct TransmogrifyPendingItem {
    /* 0000 */ WOWGUID m_item;
    /* 0008 */ uint32_t m_slot;
};

class ClientItemRec : public ItemRecInterface {
    public:
    /* 0004 */ const ItemRecSparse* m_sparse;
    /* 0008 */ const ItemRec* m_item;

    ClientItemRec(const ItemRecSparse* sparse);
    int32_t GetID() const override;
    int32_t GetClassID() const override;
    int32_t GetSubclassID() const override;
    int32_t GetInventoryType() const override;
    int32_t GetItemLevel() const override;
    int32_t GetAllowableClass() const override;
    int32_t GetFlags(int32_t index) const override;
    int32_t GetNumStats() const override;
    int32_t GetStatType(int32_t index) const override;
    int32_t GetStatValue(int32_t index) const override;
    int32_t GetNumSpells() const override;
    int32_t GetSpellID(int32_t index) const override;
    int32_t GetSpellTrigger(int32_t index) const override;
    int32_t GetQuality() const override;
    int32_t GetScalingStatDistribution() const override;
};

class CGTransmogrify {
    public:
    static WOWGUID m_transmogrifier;
    static std::list<TransmogrifyPendingItem, Blizzard::Memory::AllocSTL_raw<TransmogrifyPendingItem> > s_pendingItems;

    static void InitializeGame();
    static void ShutdownGame();
    static void SetTransmogrifier(const WOWGUID& guid);
    static void OnTransmogrifySuccess(uint32_t slot);
    static TransmogrifyPendingItem* GetPendingItem(uint32_t slot);
    static void AutoAddItem(CGItem_C* item);
    static void NotifySlotChanged(int32_t slot, WOWGUID item, bool notify);
    static void RemovePendingItem(int32_t slot, bool notify);
    static void SetPendingItem(int32_t slot, WOWGUID item, bool notify);
    static void ValidatePendingItems();
    static void UseItemForSlot(CGItem_C* item, uint32_t slot);
};

void TransmogrifyRegisterScriptFunctions();
void TransmogrifyUnregisterScriptFunctions();

#endif
