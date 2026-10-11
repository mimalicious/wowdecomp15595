#include "DBCacheInstances.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Game/GameClient/WardenClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ArenaTeamCache.h"
#include "WoW/Source/Object/CGPetition.h"
#include "WoW/Source/Object/DanceCache.h"
#include "WoW/Source/Object/ItemTextCache.h"
#include "WoW/Source/Object/NPCText.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/PageTextCache.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00D97EF8
DBCache<CreatureStats_C, int32_t, HASHKEY_INT> g_creatureDBCache('WMOB', "creaturecache.wdb", CMSG_QUERY_CREATURE, 0, true, true, 0);

// STATIC: 0x00D97F88
DBCache<GameObjectStats_C, int32_t, HASHKEY_INT> g_gameObjectDBCache('WGOB', "gameobjectcache.wdb", CMSG_QUERY_GAME_OBJECT, 0, true, true, 0);

// STATIC: 0x00D98018
DBCache<NPCText, int32_t, HASHKEY_INT> g_npcTextDBCache('WNPC', "npccache.wdb", CMSG_QUERY_NPC_TEXT, 0, true, true, 0);

// STATIC: 0x00D980A8
DBCache<NameCache, uint64_t, CHashKeyGUID> g_nameDBCache('WNAM', "namecache.wdb", CMSG_NAME_QUERY, 0, false, false, 256);

// STATIC: 0x00D98138
DBCache<GuildStats_C, uint64_t, CHashKeyGUID> g_guildInfoCache('WGLD', "guildcache.wdb", CMSG_QUERY_GUILD_INFO, 0, false, false, 0);

// STATIC: 0x00D981C8
DBCache<QuestCache, int32_t, HASHKEY_INT> g_questDBCache('WQST', "questcache.wdb", CMSG_QUERY_QUEST_INFO, 0, false, true, 0);

// STATIC: 0x00D98258
DBCache<PageTextCache_C, int32_t, HASHKEY_INT> g_pageTextCache('WPTX', "pagetextcache.wdb", CMSG_QUERY_PAGE_TEXT, 0, true, true, 0);

// STATIC: 0x00D982E8
DBCache<PetNameCache, int32_t, HASHKEY_INT> g_petNameCache('WPNM', "petnamecache.wdb", CMSG_QUERY_PET_NAME, 0, true, false, 0);

// STATIC: 0x00D98378
DBCache<CGPetition, int32_t, HASHKEY_INT> g_petitionCache('WPTN', "petitioncache.wdb", CMSG_PETITION_QUERY, 0, true, false, 0);

// STATIC: 0x00D98408
DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID> g_itemTextDBCache('WITX', "itemtextcache.wdb", CMSG_ITEM_TEXT_QUERY, 0, false, true, 0);

// STATIC: 0x00D98498
DBCache<WardenCachedModule, CWardenKey, CWardenKey> g_wardenCache('WRDN', "wowcache.wdb", 0, 0, true, true, 0);

// STATIC: 0x00D98528
DBCache<ArenaTeamCache, int32_t, HASHKEY_INT> g_arenaTeamCache('WATM', "arenateamcache.wdb", CMSG_ARENA_TEAM_QUERY, 0, false, false, 0);

// STATIC: 0x00D985B8
DBCache<DanceCache, int32_t, HASHKEY_INT> g_danceCache('WDAN', "dancecache.wdb", CMSG_DANCE_QUERY, 0, false, false, 0);

// OFFSET: 0x00496AD0
static void LoadDBCaches() {
    g_creatureDBCache.Load();
    g_gameObjectDBCache.Load();
    g_npcTextDBCache.Load();
    g_nameDBCache.Load();
    g_guildInfoCache.Load();
    g_questDBCache.Load();
    g_pageTextCache.Load();
    g_petNameCache.Load();
    g_petitionCache.Load();
    g_itemTextDBCache.Load();
    g_wardenCache.Load();
    g_arenaTeamCache.Load();
    g_danceCache.Load();
}

// OFFSET: 0x00496BE0
static int32_t ReceiveCreature(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    g_creatureDBCache.AddItems(msg, true);

    return 1;
}

// OFFSET: 0x00496C00
static int32_t ReceiveGameObject(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    g_gameObjectDBCache.AddItems(msg, true);

    return 1;
}

