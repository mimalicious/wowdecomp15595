#include "CurrencyTypes.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/FactionGroupRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D14EE8
static int32_t s_lastGainedCurrency = -1;

// STATIC: 0x00D14F40
TSList<PlayerCurrency, TSGetLink<PlayerCurrency> > CGCurrencyTypes::s_currencies;

// STATIC: 0x00F4B848
static char s_currencyIcon[260];

// STATIC: 0x00F4B94C
static int32_t s_gainedAmount;

// STATIC: 0x00F4B950
static uint32_t s_dumpTimer;

// STATIC: 0x00F4B954
int32_t CGCurrencyTypes::s_unusedCategory;

// STATIC: 0x00F4B958
TSGrowableArray<CGCurrencyTypes::CurrencyCategoryInfo> CGCurrencyTypes::s_categories;

// STATIC: 0x00F4B968
TSGrowableArray<CGCurrencyTypes::CurrencyListInfo> CGCurrencyTypes::s_displayList;

// OFFSET: 0x0099F760
static int32_t DumpCurrencyToChat(const void* data, void* param) {
    CGCurrencyTypes::SendChatCurrency();
    return 1;
}

// OFFSET: 0x0099F770/0x140723740
int32_t CGCurrencyTypes::CurrencyCategorySortCallback(const void* a, const void* b) {
    const CurrencyCategoryInfo* first = static_cast<const CurrencyCategoryInfo*>(a);
    const CurrencyCategoryInfo* second = static_cast<const CurrencyCategoryInfo*>(b);

    if (!first || !second) {
        return 0;
    }

    int32_t result = (first->m_flags & 0x1) - (second->m_flags & 0x1);

    if (!result) {
        return SStrSortUTF8I(first->m_name, second->m_name, 0x7FFFFFFF);
    }

    return result;
}

// OFFSET: 0x0099F9B0
int32_t CGCurrencyTypes::DisplayListSortCallback(const void* a, const void* b) {
    const CurrencyListInfo* first = static_cast<const CurrencyListInfo*>(a);
    const CurrencyListInfo* second = static_cast<const CurrencyListInfo*>(b);

    if (!first || !second) {
        return 0;
    }

    if (first->m_categoryID != second->m_categoryID) {
        CurrencyCategoryRec* firstCategory = g_currencyCategoryDB.GetRecord(first->m_categoryID);
        CurrencyCategoryRec* secondCategory = g_currencyCategoryDB.GetRecord(second->m_categoryID);
        return SStrSortUTF8I(firstCategory->m_name, secondCategory->m_name, 0x7FFFFFFF);
    }

    if (first->m_isHeader || second->m_isHeader) {
        return 2 * (first->m_isHeader == 0) - 1;
    }

    CurrencyTypesRec* firstRec = g_currencyTypesDB.GetRecord(first->m_currencyID);
    CurrencyTypesRec* secondRec = g_currencyTypesDB.GetRecord(second->m_currencyID);

    if (!firstRec) {
        return 1;
    }

    if (!secondRec) {
        return -1;
    }

    const char* firstName = firstRec->m_name;
    const char* secondName = secondRec->m_name;

    if (!firstName) {
        return secondName != NULL;
    }

    if (!secondName) {
        return -1;
    }

    return SStrSortUTF8I(firstName, secondName, 0x7FFFFFFF);
}

// OFFSET: 0x0099FAB0
CGCurrencyTypes::CurrencyListInfo* CGCurrencyTypes::GetDisplayListInfo(uint32_t index) {
    if (index >= CGCurrencyTypes::s_displayList.Count()) {
        return NULL;
    }

    return &CGCurrencyTypes::s_displayList[index];
}

// OFFSET: 0x0099FAD0
void CGCurrencyTypes::SetCategoryExpand(uint32_t index, int32_t expand) {
    if (index >= CGCurrencyTypes::s_displayList.Count() || !CGCurrencyTypes::s_displayList[index].m_isHeader) {
        return;
    }

    for (uint32_t i = 0; i < CGCurrencyTypes::s_categories.Count(); i++) {
        if (CGCurrencyTypes::s_categories[i].m_id == CGCurrencyTypes::s_displayList[index].m_categoryID) {
            CGCurrencyTypes::s_categories[i].m_expanded = expand;
            return;
        }
    }
}

