#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_ITEM_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_ITEM_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

class CClientObjCreate;
class CImVector;
class ItemRecSparse;
class ItemReforgeRec;
class ItemGroupSoundsRec;
class LockRec;

struct INVENTORYART : public TSHashObject<INVENTORYART, HASHKEY_NONE> {
    /* 0018 */ char* m_texture;

    void SetTexture(const char* texture);
};

struct ITEMBYNAME : public TSHashObject<ITEMBYNAME, HASHKEY_UTF8I> {
    /* 0018 */ int32_t m_itemId;
};

extern const uint32_t s_inventoryTypeSlots[29];

int32_t GetItemIDByName(const char* name);
void EvalutateReforgeOfRandomProp(int32_t randomPropertiesId, int32_t propertySeed, const ItemReforgeRec* reforge, int32_t& hasSourceStat, int32_t& invalid, int32_t& reason);
int32_t CalculateItemBonusFromRandomEnchant(int32_t stat, int32_t randomPropertiesId, int32_t propertySeed);

class CGItem_C : public CGObject_C, public CGItem {
    public:
    static void Initialize();
    static void Shutdown();
    struct ItemPurchaseData {
        /* 0000 */ uint32_t m_money;
        /* 0004 */ uint32_t m_honorPoints;
        /* 0008 */ uint32_t m_arenaPoints;
        /* 000C */ uint32_t m_itemId [5];
        /* 0020 */ uint32_t m_itemCount [5];
        /* 0034 */ uint32_t unk_0034;
        /* 0038 */ uint32_t m_refundTimeLeft;
    };

    static char* BuildItemName(char* buffer, uint32_t size, int32_t itemId, int32_t randomPropertiesId);
    static void GetCooldowns(int32_t itemId, uint32_t& cooldown, uint32_t& categoryCooldown, uint32_t& category);
    static int32_t IsMetal(int32_t materialId);
    static int32_t NumBaseCharges(int32_t itemId);
    static void RemoveNameFromCache(const char* name);
    static const char* GetInventoryArt(int32_t displayId);
    const char* GetInventoryArt() const;

    /* 0104 */ TSList<void> m_itemMirrorLists [66];
    /* 041C */ uint32_t m_itemFlags;
    /* 0420 */ CGUnitVirtualItem m_virtualItem;
    /* 0428 */ uint32_t m_expirationTime;
    /* 042C */ uint32_t m_enchantmentExpiration [15];
    /* 0468 */ ItemPurchaseData* m_itemPurchaseData;
    /* 046C */ const ItemGroupSoundsRec* m_groupSounds;

    ~CGItem_C();
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) override;
    void PostReenable(const CClientObjCreate& create) override;
    int32_t GetSelectionHighlightColor(CImVector* color) const override;
    const char* GetObjectName() const override;
    int32_t GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const override;
    uint32_t GetItemStaticFlags(int32_t index) override;
    void Disable() override;

    CGItem_C(uint32_t time, const CClientObjCreate& create);
    uint32_t GetBagFamily() const;
    static int32_t CanGoInSlot(int32_t itemId, uint32_t slot, int32_t bypass);
    int32_t CanGoInSlot(uint32_t slot, int32_t bypass);
    uint32_t GetClassID() const;
    int32_t GetCurrentSpellCharges();
    uint32_t GetDisplayID() const;
    uint32_t GetEnchantmentTimeLeft(int32_t slot) const;
    int32_t UseItem(WOWGUID* target, int32_t a2);
    static WOWGUID s_bindOnUseTarget;
    static WOWGUID s_bindOnUseItem;
    static void ConfirmBindOnUse();
    static void ConfirmOnUse();
    int32_t CanBeUsed();
    bool CanBeUsedInArenas();
    bool CanBeUsedInRatedBattleground();
    bool CanBeStoredInVoidStorage(int32_t& error);
    uint32_t GetExpirationTimeLeft() const;
    char* GetFullItemName(char* buffer, uint32_t size) const;
    uint32_t GetInventoryType() const;
    int32_t GetNumSockets();
    int32_t GetOnUseSlot() const;
    int32_t GetPrismaticSocketCount();
    int32_t GetRandomItemSlot() const;
    int32_t GetRandomPropertyPoints() const;
    uint32_t GetSheatheType() const;
    uint32_t GetSpellItemEnchantmentId() const;
    ItemRecSparse* GetStats() const;
    uint32_t GetSubtypeID() const;
    int32_t GetTransmogrifyCost();
    int32_t GetUsableSpellCharges() const;
    bool HasEnchantments();
    int32_t HasOnUseEnchant() const;
    bool HasSoulboundEnchant();
    int32_t HasUseEffect();
    bool IsBound();
    void Unlock();
    void Lock();
    static void SignalEvent(CGItem_C* item, uint32_t event);
    static int32_t GetUseSpell(int32_t itemId);
    static int32_t GetRandomPropertyOverrideAmountEx(int32_t slot, int32_t randomPropertiesId, int32_t points);
    static int32_t IsLocked(const LockRec* lock, int32_t itemLevel, int32_t* spellId, int32_t* skillValue, int32_t* requiredSkill, int32_t* lockIndex, CGItem_C** key, int32_t* entry);
    static bool ReforgeIsValid(const ItemRecSparse* stats, const ItemReforgeRec* reforge, int32_t randomPropertiesId, int32_t propertySeed, int32_t& reason);
    static void ModStatByReforge(int32_t level, const ItemRecSparse* stats, const ItemReforgeRec* reforge, int32_t stat, int32_t& value, int32_t randomPropertiesId, int32_t propertySeed);
    bool ReforgeIsValid(const ItemReforgeRec* reforge, int32_t& reason);
    int32_t GetRandomPropertyOverrideAmount(int32_t slot);
    int32_t GetRepairCost();
    int32_t GetEnchantmentSpell(int32_t a1);
    void GetSpellCooldownInfo(uint32_t& cooldown, uint32_t& categoryCooldown, uint32_t& category);
    bool IsBoundForTrade();
    bool IsBoundToAccount();
    bool IsBoundToBattlenetAccount();
    bool IsInstanceBound() const;
    bool IsPermanentlyBoundForTrade();
    bool IsZoneBound() const;
    void PostInitWithStats();
    void RequestItemCacheInfo();
    void RequestItemPurchaseData();
    void SetItemPurchaseData(ItemPurchaseData* data);
    void SetStorage(uint32_t* storage, uint32_t* saved);
    void SetTranslated();
    void UpdateEnchantments(uint16_t changed);
    int32_t GetItemEnchantmentVisualID() const;
    void UpdateEnchantmentTime(int32_t slot, int32_t seconds);
    void UpdateExpirationTime(int32_t seconds);
    void UpdateSockets(uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
};

#endif
