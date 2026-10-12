#include "UIMacroOptions.h"
#include <cstring>
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MacroConditions.h"
#include "WoW/Source/Ui/MacroSubstring.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/UIMacros.h"

void InitializeMacroOptions() {
}

// OFFSET: 0x00A0D3E0/0x1407AF000
int32_t EvaluateExists(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGObject_C* object = ClntObjMgrObjectPtr(*unit, TYPEMASK_OBJECT, __FILE__, __LINE__);
    int32_t exists = (object && object->CanBeTargetted()) || CGGameUI::IsRaidMemberOrPet(*unit);

    if (!args->m_count) {
        return exists;
    }

    return exists == args->m_data->ToNumber();
}

// OFFSET: 0x00A0D450/0x1407AF0C0
int32_t EvaluateParty(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    int32_t party = CGPartyInfo::IsPlayerOrPetInParty(*unit);

    if (!args->m_count) {
        return party;
    }

    return party == args->m_data->ToNumber();
}

// OFFSET: 0x00A0D490/0x1407AF120
int32_t EvaluateRaid(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    int32_t raid = CGPartyInfo::IsPlayerOrPetInParty(*unit) || CGRaidInfo::IsPlayerOrPetInRaid(*unit);

    if (!args->m_count) {
        return raid;
    }

    return raid == args->m_data->ToNumber();
}

// OFFSET: 0x00A0DF90/0x1407AFFF0
int32_t EvaluateFlyable(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(*unit, TYPEMASK_UNIT, __FILE__, __LINE__));
    bool flyable = player ? player->IsInFlyableZone(1) : IsInFlyableArea() != 0;

    if (!args->m_count) {
        return flyable;
    }

    return flyable == args->m_data->ToNumber();
}

// OFFSET: 0x00A0D4E0/0x1407AF1C0
int32_t EvaluateGroup(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    if (!args->m_count) {
        return CGPartyInfo::NumMembers() || CGRaidInfo::m_numMembers;
    }

    for (uint32_t i = args->m_count; i; i--) {
        MacroSubstring& arg = args->m_data[i - 1];

        if (arg == "party") {
            return CGPartyInfo::NumMembers() != 0;
        }

        if (arg == "raid") {
            return CGRaidInfo::m_numMembers != 0;
        }
    }

    return 0;
}