// OFFSET: 0x0099FB30
const CurrencyTypesRec* CGCurrencyTypes::GetCurrencyRec(uint32_t index) {
    if (index >= CGCurrencyTypes::s_displayList.Count()) {
        return NULL;
    }

    return g_currencyTypesDB.GetRecord(CGCurrencyTypes::s_displayList[index].m_currencyID);
}

// OFFSET: 0x0099FB70
const char* CGCurrencyTypes::GetCurrencyIcon(const CurrencyTypesRec* rec) {
    if (rec->m_id == 392 || rec->m_id == 390) {
        SStrCopy(s_currencyIcon, "inv_misc_questionmark", sizeof(s_currencyIcon));
        ChrRacesRec* race = g_chrRacesDB.GetRecord(ClientServices::GetCharacterRace());

        if (race) {
            FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(race->m_factionID);

            if (faction) {
                for (int32_t i = 0; i < g_factionGroupDB.m_numRecords; i++) {
                    FactionGroupRec* group = g_factionGroupDB.GetRecordByIndex(i);

                    if (((1 << group->m_maskID) & faction->m_factionGroup) && *group->m_name) {
                        if (rec->m_id == 390) {
                            SStrPrintf(s_currencyIcon, sizeof(s_currencyIcon), "PVPCurrency-Conquest-%s", group->m_internalName);
                        } else {
                            SStrPrintf(s_currencyIcon, sizeof(s_currencyIcon), "PVPCurrency-Honor-%s", group->m_internalName);
                        }
                    }
                }
            }
        }

        return s_currencyIcon;
    }

    if (ViolenceGetLevel() >= 2) {
        return rec->m_inventoryIcon;
    }

    const char* icon = rec->m_inventoryIcon2;

    if (!icon || !*icon) {
        return rec->m_inventoryIcon;
    }

    return icon;
}