// OFFSET: 0x00496C20
static int32_t ReceiveNPCText(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    g_npcTextDBCache.AddItems(msg, true);

    return 1;
}

// OFFSET: 0x00496C40
static int32_t ReceiveQuest(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    QuestCache quest;
    quest.Unpack(msg);

    if (quest.m_name[0]) {
        g_questDBCache.AddItem(&quest, quest.m_id);
    } else {
        g_questDBCache.DenyItem(quest.m_id);
    }

    return 1;
}

// OFFSET: 0x00496CB0
static int32_t ReceivePageText(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    g_pageTextCache.AddItems(msg, true);

    return 1;
}

// OFFSET: 0x00496CD0
static int32_t ReceivePetName(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    PetNameCache petName;
    msg->Get(petName.m_petNumber);
    msg->GetString(petName.m_name, sizeof(petName.m_name));
    msg->Get(petName.m_timestamp);

    uint8_t hasDeclinedNames;
    msg->Get(hasDeclinedNames);

    if (hasDeclinedNames) {
        petName.m_declinedNames = new (__FILE__, __LINE__) PetNameCache::DeclinedNames;

        for (uint32_t i = 0; i < sizeof(petName.m_declinedNames->m_names) / sizeof(petName.m_declinedNames->m_names[0]); i++) {
            msg->GetString(petName.m_declinedNames->m_names[i], sizeof(petName.m_declinedNames->m_names[i]));
        }
    }

    if (petName.m_name[0]) {
        g_petNameCache.AddItem(&petName, petName.m_petNumber);
    } else {
        g_petNameCache.DenyItem(petName.m_petNumber);
    }

    return 1;
}

// OFFSET: 0x00496DB0
static int32_t ReceivePetition(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPetition petition;
    petition.Unpack(msg);

    if (petition.m_id > 0) {
        g_petitionCache.AddItem(&petition, petition.m_id);
    } else {
        g_petitionCache.DenyItem(-petition.m_id);
    }

    return 1;
}

// OFFSET: 0x00496E20
static int32_t ReceiveInvalidatePlayer(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if ((guid.m_guid & 0xF000000000000000ULL) == 0x1000000000000000ULL && (guid.m_guid & 0x0FF0000000000000ULL) == 0x0FF0000000000000ULL) {
        g_guildInfoCache.Invalidate(static_cast<int32_t>(guid.m_guid));
    } else {
        g_nameDBCache.Invalidate(guid.m_guid);
        CGGameUI::InvalidatePlayer(guid);
    }

    return 1;
}

// OFFSET: 0x00496EA0
static int32_t ReceiveArenaTeam(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    ArenaTeamCache arenaTeam;
    arenaTeam.Unpack(msg);

    if (arenaTeam.m_name[0]) {
        g_arenaTeamCache.AddItem(&arenaTeam, arenaTeam.m_id);
    } else {
        g_arenaTeamCache.DenyItem(arenaTeam.m_id);
    }

    return 1;
}

// OFFSET: 0x00496EF0
static int32_t ReceiveInvalidateDance(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t danceID;
    msg->Get(danceID);
    g_danceCache.Invalidate(danceID);

    return 1;
}

// OFFSET: 0x00497050/0x1400BD3C0
void SetClientCacheVersion(uint32_t version) {
    g_creatureDBCache.VerifyAndUpdateVersion(version);
    g_creatureDBCache.VerifyAndUpdateVersion(version);
    g_gameObjectDBCache.VerifyAndUpdateVersion(version);
    g_npcTextDBCache.VerifyAndUpdateVersion(version);
    g_nameDBCache.VerifyAndUpdateVersion(version);
    g_guildInfoCache.VerifyAndUpdateVersion(version);
    g_questDBCache.VerifyAndUpdateVersion(version);
    g_pageTextCache.VerifyAndUpdateVersion(version);
    g_petNameCache.VerifyAndUpdateVersion(version);
    g_petitionCache.VerifyAndUpdateVersion(version);
    g_itemTextDBCache.VerifyAndUpdateVersion(version);
    g_wardenCache.VerifyAndUpdateVersion(version);
    g_arenaTeamCache.VerifyAndUpdateVersion(version);
    g_danceCache.VerifyAndUpdateVersion(version);
}