// OFFSET: 0x00A0D6E0/0x1407AF470
int32_t EvaluateButton(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    const char* button = CSimpleTop::s_instance->m_mouseButtonName;
    const char* macroButton = CGUIMacros::m_macroRunning;

    if (!button && !macroButton) {
        button = "LeftButton";
    }

    if (!args->m_count) {
        return 0;
    }

    for (uint32_t i = args->m_count; i; i--) {
        const MacroSubstring& arg = args->m_data[i - 1];
        char name[1024];

        if (*arg.m_start >= '0' && *arg.m_start <= '9') {
            uint32_t index = 0;

            for (const char* c = arg.m_start; c < arg.m_end && *c >= '0' && *c <= '9'; c++) {
                index = *c + 10 * index - '0';
            }

            SStrCopy(name, GetMouseButtonName(IndexToMouseButton(index)), sizeof(name));
        } else {
            uint32_t length = arg.m_end - arg.m_start;

            if (length > 1023) {
                length = 1023;
            }

            memcpy(name, arg.m_start, length);
            name[length] = '\0';
        }

        if ((button && !SStrCmpUTF8I(name, button, 0x7FFFFFFF)) || (macroButton && !SStrCmpUTF8I(name, macroButton, 0x7FFFFFFF))) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A0D820/0x1407AF610
int32_t EvaluateActionBar(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    if (!args->m_count) {
        return 0;
    }

    for (uint32_t i = args->m_count; i; i--) {
        const MacroSubstring& arg = args->m_data[i - 1];
        uint32_t page = 0;

        for (const char* c = arg.m_start; c < arg.m_end && *c >= '0' && *c <= '9'; c++) {
            page = *c + 10 * page - '0';
        }

        if (page == CGActionBar::m_currentPage + 1) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A0D890/0x1407AF680
int32_t EvaluateBonusBar(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    if (!args->m_count) {
        return 0;
    }

    for (uint32_t i = args->m_count; i; i--) {
        const MacroSubstring& arg = args->m_data[i - 1];
        int32_t offset = 0;

        for (const char* c = arg.m_start; c < arg.m_end && *c >= '0' && *c <= '9'; c++) {
            offset = *c + 10 * offset - '0';
        }

        if (offset == CGActionBar::m_bonusBarOffset) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A0D8F0/0x1407AF6F0
int32_t EvaluateSpec(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    uint32_t group = CGTalentInfo::GetActiveTalentGroup(0) + 1;

    if (args->m_count <= 0) {
        return 0;
    }

    for (uint32_t i = args->m_count; i; i--) {
        const MacroSubstring& arg = args->m_data[i - 1];
        uint32_t spec = 0;

        for (const char* c = arg.m_start; c < arg.m_end && *c >= '0' && *c <= '9'; c++) {
            spec = *c + 10 * spec - '0';
        }

        if (spec == group) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A0D970/0x1407AF770
static VehicleSeatRec* GetVehicleSeatRec(const WOWGUID* guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!unit->HasVehicleTransport()) {
            return NULL;
        }

        WOWGUID transportGuid;
        transportGuid.m_guid = unit->GetTransportGUID();

        if (!transportGuid) {
            return NULL;
        }

        CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transportGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!transport || !transport->m_vehicle) {
            return NULL;
        }

        return transport->m_vehicle->GetVehicleSeatRec(unit->m_movementData.m_transportSeat);
    }

    PartyMemberState* state = CGGameUI::GetPartyMemberState(*guid);

    if (state && state->m_vehicleSeatId > 0) {
        return g_vehicleSeatDB.GetRecord(state->m_vehicleSeatId);
    }

    return NULL;
}

// OFFSET: 0x00A0DA30/0x1407AF860
int32_t EvaluateVehicleUI(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    WOWGUID guid = ClntObjMgrGetActivePlayer();
    VehicleSeatRec* seat = GetVehicleSeatRec(&guid);

    if (seat) {
        return seat->m_flags & 0x20000000;
    }

    return 0;
}

// OFFSET: 0x00A0DA70/0x1407AF8A0
int32_t EvaluateUnitHasVehicleUI(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    VehicleSeatRec* seat = GetVehicleSeatRec(unit);

    if (seat) {
        return seat->m_flags & 0x20000000;
    }

    return 0;
}

// OFFSET: 0x00A0DA90/0x1407AF8D0
int32_t EvaluateCursor(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    if (!CSimpleTop::s_instance->m_allowProtectedActions) {
        return 0;
    }

    const char* type;

    switch (CGGameUI::m_cursorItemType) {
        case 1:
        case 7:
        case 9:
        case 11:
            type = "item";
            break;

        case 2:
            type = "money";
            break;

        case 3:
            type = "spell";
            break;

        case 5:
            type = "merchant";
            break;

        case 8:
            type = "macro";
            break;

        case 12:
            type = "guildbankmoney";
            break;

        case 13:
            type = "equipmentset";
            break;

        default:
            return 0;
    }

    if (args->m_count) {
        for (uint32_t i = args->m_count; i; i--) {
            const MacroSubstring& arg = args->m_data[i - 1];
            char text[1024];
            uint32_t length = arg.m_end - arg.m_start;

            if (length > 1023) {
                length = 1023;
            }

            memcpy(text, arg.m_start, length);
            text[length] = '\0';

            if (!SStrCmpI(text, type, 0x7FFFFFFF)) {
                return 1;
            }
        }

        return 0;
    }

    return 1;
}

// OFFSET: 0x00A0DD20/0x1407AFC20
int32_t EvaluateCombat(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t combat;

    if (player && (player->m_unit->m_unitFieldFlags & 0x80000)) {
        combat = 1;
    } else {
        CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
        combat = pet && (pet->m_unit->m_unitFieldFlags & 0x80000);
    }

    if (!args->m_count) {
        return combat;
    }

    return combat == args->m_data->ToNumber();
}

// OFFSET: 0x00A0DDC0/0x1407AFD10
int32_t EvaluateHelp(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*unit, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t help = player && target && player->CanAssist(target, false);

    if (!args->m_count) {
        return help;
    }

    return help == args->m_data->ToNumber();
}

// OFFSET: 0x00A0DE40/0x1407AFDF0
int32_t EvaluateHarm(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*unit, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t harm = player && target && player->CanAttack(target);

    if (!args->m_count) {
        return harm;
    }

    return harm == args->m_data->ToNumber();
}

// OFFSET: 0x00A0DEC0/0x1407AFED0
int32_t EvaluateDead(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGUnit_C* object = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*unit, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t dead;

    if (object) {
        if (object->m_unit->m_unitFieldHealth <= 0) {
            dead = 1;
        } else if ((object->m_unit->m_unitDynamicFlags & 0x20) && !CGGameUI::IsRaidMemberOrPet(object->m_obj->m_guid)) {
            dead = 1;
        } else if ((object->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(object)->m_player->m_playerFlags & 0x10)) {
            dead = 1;
        } else {
            dead = 0;
        }
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(*unit);
        dead = state ? (state->m_flags & 0xC) : 0;
    }

    if (!args->m_count) {
        return dead;
    }

    return dead == args->m_data->ToNumber();
}

// OFFSET: 0x00A0E000/0x1407B0090
int32_t EvaluateIndoors(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t indoors = player && !player->IsOutdoors();

    if (!args->m_count) {
        return indoors;
    }

    return indoors == args->m_data->ToNumber();
}

// OFFSET: 0x00A0E060/0x1407B0140
int32_t EvaluateOutdoors(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t outdoors = player && player->IsOutdoors();

    if (!args->m_count) {
        return outdoors;
    }

    return outdoors == args->m_data->ToNumber();
}

// OFFSET: 0x00A0E0C0/0x1407B01F0
int32_t EvaluateSwimming(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t swimming = player && (player->m_clientFlags & 0x200000);

    if (args->m_count) {
        return swimming == args->m_data->ToNumber();
    }

    return swimming;
}

// OFFSET: 0x00A0E120/0x1407B0290
int32_t EvaluateFlying(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t flying = player && (player->m_movement->m_flags & 0x1000000);

    if (args->m_count) {
        return flying == args->m_data->ToNumber();
    }

    return flying;
}

// OFFSET: 0x00A0E180/0x1407B0330
int32_t EvaluateMounted(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t mounted = player && player->m_mountDisplayId > 0 && !(player->m_clientFlags & 0x10000000);

    if (!args->m_count) {
        return mounted;
    }

    return mounted == args->m_data->ToNumber();
}

// OFFSET: 0x00A0E1F0/0x1407B03D0
int32_t EvaluateStealth(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t stealth = player && (player->m_unit->m_unitFieldBytes1[2] & 0x2);

    if (!args->m_count) {
        return stealth;
    }

    return stealth == args->m_data->ToNumber();
}

// OFFSET: 0x00A0E250/0x1407B0470
int32_t EvaluatePet(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    const WOWGUID* petGuid = player->m_unit->m_unitFieldCharm.m_guid ? &player->m_unit->m_unitFieldCharm : &player->m_unit->m_unitFieldSummon;
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*petGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return 0;
    }

    if (args->m_count > 0) {
        char name[1024] = "";
        char family[1024] = "";
        SStrCopy(name, pet->GetUnitName(NULL, 1), sizeof(name));
        uint32_t familyId = pet->GetCreatureFamily();

        if (familyId) {
            CreatureFamilyRec* rec = g_creatureFamilyDB.GetRecord(familyId);

            if (rec) {
                SStrCopy(family, rec->m_name, sizeof(family));
            }
        }

        for (uint32_t i = args->m_count; i; i--) {
            const MacroSubstring& arg = args->m_data[i - 1];
            char text[1024];
            uint32_t length = arg.m_end - arg.m_start;

            if (length > 1023) {
                length = 1023;
            }

            memcpy(text, arg.m_start, length);
            text[length] = '\0';

            if (!SStrCmpUTF8I(text, name, 0x7FFFFFFF) || !SStrCmpUTF8I(text, family, 0x7FFFFFFF)) {
                return 1;
            }
        }

        return 0;
    }

    return 1;
}

// OFFSET: 0x00A0E410/0x1407B06E0
int32_t EvaluateEquipped(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    for (uint32_t i = args->m_count; i; i--) {
        const MacroSubstring& arg = args->m_data[i - 1];
        char text[1024];
        uint32_t length = arg.m_end - arg.m_start;

        if (length > 1023) {
            length = 1023;
        }

        memcpy(text, arg.m_start, length);
        text[length] = '\0';

        int32_t inventoryType = -1;

        for (uint32_t j = 0; j < 29; j++) {
            if (!SStrCmpI(text, s_inventoryTypeTokens[j], 0x7FFFFFFF) || !SStrCmpUTF8I(text, FrameScript_GetText(s_inventoryTypeTokens[j], -1, GENDER_NOT_APPLICABLE), 0x7FFFFFFF)) {
                inventoryType = j;
                break;
            }
        }

        WOWGUID container;
        uint32_t slot;
        CGItem_C* item;

        if (inventoryType > 0) {
            item = player->m_bag.FindItem(&CanEquipInSlotCallback, &inventoryType, container, slot, 1);
        } else {
            item = player->m_bag.FindItem(&FindItemClassTypeCallback, text, container, slot, 1);
        }

        if (item) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A0E540/0x1407B08D0
int32_t EvaluateChanneling(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t now = OsGetAsyncTimeMs();
    SpellRec* spell;

    if (!(spell = g_spellDB.GetRecord(player->m_channelSpellId)) || static_cast<int32_t>(now - player->m_channelEndTime) >= 0) {
        return 0;
    }

    if (args->m_count <= 0) {
        return 1;
    }

    for (uint32_t i = args->m_count; i; i--) {
        const MacroSubstring& arg = args->m_data[i - 1];
        char text[1024];
        uint32_t length = arg.m_end - arg.m_start;

        if (length > 1023) {
            length = 1023;
        }

        memcpy(text, arg.m_start, length);
        text[length] = '\0';

        if (!SStrCmpUTF8I(text, spell->m_name, 0x7FFFFFFF)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A0E620/0x1407B0A50
int32_t EvaluateResting(const WOWGUID* unit, const TSGrowableArray<MacroSubstring>* args) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    return player && (player->m_player->m_playerFlags & 0x20);
}