// OFFSET: 0x0099FD90
int32_t CGCurrencyTypes::CategoryHasKnownTypes(uint32_t index) {
    if (index > CGCurrencyTypes::s_categories.Count()) {
        return 0;
    }

    int32_t id = CGCurrencyTypes::s_categories.m_data[index].m_id;

    for (PlayerCurrency* currency = CGCurrencyTypes::s_currencies.Head(); currency; currency = CGCurrencyTypes::s_currencies.Next(currency)) {
        const CurrencyTypesRec* rec = currency->m_rec;

        if (!rec) {
            continue;
        }

        CurrencyCategoryRec* category = g_currencyCategoryDB.GetRecord(rec->m_categoryID);

        if (category && (category->m_flags & 0x4)) {
            continue;
        }

        int32_t categoryID = (currency->m_flags & 0x8) ? CGCurrencyTypes::s_unusedCategory : rec->m_categoryID;

        if (id == categoryID) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0099FE40
void CGCurrencyTypes::BuildDisplayList() {
    CGCurrencyTypes::s_displayList.m_count = 0;

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    for (uint32_t i = 0; i < CGCurrencyTypes::s_categories.Count(); i++) {
        if (!CGCurrencyTypes::CategoryHasKnownTypes(i)) {
            continue;
        }

        CurrencyListInfo* header = CGCurrencyTypes::s_displayList.New();
        header->m_isHeader = 1;
        header->m_isExpanded = CGCurrencyTypes::s_categories[i].m_expanded;
        header->m_categoryID = CGCurrencyTypes::s_categories[i].m_id;
        header->m_currencyID = 0;
        header->m_isWatched = 0;
    }

    for (PlayerCurrency* currency = CGCurrencyTypes::s_currencies.Head(); currency; currency = CGCurrencyTypes::s_currencies.Next(currency)) {
        const CurrencyTypesRec* rec = currency->m_rec;

        if (!rec) {
            continue;
        }

        CurrencyCategoryRec* category = g_currencyCategoryDB.GetRecord(rec->m_categoryID);

        if (category->m_flags & 0x4) {
            continue;
        }

        int32_t categoryID = (currency->m_flags & 0x8) ? CGCurrencyTypes::s_unusedCategory : rec->m_categoryID;
        uint32_t j = 0;

        for (; j < CGCurrencyTypes::s_displayList.Count(); j++) {
            if (CGCurrencyTypes::s_displayList[j].m_categoryID == categoryID) {
                break;
            }
        }

        if (j < CGCurrencyTypes::s_displayList.Count() && !CGCurrencyTypes::s_displayList[j].m_isExpanded) {
            continue;
        }

        CurrencyListInfo* entry = CGCurrencyTypes::s_displayList.New();
        entry->m_currencyID = rec->m_id;
        entry->m_isWatched = currency->m_flags & 0x4;
        entry->m_isHeader = 0;
        entry->m_categoryID = categoryID;
    }

    qsort(CGCurrencyTypes::s_displayList.m_data, CGCurrencyTypes::s_displayList.Count(), sizeof(CurrencyListInfo), CGCurrencyTypes::DisplayListSortCallback);
}

// OFFSET: 0x009A00B0
PlayerCurrency* CGCurrencyTypes::GetCurrency(int32_t id) {
    for (PlayerCurrency* currency = CGCurrencyTypes::s_currencies.Head(); currency; currency = CGCurrencyTypes::s_currencies.Next(currency)) {
        if (currency->m_rec->m_id == id) {
            return currency;
        }
    }

    return NULL;
}

// OFFSET: 0x009A00F0
bool CGCurrencyTypes::HasCurrency(int32_t id, int32_t count) {
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(id);

    if (!currency) {
        return false;
    }

    int32_t quantity = currency->m_quantity;

    if (currency->m_rec->m_flags & 0x8) {
        quantity *= 100;
    }

    return quantity >= count;
}

// OFFSET: 0x009A0140
bool CGCurrencyTypes::HasEarnedCurrency(int32_t id, int32_t count) {
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(id);

    if (!currency) {
        return false;
    }

    int32_t tracked = currency->m_trackedQuantity;

    if (currency->m_rec->m_flags & 0x8) {
        return tracked >= count / 100;
    }

    return tracked >= count;
}

// OFFSET: 0x009A01B0
void CGCurrencyTypes::SetCurrencyFlags(int32_t id, uint32_t flag, int32_t set) {
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(id);

    if (!currency) {
        return;
    }

    if (set) {
        currency->m_flags |= flag;
    } else {
        currency->m_flags &= ~flag;
    }

    PlayerSetCurrencyFlags message;
    message.m_type = id;
    message.m_flags = currency->m_flags;
    ClientServices::Send(&message);
}

// OFFSET: 0x009A0230
bool CGCurrencyTypes::HasCurrencyFlags(int32_t id, uint32_t flag) {
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(id);

    if (!currency) {
        return false;
    }

    return (flag & currency->m_flags) != 0;
}

// OFFSET: 0x009A04E0
void CGCurrencyTypes::EnterWorld() {
    CGCurrencyTypes::BuildDisplayList();
    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENCY_DISPLAY_UPDATE, NULL);
}

// OFFSET: 0x009A0500
static bool OnWeeklyResetCurrency(ClientWeeklyResetCurrency* msg) {
    for (PlayerCurrency* currency = CGCurrencyTypes::s_currencies.Head(); currency; currency = CGCurrencyTypes::s_currencies.Next(currency)) {
        currency->m_weeklyQuantity = 0;
    }

    CGCurrencyTypes::BuildDisplayList();
    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENCY_DISPLAY_UPDATE, NULL);
    return true;
}

// OFFSET: 0x009A0540
const CurrencyTypesRec* CGCurrencyTypes::GetBackpackCurrencyInfo(int32_t index) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return NULL;
    }

    int32_t found = 0;

    for (int32_t i = 0; i < g_currencyTypesDB.m_numRecords; i++) {
        CurrencyTypesRec* rec = g_currencyTypesDB.GetRecordByIndex(i);

        if (!rec || !CGCurrencyTypes::HasCurrencyFlags(rec->m_id, 0x4)) {
            continue;
        }

        if (found == index) {
            return rec;
        }

        found++;
    }

    return NULL;
}

// OFFSET: 0x009A05D0
void CGCurrencyTypes::SetUnused(uint32_t index, int32_t unused) {
    if (index >= CGCurrencyTypes::s_displayList.Count() || CGCurrencyTypes::s_displayList[index].m_isHeader) {
        return;
    }

    const CurrencyTypesRec* rec = CGCurrencyTypes::GetCurrencyRec(index);

    if (rec) {
        CGCurrencyTypes::SetCurrencyFlags(rec->m_id, 0x8, unused);
    }
}