// OFFSET: 0x00497100
static int32_t ReceiveName(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    NameCache name;
    SmartGUID guid;
    *msg >> guid;
    name.m_guid.m_guid = guid.m_guid;

    uint8_t result;
    msg->Get(result);

    switch (result) {
        case 0: {
            msg->GetString(name.m_name, sizeof(name.m_name));
            msg->GetString(name.m_realmName, sizeof(name.m_realmName));

            uint8_t race;
            uint8_t sex;
            uint8_t classID;
            msg->Get(race);
            msg->Get(sex);
            msg->Get(classID);
            name.m_race = race;
            name.m_sex = sex;
            name.m_class = classID;

            uint8_t hasDeclinedNames;
            msg->Get(hasDeclinedNames);

            if (hasDeclinedNames) {
                name.m_declinedNames = new (__FILE__, __LINE__) NameCache::DeclinedNames;

                for (uint32_t i = 0; i < sizeof(name.m_declinedNames->m_names) / sizeof(name.m_declinedNames->m_names[0]); i++) {
                    msg->GetString(name.m_declinedNames->m_names[i], sizeof(name.m_declinedNames->m_names[i]));
                }
            }

            g_nameDBCache.AddItem(&name, name.m_guid.m_guid);

            if ((name.m_guid.m_guid & 0xF000000000000000ULL) == 0x1000000000000000ULL && (name.m_guid.m_guid & 0x0FF0000000000000ULL) == 0x0FD0000000000000ULL) {
                g_nameDBCache.SetTemporary(name.m_guid.m_guid);
            }

            break;
        }

        case 2:
            g_nameDBCache.RetryItem(name.m_guid.m_guid);
            break;

        case 3:
            SStrCopy(name.m_name, "?", sizeof(name.m_name));
            memset(name.m_realmName, 0, sizeof(name.m_realmName));
            name.m_race = 0;
            name.m_sex = 0;
            name.m_class = 0;
            name.m_declinedNames = NULL;
            g_nameDBCache.AddItem(&name, name.m_guid.m_guid);
            g_nameDBCache.SetTemporary(name.m_guid.m_guid);
            break;

        default:
            g_nameDBCache.DenyItem(name.m_guid.m_guid);
            break;
    }

    return 1;
}

// OFFSET: 0x00497330
static int32_t ReceiveGuildInfo(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    GuildStats_C guild;
    guild.Unpack(msg);

    if (guild.m_name[0]) {
        g_guildInfoCache.AddItem(&guild, guild.m_guildGuid);
    } else {
        g_guildInfoCache.DenyItem(guild.m_guildGuid);
    }

    return 1;
}

// OFFSET: 0x00497390
static int32_t ReceiveItemText(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    ItemTextCache_C itemText;

    uint8_t result;
    msg->Get(result);

    if (!result) {
        WOWGUID itemGuid;
        msg->Get(itemGuid.m_guid);
        msg->GetString(itemText.m_text, sizeof(itemText.m_text));
        g_itemTextDBCache.AddItem(&itemText, itemGuid.m_guid);
    }

    return 1;
}

// OFFSET: 0x00497400
static int32_t ReceiveDance(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    DanceCache dance;
    dance.Unpack(msg);

    if (dance.m_id & 0x80000000) {
        g_danceCache.DenyItem(dance.m_id & 0x7FFFFFFF);
    } else {
        g_danceCache.AddItem(&dance, dance.m_id);
    }

    return 1;
}

// PROVEN: 2b008480 IDENTICAL/NONE
// OFFSET: 0x00496F20/0x1400BD250
void DBCache_Initialize() {
    LoadDBCaches();
}

// OFFSET: 0x00496F40
void DBCache_Destroy() {
    g_creatureDBCache.Destroy();
    g_gameObjectDBCache.Destroy();
    g_npcTextDBCache.Destroy();
    g_nameDBCache.Destroy();
    g_guildInfoCache.Destroy();
    g_questDBCache.Destroy();
    g_pageTextCache.Destroy();
    g_petNameCache.Destroy();
    g_petitionCache.Destroy();
    g_itemTextDBCache.Destroy();
    g_wardenCache.Destroy();
    g_arenaTeamCache.Destroy();
    g_danceCache.Destroy();
}

