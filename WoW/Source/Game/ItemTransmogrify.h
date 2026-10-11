#ifndef WOW_SOURCE_GAME_ITEMTRANSMOGRIFY_H
#define WOW_SOURCE_GAME_ITEMTRANSMOGRIFY_H

#include <cstdint>

class SpellRec;

class ItemRecInterface {
    public:
    virtual int32_t GetID() const = 0;
    virtual int32_t GetClassID() const = 0;
    virtual int32_t GetSubclassID() const = 0;
    virtual int32_t GetInventoryType() const = 0;
    virtual int32_t GetItemLevel() const = 0;
    virtual int32_t GetAllowableClass() const = 0;
    virtual int32_t GetFlags(int32_t index) const = 0;
    virtual int32_t GetNumStats() const = 0;
    virtual int32_t GetStatType(int32_t index) const = 0;
    virtual int32_t GetStatValue(int32_t index) const = 0;
    virtual int32_t GetNumSpells() const = 0;
    virtual int32_t GetSpellID(int32_t index) const = 0;
    virtual int32_t GetSpellTrigger(int32_t index) const = 0;
    virtual int32_t GetQuality() const = 0;
    virtual int32_t GetScalingStatDistribution() const = 0;
};

enum TRANSMOGRIFY_RESULT {
    TRANSMOGRIFY_OK = 0,
    TRANSMOGRIFY_NO_ITEM = 1,
    TRANSMOGRIFY_NOT_SOULBOUND = 2,
    TRANSMOGRIFY_LEGENDARY = 3,
    TRANSMOGRIFY_INVALID_TYPE = 4,
    TRANSMOGRIFY_INVALID_TARGET = 5,
    TRANSMOGRIFY_MISMATCH = 6,
    TRANSMOGRIFY_SAME_ITEM = 7,
    TRANSMOGRIFY_INVALID_SOURCE = 8,
    TRANSMOGRIFY_NO_STATS = 9,
};

bool IsRestrictedItemMod(int32_t stat);
bool IsRestrictedCombatRating(int32_t rating);
bool CanClassUseArmorSubclass(uint8_t classID, uint32_t subclass);
bool CanClassUseWeaponSubclass(int32_t classID, int32_t subclass);
bool ClassUsesItemStatCategory(int32_t classID, int32_t category);
bool CanClassDualWield(int32_t classID);
bool IsRelicItem(const ItemRecInterface* item);
bool IsBowGunOrCrossbow(const ItemRecInterface* item);
bool CanItemBeTransmogrified(const ItemRecInterface* item, int32_t* reason);
bool SpellHasRestrictedStat(const SpellRec* spell);
bool EnchantmentHasRestrictedStat(int32_t enchantmentId);
int32_t GetItemStatCategory(const ItemRecInterface* item, int32_t randomPropertiesId);
bool IsAllClassesMask(int32_t mask);
bool IsItemUsefulForClass(int32_t classID, const ItemRecInterface* item, int32_t randomPropertiesId, bool checkLevel, bool ignoreStats);
bool ItemHasStats(const ItemRecInterface* item, int32_t randomPropertiesId);
bool CanTransmogrifyTarget(const ItemRecInterface* item, uint32_t bound, int32_t randomPropertiesId, int32_t* reason);
bool CanTransmogrifySource(const ItemRecInterface* item, int32_t randomPropertiesId, int32_t* reason);
bool CanTransmogrifyItemWithItem(const ItemRecInterface* source, const ItemRecInterface* target, uint32_t bound, int32_t sourceRandomPropertiesId, int32_t targetRandomPropertiesId, int32_t* reason);

#endif