// OFFSET: 0x009A0610
int32_t CGCurrencyTypes::IsCurrencyUnused(uint32_t index) {
    if (index >= CGCurrencyTypes::s_displayList.Count()) {
        return 0;
    }

    if (CGCurrencyTypes::s_displayList[index].m_isHeader) {
        return CGCurrencyTypes::s_displayList[index].m_categoryID == CGCurrencyTypes::s_categories[CGCurrencyTypes::s_categories.Count() - 1].m_id;
    }

    const CurrencyTypesRec* rec = CGCurrencyTypes::GetCurrencyRec(index);

    if (!rec) {
        return 0;
    }

    return CGCurrencyTypes::HasCurrencyFlags(rec->m_id, 0x8);
}

// OFFSET: 0x009A0680
void CGCurrencyTypes::SetBackpack(uint32_t index, int32_t backpack) {
    if (index >= CGCurrencyTypes::s_displayList.Count() || CGCurrencyTypes::s_displayList[index].m_isHeader) {
        return;
    }

    const CurrencyTypesRec* rec = CGCurrencyTypes::GetCurrencyRec(index);

    if (rec) {
        CGCurrencyTypes::SetCurrencyFlags(rec->m_id, 0x4, backpack);
    }
}

// OFFSET: 0x009A06C0
void CGCurrencyTypes::SendChatCurrency() {
    ClientKillTimer(s_dumpTimer, DumpCurrencyToChat, "DumpCurrencyToChat");
    s_dumpTimer = 0;

    for (PlayerCurrency* currency = CGCurrencyTypes::s_currencies.Head(); currency; currency = CGCurrencyTypes::s_currencies.Next(currency)) {
        if (currency->m_rec->m_id != s_lastGainedCurrency) {
            continue;
        }

        if (s_gainedAmount > 0) {
            char text[1024];
            SStrPrintf(text, sizeof(text), FrameScript_GetText("CURRENCY_GAINED_MULTIPLE", -1, GENDER_NOT_APPLICABLE), CGTooltip::GetCurrencyHyperlinkString(s_lastGainedCurrency), s_gainedAmount);
            CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(64), NULL);
        }

        break;
    }

    s_gainedAmount = 0;
    s_lastGainedCurrency = -1;
    s_dumpTimer = 0;
}

// OFFSET: 0x009A0CB0
PlayerCurrency* CGCurrencyTypes::CreateCurrency(int32_t id, int32_t quantity, int32_t weeklyQuantity, int32_t weeklyCap, int32_t trackedQuantity, uint32_t flags) {
    PlayerCurrency* currency = CGCurrencyTypes::s_currencies.NewNode(2, 0, 0);
    currency->m_rec = g_currencyTypesDB.GetRecord(id);
    currency->m_quantity = quantity;
    currency->m_weeklyQuantity = weeklyQuantity;
    currency->m_weeklyCap = weeklyCap;
    currency->m_trackedQuantity = trackedQuantity;
    currency->m_flags = flags;
    return currency;
}

// OFFSET: 0x009A0D80
bool CGCurrencyTypes::SetCurrency(int32_t id, int32_t quantity, int32_t weeklyQuantity, int32_t trackedQuantity, bool suppressChatLog) {
    bool created = false;
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(id);

    if (!currency) {
        CurrencyTypesRec* rec = g_currencyTypesDB.GetRecord(id);
        int32_t weeklyCap = 0;

        if (rec->m_weeklyCap > 0) {
            weeklyCap = rec->m_weeklyCap;

            if (rec->m_flags & 0x8) {
                weeklyCap /= 100;
            }
        }

        currency = CGCurrencyTypes::CreateCurrency(id, 0, weeklyQuantity, weeklyCap, trackedQuantity, 0);
        created = true;
    }

    int32_t previous = currency->m_quantity;
    currency->m_quantity = quantity;
    currency->m_weeklyQuantity = weeklyQuantity;
    currency->m_trackedQuantity = trackedQuantity;

    if (suppressChatLog) {
        return created;
    }

    if (previous > quantity) {
        CGCurrencyTypes::SendChatCurrency();
        return created;
    }

    if (id == s_lastGainedCurrency) {
        s_gainedAmount += quantity - previous;
    } else {
        CGCurrencyTypes::SendChatCurrency();
        s_gainedAmount = currency->m_quantity - previous;
        s_lastGainedCurrency = id;
    }

    s_dumpTimer = ClientSetTimer(1000, DumpCurrencyToChat, NULL);
    return created;
}