// OFFSET: 0x004975F0
void DBCache_ClearHandlers() {
    ClientServices::ClearMessageHandler(SMSG_QUERY_CREATURE_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_GAME_OBJECT_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_NPC_TEXT_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_PLAYER_NAME_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_GUILD_INFO_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_QUEST_INFO_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_PAGE_TEXT_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_PET_NAME_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_PETITION_QUERY_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_QUERY_ITEM_TEXT_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_INVALIDATE_PLAYER);
    ClientServices::ClearMessageHandler(SMSG_ARENA_TEAM_QUERY_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_INVALIDATE_DANCE);
    ClientServices::ClearMessageHandler(SMSG_DANCE_QUERY_RESPONSE);
}

// OFFSET: 0x00497490
void DBCache_RegisterHandlers() {
    ClientServices::SetMessageHandler(SMSG_QUERY_CREATURE_RESPONSE, ReceiveCreature, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_GAME_OBJECT_RESPONSE, ReceiveGameObject, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_NPC_TEXT_RESPONSE, ReceiveNPCText, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_PLAYER_NAME_RESPONSE, ReceiveName, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_GUILD_INFO_RESPONSE, ReceiveGuildInfo, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_QUEST_INFO_RESPONSE, ReceiveQuest, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_PAGE_TEXT_RESPONSE, ReceivePageText, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_PET_NAME_RESPONSE, ReceivePetName, NULL);
    ClientServices::SetMessageHandler(SMSG_PETITION_QUERY_RESPONSE, ReceivePetition, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_ITEM_TEXT_RESPONSE, ReceiveItemText, NULL);
    ClientServices::SetMessageHandler(SMSG_INVALIDATE_PLAYER, ReceiveInvalidatePlayer, NULL);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_QUERY_RESPONSE, ReceiveArenaTeam, NULL);
    ClientServices::SetMessageHandler(SMSG_INVALIDATE_DANCE, ReceiveInvalidateDance, NULL);
    ClientServices::SetMessageHandler(SMSG_DANCE_QUERY_RESPONSE, ReceiveDance, NULL);
}

// OFFSET: 0x004976F0/0x1400BDB80
void DBCache_ClearTemporaryCaches() {
    if (!g_creatureDBCache.m_enabled) {
        g_creatureDBCache.Clear();
    }

    if (!g_gameObjectDBCache.m_enabled) {
        g_gameObjectDBCache.Clear();
    }

    if (!g_npcTextDBCache.m_enabled) {
        g_npcTextDBCache.Clear();
    }

    if (!g_nameDBCache.m_enabled) {
        g_nameDBCache.Clear();
    }

    if (!g_guildInfoCache.m_enabled) {
        g_guildInfoCache.Clear();
    }

    if (!g_questDBCache.m_enabled) {
        g_questDBCache.Clear();
    }

    if (!g_pageTextCache.m_enabled) {
        g_pageTextCache.Clear();
    }

    if (!g_petNameCache.m_enabled) {
        g_petNameCache.Clear();
    }

    if (!g_petitionCache.m_enabled) {
        g_petitionCache.Clear();
    }

    if (!g_itemTextDBCache.m_enabled) {
        g_itemTextDBCache.Clear();
    }

    if (!g_wardenCache.m_enabled) {
        g_wardenCache.Clear();
    }

    if (!g_arenaTeamCache.m_enabled) {
        g_arenaTeamCache.Clear();
    }

    if (!g_danceCache.m_enabled) {
        g_danceCache.Clear();
    }
}

// OFFSET: #0x0066FFE0
template <>
DBCache<NameCache, uint64_t, CHashKeyGUID>::REVERSEENTRY* DBCache<NameCache, uint64_t, CHashKeyGUID>::FindReverseEntry(const char* name) {
    if (this->m_reverse.m_slotmask == 0xFFFFFFFF) {
        return NULL;
    }

    uint32_t hashval = SStrHashHT(name);
    TSExplicitList<REVERSEENTRY>& list = this->m_reverse.m_slotlistarray[hashval & this->m_reverse.m_slotmask];

    for (REVERSEENTRY* entry = list.Head(); entry; entry = list.Next(entry)) {
        if (entry->m_hashval == hashval && (entry->m_key.m_str == name || !SStrCmpUTF8I(entry->m_key.m_str, name, STORM_MAX_STR))) {
            return entry;
        }
    }

    return NULL;
}

// OFFSET: #0x006701F0
template <>
NameCache* DBCache<NameCache, uint64_t, CHashKeyGUID>::GetRecordByName(const char* name) {
    REVERSEENTRY* entry = this->FindReverseEntry(name);
    return entry ? entry->m_record : NULL;
}

