#include "PetitionInfo.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Object/CGPetition.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00F71078
WOWGUID CGPetitionInfo::m_petitionGUID;

// STATIC: 0x00F71080
WOWGUID CGPetitionInfo::m_petitionItem;

// STATIC: 0x00F71088
uint32_t CGPetitionInfo::m_petitionId;

// STATIC: 0x00F7108C
int32_t CGPetitionInfo::m_signed;

// STATIC: 0x00F71090
uint32_t CGPetitionInfo::m_numPetitionNames;

// STATIC: 0x00F71094
int32_t CGPetitionInfo::m_pendingNameQueries;

// STATIC: 0x00F71098
CGPetition* CGPetitionInfo::m_petition;

// STATIC: 0x00F7109C
TSGrowableArray<PetitionSignerInfo> CGPetitionInfo::m_signers;

// OFFSET: 0x009E0100
int32_t Packet_SMSG_OFFER_PETITION_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        CGGameUI::DisplayError(ERR_ALREADY_IN_ARENA_TEAM_S, name->m_name);
    }

    return 1;
}

// OFFSET: 0x009E0160
void CGPetitionInfo::SetPetitionStats(int32_t id) {
    if (CGPetitionInfo::m_petitionId != id) {
        return;
    }

    WOWGUID none = WOWGUID();
    CGPetitionInfo::m_petition = g_petitionCache.GetRecord(id, none, NULL, NULL, false);

    if (!CGPetitionInfo::m_pendingNameQueries && CGPetitionInfo::m_petition) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_SHOW, NULL);
    }
}

// OFFSET: 0x009E0300
void SignatureNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (CGPetitionInfo::m_pendingNameQueries && !--CGPetitionInfo::m_pendingNameQueries && CGPetitionInfo::m_petition) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_SHOW, NULL);
    }
}

// PROVEN: 3b87c374 IDENTICAL/NONE
// OFFSET: 0x009E0330
void PetitionQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGPetitionInfo::SetPetitionStats(id);
    }
}

// OFFSET: 0x009E0350/0x140776F90
void CGPetitionInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_OFFER_PETITION_ERROR, Packet_SMSG_OFFER_PETITION_ERROR, NULL);
}

// OFFSET: 0x009E0390
void CGPetitionInfo::ClearSignatures() {
    if (CGPetitionInfo::m_pendingNameQueries) {
        for (uint32_t i = 0; i < CGPetitionInfo::m_numPetitionNames; i++) {
            g_nameDBCache.CancelCallback(CGPetitionInfo::m_signers.m_data[i].m_guid.m_guid, SignatureNameQueryCallback, NULL);
        }
    }

    CGPetitionInfo::m_numPetitionNames = 0;
    CGPetitionInfo::m_pendingNameQueries = 0;
}

// OFFSET: 0x009E0620/0x140777290
void CGPetitionInfo::SetPetition(WOWGUID petition, int32_t id, WOWGUID item) {
    if (CGPetitionInfo::m_petitionGUID) {
        if (!CGPetitionInfo::m_signed && CGPetitionInfo::m_petition && CGPetitionInfo::m_petition->m_ownerGuid != ClntObjMgrGetActivePlayer()) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(MSG_PETITION_DECLINE));
            msg.Put(CGPetitionInfo::m_petitionGUID.m_guid);
            msg.Finalize();
            ClientServices::Send2(&msg);
        }

        CGPetitionInfo::ClearSignatures();
        CGPetitionInfo::m_petitionGUID = WOWGUID();
        CGPetitionInfo::m_petitionItem = WOWGUID();
        FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_CLOSED, NULL);
    }

    CGPetitionInfo::m_petitionItem = item;
    CGPetitionInfo::m_petitionGUID = petition;
    CGPetitionInfo::m_petitionId = id;

    if (petition) {
        CGPetitionInfo::m_petition = g_petitionCache.GetRecord(id, petition, PetitionQueryCallback, NULL, false);
        CGPetitionInfo::m_signed = 0;
    } else {
        CGPetitionInfo::m_petition = NULL;
        CGPetitionInfo::m_signed = 0;
    }
}

// OFFSET: 0x009E0CB0/0x140777A60
void CGPetitionInfo::EnterWorld() {
    CGPetitionInfo::m_petitionGUID = WOWGUID::Make(0);
    CGPetitionInfo::m_petitionItem = WOWGUID::Make(0);
    CGPetitionInfo::m_petitionId = 0;
    CGPetitionInfo::m_numPetitionNames = 0;
    CGPetitionInfo::m_pendingNameQueries = 0;
    CGPetitionInfo::m_petition = NULL;
    CGPetitionInfo::m_signers.SetCount(10);
}

// OFFSET: 0x009E0370
void CGPetitionInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_OFFER_PETITION_ERROR);
}

// OFFSET: 0x009E0D90
void CGPetitionInfo::SetSignatures(uint8_t count, const WOWGUID* guids, const uint32_t* choices) {
    CGPetitionInfo::ClearSignatures();

    if (CGPetitionInfo::m_signers.m_count < count) {
        CGPetitionInfo::m_signers.SetCount(count);
    }

    CGPetitionInfo::m_numPetitionNames = count;

    for (uint32_t i = 0; i < count; i++) {
        CGPetitionInfo::m_signers.m_data[i].m_guid = guids[i];
        CGPetitionInfo::m_signers.m_data[i].unk_0008 = choices[i];

        if (!g_nameDBCache.GetRecord(guids[i].m_guid, guids[i], SignatureNameQueryCallback, NULL, false)) {
            CGPetitionInfo::m_pendingNameQueries++;
        }
    }

    if (!CGPetitionInfo::m_pendingNameQueries && CGPetitionInfo::m_petition) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_SHOW, NULL);
    }
}

// OFFSET: 0x009E0E50
void CGPetitionInfo::AddSignature(WOWGUID petition, WOWGUID signer) {
    uint32_t count = CGPetitionInfo::m_numPetitionNames;

    for (uint32_t i = 0; i < count; i++) {
        if (CGPetitionInfo::m_signers.m_data[i].m_guid == signer) {
            return;
        }
    }

    if (CGPetitionInfo::m_numPetitionNames == CGPetitionInfo::m_signers.Count()) {
        CGPetitionInfo::m_signers.SetCount(CGPetitionInfo::m_signers.Count() + 1);
    }

    CGPetitionInfo::m_signers.m_data[CGPetitionInfo::m_numPetitionNames].m_guid = signer;
    CGPetitionInfo::m_numPetitionNames++;
    NameCache* name = g_nameDBCache.GetRecord(signer.m_guid, signer, SignatureNameQueryCallback, NULL, false);

    if (!name) {
        CGPetitionInfo::m_pendingNameQueries++;
    } else {
        CGGameUI::DisplayError(ERR_PETITION_SIGNED_S, name->m_name);
    }

    if (!CGPetitionInfo::m_pendingNameQueries && CGPetitionInfo::m_petition) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PETITION_SHOW, NULL);
    }
}

// OFFSET: 0x009E0D40/0x140777B00
void CGPetitionInfo::LeaveWorld() {
    CGPetitionInfo::m_signers.Clear();
}