// OFFSET: 0x009A0F00
void CGCurrencyTypes::ShutdownGame() {
    CGCurrencyTypes::s_currencies.Clear();
    ClientSetupCurrency::s_handler = NULL;
    ClientSetCurrency::s_handler = NULL;
    ClientUpdateCurrencyWeekLimit::s_handler = NULL;
    ClientWeeklyResetCurrency::s_handler = NULL;
    ClientKillTimer(s_dumpTimer, DumpCurrencyToChat, "DumpCurrencyToChat");
    s_gainedAmount = 0;
    s_dumpTimer = 0;
    s_lastGainedCurrency = -1;
}

// OFFSET: 0x009A0F60
static bool OnSetupCurrency(ClientSetupCurrency* msg) {
    CGCurrencyTypes::s_currencies.Clear();

    for (uint32_t i = 0; i < msg->m_data.Count(); i++) {
        const JamClientCurrencyData& data = msg->m_data[i];
        int32_t weeklyQuantity = data.m_hasWeeklyQuantity ? data.m_weeklyQuantity : 0;
        int32_t weeklyCap;

        if (data.m_hasMaxWeeklyQuantity) {
            weeklyCap = data.m_maxWeeklyQuantity;
        } else {
            CurrencyTypesRec* rec = g_currencyTypesDB.GetRecord(data.m_type);
            weeklyCap = rec->m_weeklyCap;

            if (rec->m_flags & 0x8) {
                weeklyCap /= 100;
            }
        }

        int32_t trackedQuantity = data.m_hasTrackedQuantity ? data.m_trackedQuantity : 0;
        CGCurrencyTypes::CreateCurrency(data.m_type, data.m_quantity, weeklyQuantity, weeklyCap, trackedQuantity, data.m_flags);
    }

    CGCurrencyTypes::BuildDisplayList();
    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENCY_DISPLAY_UPDATE, NULL);
    return true;
}

// OFFSET: 0x009A1050
static bool OnSetCurrency(ClientSetCurrency* msg) {
    int32_t weeklyQuantity = msg->m_hasWeeklyQuantity ? msg->m_weeklyQuantity : 0;
    int32_t trackedQuantity = msg->m_hasTrackedQuantity ? msg->m_trackedQuantity : 0;

    if (CGCurrencyTypes::SetCurrency(msg->m_type, msg->m_quantity, weeklyQuantity, trackedQuantity, msg->m_suppressChatLog)) {
        CGCurrencyTypes::BuildDisplayList();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CURRENCY_DISPLAY_UPDATE, NULL);
    CGQuestLog::UpdateQuestCurrency(msg->m_type);
    return true;
}

// OFFSET: 0x009A10C0
static bool OnUpdateCurrencyWeekLimit(ClientUpdateCurrencyWeekLimit* msg) {
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(msg->m_type);

    if (!currency) {
        currency = CGCurrencyTypes::CreateCurrency(msg->m_type, 0, 0, 0, 0, 0);
    }

    if (currency && currency->m_rec && (currency->m_rec->m_flags & 0x4)) {
        currency->m_weeklyCap = msg->m_weekCap;
    }

    return true;
}

// OFFSET: 0x009A1130
void CGCurrencyTypes::InitializeGame() {
    CGCurrencyTypes::s_categories.Reserve(16, 0);
    CGCurrencyTypes::s_categories.m_count = 0;

    for (int32_t i = 0; i < g_currencyCategoryDB.m_numRecords; i++) {
        CurrencyCategoryRec* rec = g_currencyCategoryDB.GetRecordByIndex(i);

        if (rec->m_flags & 0x2) {
            CGCurrencyTypes::s_unusedCategory = rec->m_id;
        }

        if (rec->m_flags & 0x4) {
            continue;
        }

        CurrencyCategoryInfo* category = CGCurrencyTypes::s_categories.New();
        category->m_id = rec->m_id;
        category->m_name = rec->m_name;
        category->m_flags = rec->m_flags;
        category->m_expanded = 1;
    }

    qsort(CGCurrencyTypes::s_categories.m_data, CGCurrencyTypes::s_categories.Count(), sizeof(CurrencyCategoryInfo), CGCurrencyTypes::CurrencyCategorySortCallback);
    ClientSetupCurrency::s_handler = OnSetupCurrency;
    ClientSetCurrency::s_handler = OnSetCurrency;
    ClientUpdateCurrencyWeekLimit::s_handler = OnUpdateCurrencyWeekLimit;
    ClientWeeklyResetCurrency::s_handler = OnWeeklyResetCurrency;
}