// INLINED
template <>
DBCache<PetNameCache, int32_t, HASHKEY_INT>::REVERSEENTRY* DBCache<PetNameCache, int32_t, HASHKEY_INT>::FindReverseEntry(const char* name) {
    if (this->m_reverse.m_slotmask == 0xFFFFFFFF) {
        return NULL;
    }

    uint32_t hashval = SStrHashHT(name);
    TSExplicitList<REVERSEENTRY>& list = this->m_reverse.m_slotlistarray[hashval & this->m_reverse.m_slotmask];

    for (REVERSEENTRY* entry = list.Head(); entry; entry = list.Next(entry)) {
        if (entry->m_hashval == hashval && (entry->m_key.m_str == name || !SStrCmpUTF8I(entry->m_key.m_str, name, STORM_MAX_STR))) {
            return entry;
        }
    }

    return NULL;
}

// OFFSET: #0x006701F0
template <>
PetNameCache* DBCache<PetNameCache, int32_t, HASHKEY_INT>::GetRecordByName(const char* name) {
    REVERSEENTRY* entry = this->FindReverseEntry(name);
    return entry ? entry->m_record : NULL;
}

// OFFSET: #0x00678F20
template <>
void DBCache<NameCache, uint64_t, CHashKeyGUID>::RemoveReverseEntry(const char* name) {
    REVERSEENTRY* entry = this->FindReverseEntry(name);

    if (!entry) {
        SErrSetLastError(87);
        return;
    }

    this->m_reverse.Delete(entry);
}

// OFFSET: #0x00680C20
template <>
void DBCache<NameCache, uint64_t, CHashKeyGUID>::ItemAdded(NameCache* item) {
    if (this->FindReverseEntry(item->m_name)) {
        return;
    }

    uint32_t hashval = SStrHashHT(item->m_name);
    REVERSEENTRY* entry = this->m_reverse.InternalNewNode(hashval, 0, 0);
    entry->m_hashval = hashval;
    entry->m_key.m_str = item->m_name;
    entry->m_record = item;
}

// OFFSET: #0x0067A430
template <>
void DBCache<NameCache, uint64_t, CHashKeyGUID>::Invalidate(uint64_t key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    if (!hash->m_loaded) {
        this->DenyItem(key);
    } else if (hash->m_dispatching) {
        hash->m_invalidatePending = true;
    } else {
        this->RemoveReverseEntry(hash->m_record.m_name);
        this->m_entries.Delete(hash);
    }
}

// OFFSET: #0x00680C60
template <>
void DBCache<NameCache, uint64_t, CHashKeyGUID>::AddItem(NameCache* item, uint64_t key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
    }

    hash->m_dispatching = true;
    hash->m_record.Copy(*item);
    hash->m_loaded = true;
    hash->m_id = key;
    this->ItemAdded(&hash->m_record);

    for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node;) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);
        DBCacheDispatch(node, key, static_cast<int32_t>(key), true);
        node = next;
    }

    hash->m_callbacks.Clear();
    bool invalidate = hash->m_invalidatePending;
    hash->m_dispatching = false;

    if (invalidate) {
        this->Invalidate(key);
    }
}

// OFFSET: 0x00496B60
void DBCache_Idle() {
    g_creatureDBCache.Idle();
    g_gameObjectDBCache.Idle();
    g_npcTextDBCache.Idle();
    g_nameDBCache.Idle();
    g_guildInfoCache.Idle();
    g_questDBCache.Idle();
    g_pageTextCache.Idle();
    g_petNameCache.Idle();
    g_petitionCache.Idle();
    g_itemTextDBCache.Idle();
    g_wardenCache.Idle();
    g_arenaTeamCache.Idle();
}

// OFFSET: 0x00496FD0
void DBCache_ClearPending() {
    g_creatureDBCache.ClearPending();
    g_gameObjectDBCache.ClearPending();
    g_npcTextDBCache.ClearPending();
    g_nameDBCache.ClearPending();
    g_guildInfoCache.ClearPending();
    g_questDBCache.ClearPending();
    g_pageTextCache.ClearPending();
    g_petNameCache.ClearPending();
    g_petitionCache.ClearPending();
    g_itemTextDBCache.ClearPending();
    g_arenaTeamCache.ClearPending();
    g_danceCache.ClearPending();
}
