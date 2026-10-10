#ifndef WOW_SOURCE_UI_CURRENCYTYPES_H
#define WOW_SOURCE_UI_CURRENCYTYPES_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class ClientSetCurrency;
class ClientSetupCurrency;
class ClientUpdateCurrencyWeekLimit;
class ClientWeeklyResetCurrency;
class CurrencyTypesRec;
struct lua_State;

struct PlayerCurrency : public TSLinkedNode<PlayerCurrency> {
    /* 0008 */ const CurrencyTypesRec* m_rec;
    /* 000C */ int32_t m_quantity;
    /* 0010 */ int32_t m_weeklyQuantity;
    /* 0014 */ int32_t m_weeklyCap;
    /* 0018 */ int32_t m_trackedQuantity;
    /* 001C */ uint32_t m_flags;
};

class CGCurrencyTypes {
    public:
    struct CurrencyCategoryInfo {
        /* 0000 */ int32_t m_id;
        /* 0004 */ int32_t m_expanded;
        /* 0008 */ const char* m_name;
        /* 000C */ int32_t m_flags;
    };

    struct CurrencyListInfo {
        /* 0000 */ int32_t m_currencyID;
        /* 0004 */ int32_t m_categoryID;
        /* 0008 */ int32_t m_isHeader;
        /* 000C */ int32_t m_isExpanded;
        /* 0010 */ int32_t m_isWatched;
    };

    static TSList<PlayerCurrency, TSGetLink<PlayerCurrency> > s_currencies;
    static int32_t s_unusedCategory;
    static TSGrowableArray<CurrencyCategoryInfo> s_categories;
    static TSGrowableArray<CurrencyListInfo> s_displayList;

    static int32_t CurrencyCategorySortCallback(const void* a, const void* b);
    static int32_t DisplayListSortCallback(const void* a, const void* b);
    static CurrencyListInfo* GetDisplayListInfo(uint32_t index);
    static void SetCategoryExpand(uint32_t index, int32_t expand);
    static const CurrencyTypesRec* GetCurrencyRec(uint32_t index);
    static const char* GetCurrencyIcon(const CurrencyTypesRec* rec);
    static int32_t CategoryHasKnownTypes(uint32_t index);
    static void BuildDisplayList();
    static PlayerCurrency* GetCurrency(int32_t id);
    static bool HasCurrency(int32_t id, int32_t count);
    static bool HasEarnedCurrency(int32_t id, int32_t count);
    static void SetCurrencyFlags(int32_t id, uint32_t flag, int32_t set);
    static bool HasCurrencyFlags(int32_t id, uint32_t flag);
    static void EnterWorld();
    static const CurrencyTypesRec* GetBackpackCurrencyInfo(int32_t index);
    static void SetUnused(uint32_t index, int32_t unused);
    static int32_t IsCurrencyUnused(uint32_t index);
    static void SetBackpack(uint32_t index, int32_t backpack);
    static void SendChatCurrency();
    static PlayerCurrency* CreateCurrency(int32_t id, int32_t quantity, int32_t weeklyQuantity, int32_t weeklyCap, int32_t trackedQuantity, uint32_t flags);
    static bool SetCurrency(int32_t id, int32_t quantity, int32_t weeklyQuantity, int32_t trackedQuantity, bool suppressChatLog);
    static void ShutdownGame();
    static void InitializeGame();
};

void CurrencyTypesRegisterScriptFunctions();
void CurrencyTypesUnregisterScriptFunctions();

#endif
