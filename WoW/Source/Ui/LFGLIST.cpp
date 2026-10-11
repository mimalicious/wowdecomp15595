#include "LFGLIST.h"
#include <typeinfo>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"

// PROVEN: 004b53c3 IDENTICAL/NONE
// OFFSET: 0x0097FA10
void LFGLIST::Reindex() {
    for (uint32_t i = 0; i < this->m_playerList.Count(); i++) {
        this->m_playerList.m_data[i]->m_index = i;
    }

    for (uint32_t i = 0; i < this->m_partyList.Count(); i++) {
        this->m_partyList.m_data[i]->m_index = i;
    }
}

// OFFSET: 0x009842C0
void LFGLIST::AddPlayer(LFGListPlayer* player) {
    if (player->m_party) {
        player->m_party->AddPlayer(player);
    } else if (player->m_index < 0) {
        uint32_t count = this->m_playerList.Count();
        this->m_playerList.SetCount(count + 1);
        this->m_playerList.m_data[count] = player;
        player->m_index = count;
    }
}

// OFFSET: #0x00556F90
void LFGLIST::RemovePlayer(LFGListPlayer* player) {
    int32_t index = player->m_index;

    if (index >= 0) {
        uint32_t count = this->m_playerList.Count();

        for (; static_cast<uint32_t>(index) < count - 1; index++) {
            this->m_playerList.m_data[index] = this->m_playerList.m_data[index + 1];
            this->m_playerList.m_data[index]->m_index = index;
        }

        this->m_playerList.SetCount(count - 1);
        player->m_index = -1;
    }

    if (player->m_party) {
        player->m_party->RemovePlayer(player);
    }
}

// OFFSET: #0x00557030
void LFGLIST::RemoveParty(LFGListParty* party) {
    int32_t index = party->m_index;

    if (index < 0) {
        return;
    }

    uint32_t count = this->m_partyList.Count();

    for (; static_cast<uint32_t>(index) < count - 1; index++) {
        this->m_partyList.m_data[index] = this->m_partyList.m_data[index + 1];
        this->m_partyList.m_data[index]->m_index = index;
    }

    this->m_partyList.SetCount(count - 1);
    party->m_index = -1;
}

// OFFSET: #0x005589D0
void LFGLIST::DeleteParty(LFGListParty* party) {
    for (int32_t i = party->m_members.Count() - 1; i >= 0; i--) {
        LFGListPlayer* player = party->m_members.m_data[i];
        this->RemovePlayer(player);
        delete player;
    }

    this->RemoveParty(party);
    delete party;
}

// OFFSET: #0x00558AF0
void LFGLIST::Clear() {
    LFGListPlayer* player = this->m_players.m_fulllist.Head();

    while (player) {
        LFGListPlayer* next = this->m_players.m_fulllist.Next(player);
        g_nameDBCache.CancelCallback(player->m_key.m_guid.m_guid, LFGNameCacheCallback, NULL);
        delete player;
        player = next;
    }

    this->m_playerList.m_count = 0;
    this->m_players.Clear();

    LFGListParty* party = this->m_parties.m_fulllist.Head();

    while (party) {
        LFGListParty* next = this->m_parties.m_fulllist.Next(party);
        delete party;
        party = next;
    }

    this->m_partyList.m_count = 0;
    this->m_lfgID = 0;
    this->m_time = 0;
    this->m_totalPlayers = 0;
    this->m_totalParties = 0;
    this->unk_0010 = 0;
    this->m_pendingNames = 0;
}

// OFFSET: #0x0055AA40
LFGLIST::~LFGLIST() {
    for (int32_t i = this->m_partyList.Count() - 1; i >= 0; i--) {
        this->DeleteParty(this->m_partyList.m_data[i]);
    }

    for (int32_t i = this->m_playerList.Count() - 1; i >= 0; i--) {
        LFGListPlayer* player = this->m_playerList.m_data[i];
        this->RemovePlayer(player);
        delete player;
    }
}

// OFFSET: 0x0098A470
bool LFGLIST::Copy(LFGLIST& source) {
    this->Clear();
    this->m_lfgID = source.m_lfgID;
    this->m_time = source.m_time;
    this->m_totalPlayers = source.m_totalPlayers;
    this->m_totalParties = source.m_totalParties;
    this->unk_0010 = source.unk_0010;
    this->m_pendingNames = source.m_pendingNames;

    for (LFGListPlayer* player = source.m_players.m_fulllist.Head(); player; player = source.m_players.m_fulllist.Next(player)) {
        uint64_t guid = player->m_key.m_guid.m_guid;
        LFGListPlayer* copy = new (__FILE__, __LINE__) LFGListPlayer(*player);
        copy->m_index = -1;
        copy->m_partyIndex = -1;
        copy->m_party = NULL;
        this->m_players.Insert(copy, static_cast<uint32_t>(guid), CHashKeyGUID(guid));
    }

    for (uint32_t i = 0; i < source.m_playerList.Count(); i++) {
        uint64_t guid = source.m_playerList.m_data[i]->m_key.m_guid.m_guid;
        LFGListPlayer* player = this->m_players.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

        if (player->m_party) {
            player->m_party->AddPlayer(player);
        } else if (player->m_index < 0) {
            uint32_t count = this->m_playerList.Count();
            this->m_playerList.SetCount(count + 1);
            this->m_playerList.m_data[count] = player;
            player->m_index = count;
        }
    }

    for (LFGListParty* party = source.m_parties.m_fulllist.Head(); party; party = source.m_parties.m_fulllist.Next(party)) {
        uint64_t guid = party->m_key.m_guid.m_guid;
        LFGListParty* copy = new (__FILE__, __LINE__) LFGListParty(*party);
        copy->m_index = -1;

        for (uint32_t j = 0; j < party->m_members.Count(); j++) {
            uint64_t memberGuid = party->m_members.m_data[j]->m_key.m_guid.m_guid;
            LFGListPlayer* player = this->m_players.Ptr(static_cast<uint32_t>(memberGuid), CHashKeyGUID(memberGuid));
            player->m_party = copy;
            copy->AddPlayer(player);
        }

        this->m_parties.Insert(copy, static_cast<uint32_t>(guid), CHashKeyGUID(guid));
    }

    for (uint32_t i = 0; i < source.m_partyList.Count(); i++) {
        uint64_t guid = source.m_partyList.m_data[i]->m_key.m_guid.m_guid;
        LFGListParty* party = this->m_parties.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

        if (party->m_index < 0) {
            uint32_t count = this->m_partyList.Count();
            this->m_partyList.SetCount(count + 1);
            this->m_partyList.m_data[count] = party;
            party->m_index = count;
        }
    }

    for (uint32_t i = 0; i < this->m_playerList.Count(); i++) {
        this->m_playerList.m_data[i]->m_index = i;
    }

    for (uint32_t i = 0; i < this->m_partyList.Count(); i++) {
        this->m_partyList.m_data[i]->m_index = i;
    }

    return true;
}
