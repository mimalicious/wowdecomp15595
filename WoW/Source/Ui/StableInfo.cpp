#include "StableInfo.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Services/SysMessage.h"

// STATIC: 0x00F484B8
StablePetInfo CGStableInfo::m_pets[25];

// STATIC: 0x00F48E18
WOWGUID CGStableInfo::m_stableMaster;

// STATIC: 0x00F48E20
uint32_t CGStableInfo::m_numPets;

// STATIC: 0x00F48E24
uint8_t CGStableInfo::m_slotChangePending;

// OFFSET: 0x00464740
StablePetInfo* CGStableInfo::GetPetInfo(uint32_t index) {
    if (index >= 25) {
        return NULL;
    }

    return &CGStableInfo::m_pets[index];
}

// OFFSET: 0x0096C880/0x1406E3630
void CGStableInfo::EnterWorld() {
    CGStableInfo::m_stableMaster = WOWGUID::Make(0);
}

// OFFSET: 0x0096C890/0x1406E3640
void CGStableInfo::CloseStables(bool force) {
    if (!CGStableInfo::m_stableMaster && !force) {
        return;
    }

    CGGameUI::ClearInteractTarget(CGStableInfo::m_stableMaster, 0);
    CGStableInfo::m_stableMaster = WOWGUID::Make(0);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_CLOSED, NULL);
}

// OFFSET: 0x0096C8E0
int32_t CGStableInfo::GetNumActivePets() {
    int32_t count = 0;

    for (uint32_t i = 0; i < 25; i++) {
        if (CGStableInfo::m_pets[i].m_petNumber && !(CGStableInfo::m_pets[i].m_flags & 0x2)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x0096C950
void CGStableInfo::SetPetSlot(uint32_t index, uint32_t slot) {
    if (index > 24 || slot > 24 || index == slot || CGStableInfo::m_slotChangePending) {
        return;
    }

    StablePetInfo* pet = CGStableInfo::GetPetInfo(index);

    if (!pet || !pet->m_petNumber) {
        return;
    }

    PlayerSetPetSlot msg;
    msg.m_petNumber = pet->m_petNumber;
    msg.m_destSlot = slot;
    *reinterpret_cast<WOWGUID*>(msg.m_stableMaster) = CGStableInfo::m_stableMaster;
    ClientServices::Send(&msg);
    CGStableInfo::m_slotChangePending = 1;
}

// OFFSET: 0x0096C9D0/0x1406E3740
int32_t CGStableInfo::FindPetIndexBySpellID(int32_t spellId, int32_t effectIndex) {
    const SpellEffectRec* effect = SpellGetEffect(spellId, effectIndex);

    if (!effect || effect->m_effect != 56 || effect->m_effectMiscValue) {
        return -1;
    }

    int32_t index = effect->m_effectBasePoints;

    if (index >= 5) {
        return -1;
    }

    return index;
}

// OFFSET: 0x0096CB80/0x1406E3920
void CGStableInfo::LeaveWorld() {
    CGStableInfo::CloseStables(false);
}

// OFFSET: 0x0096CBC0
void CGStableInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(MSG_LIST_STABLED_PETS);
    ClientServices::ClearMessageHandler(SMSG_STABLE_RESULT);
    ClientPetSlotUpdated::s_handler = NULL;
    ClientPetAdded::s_handler = NULL;
    memset(CGStableInfo::m_pets, 0, sizeof(CGStableInfo::m_pets));
}

// OFFSET: 0x0096CC10
int32_t CGStableInfo::OnStableList(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGossipInfo::ClearGossip();
    WOWGUID master;
    msg->Get(master.m_guid);
    WOWGUID npc = master;

    if (CGStableInfo::m_stableMaster != npc) {
        CGGameUI::SetInteractTarget(npc);
        CGStableInfo::m_stableMaster = npc;
    }

    memset(CGStableInfo::m_pets, 0, sizeof(CGStableInfo::m_pets));
    uint8_t count;
    uint8_t numSlots;
    msg->Get(count);
    msg->Get(numSlots);

    if (count >= 25) {
        count = 25;
    }

    for (uint32_t i = 0; i < count; i++) {
        uint32_t slot;
        msg->Get(slot);

        if (slot <= 24 && !CGStableInfo::m_pets[slot].m_petNumber) {
            StablePetInfo& pet = CGStableInfo::m_pets[slot];
            msg->Get(pet.m_petNumber);
            msg->Get(reinterpret_cast<uint32_t&>(pet.m_entry));
            msg->Get(pet.m_level);
            msg->GetString(pet.m_name, sizeof(pet.m_name));
            msg->Get(pet.m_flags);
            CGStableInfo::m_numPets++;
        } else {
            uint32_t petNumber;
            uint32_t creatureId;
            uint32_t level;
            char name[80];
            uint8_t flags;
            msg->Get(petNumber);
            msg->Get(creatureId);
            msg->Get(level);
            msg->GetString(name, sizeof(name));
            msg->Get(flags);
            SysMsgPrintf(STATUS_ERROR, 4, "BAD STABLE DATA! petnumber: %d, petSlot: %d, creatureID:%d, m_petList[petSlot].petNumber: %d", petNumber, slot, creatureId, slot <= 24 ? CGStableInfo::m_pets[slot].m_petNumber : -1);
        }
    }

    if (master) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_SHOW, NULL);
    }

    return 1;
}

