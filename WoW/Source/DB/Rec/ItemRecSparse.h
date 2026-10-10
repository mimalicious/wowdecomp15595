#ifndef WOW_SOURCE_DB_REC_ITEMRECSPARSE_H
#define WOW_SOURCE_DB_REC_ITEMRECSPARSE_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB2.h"

class ItemRecSparse {
    public:
    static const DB2Meta s_meta;

    static void FixupRecord(void* record);
    static void Initialize();

    const char* GetName(int32_t index) const;
    class ItemRec* GetNonSparse() const;
    int32_t GetBuyPrice(bool* computed) const;
    int32_t GetSellPrice() const;

    /* 0000 */ int32_t m_ID;
    /* 0004 */ int32_t m_quality;
    /* 0008 */ int32_t m_flags[2];
    /* 0010 */ float m_priceRandomValue;
    /* 0014 */ float m_priceVariance;
    /* 0018 */ int32_t m_buyCount;
    /* 001C */ int32_t m_buyPrice;
    /* 0020 */ int32_t m_sellPrice;
    /* 0024 */ int32_t m_inventoryType;
    /* 0028 */ int32_t m_allowableClass;
    /* 002C */ int32_t m_allowableRace;
    /* 0030 */ int32_t m_itemLevel;
    /* 0034 */ int32_t m_requiredLevel;
    /* 0038 */ int32_t m_requiredSkill;
    /* 003C */ int32_t m_requiredSkillRank;
    /* 0040 */ int32_t m_requiredSpell;
    /* 0044 */ int32_t m_requiredHonorRank;
    /* 0048 */ int32_t m_requiredCityRank;
    /* 004C */ int32_t m_requiredReputationFaction;
    /* 0050 */ int32_t m_requiredReputationRank;
    /* 0054 */ int32_t m_maxCount;
    /* 0058 */ int32_t m_stackable;
    /* 005C */ int32_t m_containerSlots;
    /* 0060 */ int32_t m_statModifierBonusStat[10];
    /* 0088 */ int32_t m_statModifierBonusAmount[10];
    /* 00B0 */ int32_t m_statPercentEditor[10];
    /* 00D8 */ int32_t m_statPercentageOfSocket[10];
    /* 0100 */ int32_t m_scalingStatDistribution;
    /* 0104 */ int32_t m_damageType;
    /* 0108 */ int32_t m_delay;
    /* 010C */ float m_rangedModRange;
    /* 0110 */ int32_t m_spellID[5];
    /* 0124 */ int32_t m_spellTrigger[5];
    /* 0138 */ int32_t m_spellCharges[5];
    /* 014C */ int32_t m_spellCooldown[5];
    /* 0160 */ int32_t m_spellCategory[5];
    /* 0174 */ int32_t m_spellCategoryCooldown[5];
    /* 0188 */ int32_t m_bonding;
    /* 018C */ DBCString m_name;
    /* 0190 */ DBCString m_name2;
    /* 0194 */ DBCString m_name3;
    /* 0198 */ DBCString m_name4;
    /* 019C */ DBCString m_description;
    /* 01A0 */ int32_t m_pageText;
    /* 01A4 */ int32_t m_languageID;
    /* 01A8 */ int32_t m_pageMaterial;
    /* 01AC */ int32_t m_startQuest;
    /* 01B0 */ int32_t m_lockID;
    /* 01B4 */ int32_t m_material;
    /* 01B8 */ int32_t m_sheatheType;
    /* 01BC */ int32_t m_randomProperty;
    /* 01C0 */ int32_t m_randomSuffix;
    /* 01C4 */ int32_t m_itemSet;
    /* 01C8 */ int32_t m_zoneBound;
    /* 01CC */ int32_t m_instanceBound;
    /* 01D0 */ int32_t m_bagFamily;
    /* 01D4 */ int32_t m_totemCategoryID;
    /* 01D8 */ int32_t m_socketColor[3];
    /* 01E4 */ int32_t m_socketContent[3];
    /* 01F0 */ int32_t m_socketBonus;
    /* 01F4 */ int32_t m_gemProperties;
    /* 01F8 */ float m_armorDamageModifier;
    /* 01FC */ int32_t m_duration;
    /* 0200 */ int32_t m_itemLimitCategory;
    /* 0204 */ int32_t m_holidayID;
    /* 0208 */ float m_statScalingFactor;
    /* 020C */ int32_t m_currencySubstitutionID;
    /* 0210 */ int32_t m_currencySubstitutionCount;
};

extern WowClientDB2<ItemRecSparse> g_itemSparseDB;

#endif