// OFFSET: 0x0096CDE0/0x1406E3C30
void CGStableInfo::UpdatePetName(uint32_t petNumber, const char* name) {
    if (!petNumber) {
        return;
    }

    for (uint32_t i = 0; i < 25; i++) {
        if (CGStableInfo::m_pets[i].m_petNumber == petNumber) {
            SStrCopy(CGStableInfo::m_pets[i].m_name, name, sizeof(CGStableInfo::m_pets[i].m_name));
        }
    }
}

// OFFSET: 0x0096CE40
bool CGStableInfo::OnPetSlotUpdated(ClientPetSlotUpdated* msg) {
    CGStableInfo::m_slotChangePending = 0;
    StablePetInfo* found = NULL;

    for (uint32_t i = 0; i < 25; i++) {
        if (CGStableInfo::m_pets[i].m_petNumber == msg->m_petNumber) {
            found = &CGStableInfo::m_pets[i];
            break;
        }
    }

    int32_t newSlot = msg->m_newSlot;
    int32_t prevSlot = msg->m_prevSlot;

    if (newSlot < 0) {
        if (found) {
            memset(found, 0, sizeof(StablePetInfo));
            CGStableInfo::m_numPets--;
        }
    } else {
        StablePetInfo swap = CGStableInfo::m_pets[newSlot];

        if (found) {
            CGStableInfo::m_pets[newSlot] = *found;
        }

        CGStableInfo::m_pets[prevSlot] = swap;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_UPDATE, NULL);
    return true;
}

// OFFSET: 0x0096CF00
bool CGStableInfo::OnPetAdded(ClientPetAdded* msg) {
    int32_t slot = msg->m_slot;

    if (slot >= 0 && static_cast<uint32_t>(slot) < 25) {
        StablePetInfo& pet = CGStableInfo::m_pets[slot];
        pet.m_petNumber = msg->m_petNumber;
        pet.m_entry = msg->m_creatureID;
        pet.m_level = msg->m_level;
        pet.m_flags = msg->m_flags;
        SStrCopy(pet.m_name, msg->m_name, sizeof(pet.m_name));
        CGStableInfo::m_numPets++;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_UPDATE, NULL);
    return true;
}

// OFFSET: 0x0096D230
int32_t CGStableInfo::OnStableCommandResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t result;
    msg->Get(result);

    switch (result) {
        case 1:
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            CGStableInfo::m_slotChangePending = 0;
            return 1;

        case 8:
        case 9: {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (CGStableInfo::m_stableMaster || (player && player->HasAuraEffect(292))) {
                CDataStore list;
                list.Put(static_cast<uint32_t>(MSG_LIST_STABLED_PETS));
                list.Put(CGStableInfo::m_stableMaster.m_guid);
                list.Finalize();
                ClientServices::Send2(&list);
                CGStableInfo::m_slotChangePending = 0;
                return 1;
            }

            break;
        }

        case 10:
            FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_UPDATE, NULL);
            CGStableInfo::m_slotChangePending = 0;
            return 1;

        case 3:
            Spell_C_PetTameFailure(13);
            break;

        case 11:
            Spell_C_PetTameFailure(12);
            break;

        case 12:
            Spell_C_PetTameFailure(8);
            break;
    }

    CGStableInfo::m_slotChangePending = 0;
    return 1;
}

// OFFSET: 0x0096D540/0x1406E4580
void CGStableInfo::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_LIST_STABLED_PETS, CGStableInfo::OnStableList, NULL);
    ClientServices::SetMessageHandler(SMSG_STABLE_RESULT, CGStableInfo::OnStableCommandResult, NULL);
    ClientPetSlotUpdated::s_handler = CGStableInfo::OnPetSlotUpdated;
    ClientPetAdded::s_handler = CGStableInfo::OnPetAdded;
}
