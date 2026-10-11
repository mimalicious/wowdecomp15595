#include <cstdint>
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include <cstring>
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/SharedScripts.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/DB/Rec/FileDataRec.h"
#include "WoW/Source/DB/Rec/UnitPowerBarRec.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/DB/Rec/CreatureTypeRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include <cfloat>
#include <cstdlib>
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/SpellDurationRec.h"
#include <cmath>
#include "WoW/Source/DB/Rec/VehicleUIIndicatorRec.h"
#include "WoW/Source/DB/Rec/VehicleUIIndSeatRec.h"
#include "WoW/Source/WowServices/WDataStore.h"

int32_t Script_UnitIsPartyLeader(lua_State* L);
int32_t Script_UnitInParty(lua_State* L);
int32_t Script_UnitIsDND(lua_State* L);
int32_t Script_UnitIsPVPSanctuary(lua_State* L);
int32_t Script_UnitIsPVPFreeForAll(lua_State* L);
int32_t Script_UnitFactionGroup(lua_State* L);
int32_t Script_UnitReaction(lua_State* L);
int32_t Script_UnitIsEnemy(lua_State* L);
int32_t Script_UnitCanAssist(lua_State* L);
int32_t Script_UnitCanAttack(lua_State* L);
int32_t Script_PlayerCanTeleport(lua_State* L);
int32_t Script_UnitName(lua_State* L);
int32_t Script_UnitPVPName(lua_State* L);
int32_t Script_UnitPower(lua_State* L);
int32_t Script_UnitPowerMax(lua_State* L);
int32_t Script_UnitPowerType(lua_State* L);
int32_t Script_UnitResistance(lua_State* L);
int32_t Script_UnitStat(lua_State* L);
int32_t Script_UnitAttackBothHands(lua_State* L);
int32_t Script_UnitDefense(lua_State* L);
int32_t Script_UnitArmor(lua_State* L);
int32_t Script_UnitIsTapped(lua_State* L);
int32_t Script_UnitIsTappedByPlayer(lua_State* L);
int32_t Script_UnitIsTappedByAllThreatList(lua_State* L);
int32_t Script_UnitIsTrivial(lua_State* L);
int32_t Script_UnitHasRelicSlot(lua_State* L);
int32_t Script_SetPortraitTexture(lua_State* L);
int32_t Script_HasFullControl(lua_State* L);
int32_t Script_GetComboPoints(lua_State* L);
int32_t Script_IsInGuild(lua_State* L);
int32_t Script_IsGuildLeader(lua_State* L);
int32_t Script_IsArenaTeamCaptain(lua_State* L);
int32_t Script_IsInArenaTeam(lua_State* L);
int32_t Script_IsResting(lua_State* L);
int32_t Script_GetCombatRating(lua_State* L);
int32_t Script_GetMaxCombatRatingBonus(lua_State* L);
int32_t Script_GetDodgeChance(lua_State* L);
int32_t Script_GetBlockChance(lua_State* L);
int32_t Script_GetShieldBlock(lua_State* L);
int32_t Script_GetParryChance(lua_State* L);
int32_t Script_GetCritChance(lua_State* L);
int32_t Script_GetRangedCritChance(lua_State* L);
int32_t Script_GetSpellCritChance(lua_State* L);
int32_t Script_GetSpellBonusHealing(lua_State* L);
int32_t Script_GetPetSpellBonusDamage(lua_State* L);
int32_t Script_GetSpellPenetration(lua_State* L);
int32_t Script_UnitCreatureType(lua_State* L);
int32_t Script_UnitCreatureFamily(lua_State* L);
int32_t Script_GetPVPSessionStats(lua_State* L);
int32_t Script_GetPVPYesterdayStats(lua_State* L);
int32_t Script_GetPVPLifetimeStats(lua_State* L);
int32_t Script_UnitPVPRank(lua_State* L);
int32_t Script_GetPVPRankInfo(lua_State* L);
int32_t Script_UnitCastingInfo(lua_State* L);
int32_t Script_UnitChannelInfo(lua_State* L);
int32_t Script_IsLoggedIn(lua_State* L);
int32_t Script_IsIndoors(lua_State* L);
int32_t Script_IsOutdoors(lua_State* L);
int32_t Script_IsOutOfBounds(lua_State* L);
int32_t Script_IsFalling(lua_State* L);
int32_t Script_IsSwimming(lua_State* L);
int32_t Script_IsFlying(lua_State* L);
int32_t Script_IsMounted(lua_State* L);
int32_t Script_IsStealthed(lua_State* L);
int32_t Script_GetUnitMaxHealthModifier(lua_State* L);
int32_t Script_GetManaRegen(lua_State* L);
int32_t Script_GetPowerRegen(lua_State* L);
int32_t Script_GetRuneCount(lua_State* L);
int32_t Script_GetExpertise(lua_State* L);
int32_t Script_GetExpertisePercent(lua_State* L);
int32_t Script_GetUnitSpeed(lua_State* L);
int32_t Script_GetUnitPitch(lua_State* L);
int32_t Script_UnitInVehicle(lua_State* L);
int32_t Script_UnitUsingVehicle(lua_State* L);
int32_t Script_UnitControllingVehicle(lua_State* L);
int32_t Script_UnitVehicleSeatCount(lua_State* L);
int32_t Script_CanSwitchVehicleSeat(lua_State* L);
int32_t Script_UnitIsControlling(lua_State* L);
int32_t Script_CanEjectPassengerFromSeat(lua_State* L);
int32_t Script_IsXPUserDisabled(lua_State* L);

// STATIC: 0x00BE4580
const int32_t s_unitSexValues[3] = { 2, 3, 1 };

// STATIC: #0x00AD21BC
static const char* s_unitClassificationTokens[7] = { "normal", "elite", "rareelite", "worldboss", "rare", "trivial", NULL };

// STATIC: #0x00AD8660
extern const char* s_vehicleSkinNames[2] = { "Natural", "Mechanical" };

// STATIC: #0x00C25A0C
static int32_t s_vehicleUIIndicatorID;

// STATIC: 0x00D39220
static const char* s_vehicleUIIndicatorTexture;

// STATIC: #0x00C25A14
static TSGrowableArray<VehicleUIIndSeatRec*> s_vehicleUIIndicatorSeats;

// OFFSET: 0x00454730/0x1400635C0
static int32_t Script_UnitRace(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitRace(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    ChrRacesRec* rec;
    const char* name;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        rec = g_chrRacesDB.GetRecord(ClientServices::GetCharacterRace());
        name = CGUnit_C::GetDisplayRaceNameFromRecord(rec, ClientServices::GetCharacterSex(), NULL);
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            rec = g_chrRacesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[0]);
            name = CGUnit_C::GetDisplayRaceName(unit, NULL);
        } else {
            NameCache* nameCache = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);

            if (!nameCache) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            rec = g_chrRacesDB.GetRecord(nameCache->m_race);
            name = CGUnit_C::GetDisplayRaceName(NULL, nameCache);
        }
    }

    if (rec && name) {
        lua_pushstring(L, name);
        lua_pushstring(L, rec->m_clientFileString);
        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x004548B0/0x1400637A0
static int32_t Script_UnitClass(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitClass(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    ChrClassesRec* rec;
    const char* name;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        rec = g_chrClassesDB.GetRecord(ClientServices::GetCharacterClass());
        name = CGUnit_C::GetDisplayClassNameFromRecord(rec, ClientServices::GetCharacterSex(), NULL);
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            rec = g_chrClassesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[1]);
            name = CGUnit_C::GetDisplayClassName(unit, NULL);
        } else {
            NameCache* nameCache = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);

            if (!nameCache) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            rec = g_chrClassesDB.GetRecord(nameCache->m_class);
            name = CGUnit_C::GetDisplayClassName(NULL, nameCache);
        }
    }

    if (rec && name) {
        lua_pushstring(L, name);
        lua_pushstring(L, rec->m_filename);
        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x00450C30/0x14005F110
static int32_t Script_UnitIsVisible(lua_State* L) {
    WOWGUID guid;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false) && ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00450D80/0x14005F2D0
static int32_t Script_UnitIsPlayer(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;

    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__) || CGGameUI::IsPartyMember(unitGuid)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00450F90/0x14005F530
static int32_t Script_UnitIsCorpse(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);

    if (ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451450/0x14005FAA0
static int32_t Script_UnitPlayerControlled(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (unit->m_unit->m_unitFieldFlags & 0x8)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004520C0/0x140060AA0
static int32_t Script_UnitIsCharmed(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && unit->m_unit->m_unitFieldCharmedby.m_guid) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00452150/0x140060B30
static int32_t Script_UnitIsPossessed(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (unit->m_unit->m_unitFieldFlags & 0x1000000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00452260/0x140060C40
static int32_t Script_UnitClassification(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
        lua_pushstring(L, s_unitClassificationTokens[unit->GetCreatureClassification()]);
        return 1;
    }

    lua_pushstring(L, s_unitClassificationTokens[0]);
    return 1;
}

// OFFSET: 0x004535C0/0x140062220
static int32_t Script_UnitHealth(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHealth(\"unit\")");
        return 0;
    }

    int32_t health = 0;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!(unit->m_unit->m_unitDynamicFlags & 0x20) || CGGameUI::IsRaidMemberOrPet(unitGuid)) {
            health = unit->GetPredictedHealth();
        } else {
            health = 0;
        }
    } else if (PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid)) {
        health = state->m_health;
    } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid)) {
        health = petState->m_health;
    }

    lua_pushnumber(L, health);
    return 1;
}

// OFFSET: 0x00453D50/0x140062A60
static int32_t Script_UnitOnTaxi(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitOnTaxi(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_unit->m_unitFieldFlags & 0x100000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00453DD0/0x140062B20
static int32_t Script_UnitIsFeignDeath(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsFeignDeath(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit) {
        WOWGUID guid = unit->m_obj->m_guid;

        if (CGGameUI::IsRaidMemberOrPet(guid) && (unit->m_unit->m_unitDynamicFlags & 0x20)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00453E80/0x140062C10
static int32_t Script_UnitIsDead(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsDead(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->m_unit->m_unitFieldHealth <= 0) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        if (unit->m_unit->m_unitDynamicFlags & 0x20) {
            guid = unit->m_obj->m_guid;

            if (!CGGameUI::IsRaidMemberOrPet(guid)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid);

        if (state && (state->m_flags & 0x4)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00453F80/0x140062D20
static int32_t Script_UnitIsGhost(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsGhost(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x10)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid);

        if (state && (state->m_flags & 0x8)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00454080/0x140062E20
static int32_t Script_UnitIsDeadOrGhost(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsDeadOrGhost(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->IsDeadOrGhost()) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        if (unit->m_unit->m_unitDynamicFlags & 0x20) {
            guid = unit->m_obj->m_guid;

            if (!CGGameUI::IsRaidMemberOrPet(guid)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid);

        if (state && (state->m_flags & 0xC)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00454190/0x140062F40
static int32_t Script_UnitIsConnected(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsConnected(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;

    if (!ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid);
        bool connected = state ? (state->m_flags & 0x1) != 0 : CGGameUI::GetPartyMemberPetState(unitGuid) != NULL;

        if (!connected) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00454260/0x140063030
static int32_t Script_UnitAffectingCombat(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAffectingCombat(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_unit->m_unitFieldFlags & 0x80000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004542E0/0x1400630F0
static int32_t Script_UnitSex(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitSex(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    int32_t sex;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        sex = s_unitSexValues[ClientServices::GetCharacterSex()];
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            sex = s_unitSexValues[unit->m_unit->m_unitFieldBytes0[2]];
        } else {
            NameCache* nameCache = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);
            sex = nameCache ? s_unitSexValues[nameCache->m_sex] : 1;
        }
    }

    lua_pushnumber(L, sex);
    return 1;
}

// OFFSET: 0x0044E970/0x14005CC50
static int32_t Script_FillLocalizedClassList(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: FillLocalizedClassList(classTable[, isFemale])");
    }

    int32_t female = StringToBOOL(L, 2, 0) != 0;
    uint32_t count = g_chrClassesDB.m_numRecords;
    lua_settop(L, 1);

    for (int32_t i = 0; static_cast<uint32_t>(i) < count; i++) {
        ChrClassesRec* rec = g_chrClassesDB.GetRecordByIndex(i);

        if (rec) {
            lua_pushstring(L, rec->m_filename);
            lua_pushstring(L, CGUnit_C::GetDisplayClassNameFromRecord(rec, female, NULL));
            lua_settable(L, -3);
        }
    }

    return 1;
}

// OFFSET: 0x00450B80/0x14005F060
static int32_t Script_UnitExists(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if ((object && object->CanBeTargetted()) || CGGameUI::IsRaidMemberOrPet(unitGuid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004536C0/0x140062330
static int32_t Script_UnitHealthMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHealthMax(\"unit\")");
        return 0;
    }

    int32_t maxHealth = 0;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        maxHealth = unit->m_unit->m_unitFieldMaxhealth;
    } else if (PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid)) {
        maxHealth = state->m_maxHealth;
    } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid)) {
        maxHealth = petState->m_maxHealth;
    }

    lua_pushnumber(L, maxHealth);
    return 1;
}

// OFFSET: 0x004534C0/0x1400620A0
static int32_t Script_UnitXP(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitXP(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        lua_pushnumber(L, static_cast<CGPlayer_C*>(unit)->GetCurrentXP());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00453540/0x140062160
static int32_t Script_UnitXPMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitXPMax(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        lua_pushnumber(L, static_cast<CGPlayer_C*>(unit)->GetNextLevelXP());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00454670/0x140063520
static int32_t Script_GetMoney(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    lua_pushnumber(L, player->GetCoinage());
    return 1;
}

// OFFSET: 0x0044BC90/0x1400592B0
static int32_t Script_GetTime(lua_State* L) {
    lua_pushnumber(L, OsGetAsyncTimeMs() * 0.001);

    return 1;
}

// OFFSET: 0x0044BD10/0x140059330
static int32_t Script_GetGameTime(lua_State* L) {
    lua_pushnumber(L, g_clientGameTime.m_hour);
    lua_pushnumber(L, g_clientGameTime.m_minute);

    return 2;
}

// OFFSET: 0x0044E610/0x14005C850
static int32_t Script_ConsoleExec(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ConsoleExec(\"console_command\")");
        return 0;
    }

    ConsoleCommandExecute(lua_tolstring(L, 1, NULL), 0);

    return 0;
}

// OFFSET: 0x0044BD50/0x140632130
static int32_t Script_ReadFile(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// OFFSET: 0x0044BD50/0x140632130
static int32_t Script_DeleteFile(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// OFFSET: 0x0044BD50/0x140632130
static int32_t Script_AppendToFile(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// OFFSET: 0x0045B060/0x14006B980
static int32_t Script_UnitName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitName(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        lua_pushstring(L, ClientServices::GetCharacterName());
        lua_pushnil(L);
        return 2;
    }

    const char* realm = NULL;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID unitGuid = guid;

    if (!guid || guid.m_guid == 0xFFFFFFFFFFFFFFFEULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    const char* name = NULL;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        uint32_t type = object->m_obj->m_type;

        if (type & TYPEMASK_UNIT) {
            name = static_cast<CGUnit_C*>(object)->GetUnitName(&realm, 1);
        } else if (type & TYPEMASK_GAMEOBJECT) {
            name = static_cast<CGGameObject_C*>(object)->CGGameObject_C::GetObjectName();
        } else if (type & TYPEMASK_ITEM) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(object->m_obj->m_entryID, none, NULL, NULL, false);

            if (stats) {
                name = stats->GetName(0);
            }
        } else if (type & TYPEMASK_CORPSE) {
            WOWGUID owner = static_cast<CGCorpse_C*>(object)->m_corpse->m_corpseFieldOwner;
            NameCache* record = g_nameDBCache.GetRecord(owner.m_guid, owner, UnitNameQueryCallback, NULL, true);

            if (record) {
                name = record->m_name;

                if (record->m_realmName[0]) {
                    realm = record->m_realmName;
                }
            }
        }
    } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid)) {
        name = petState->m_name;
    } else if ((unitGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
        PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((unitGuid.m_guid >> 24) & 0xFFFFFFF), unitGuid, UnitNameQueryCallback, NULL, true);

        if (record) {
            name = record->m_name;
        }
    } else {
        NameCache* record = g_nameDBCache.GetRecord(unitGuid.m_guid, unitGuid, UnitNameQueryCallback, NULL, true);

        if (record) {
            name = record->m_name;
            realm = record->m_realmName;
        }
    }

    if (!name) {
        name = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
    }

    lua_pushstring(L, name);
    lua_pushstring(L, realm);
    return 2;
}

// OFFSET: 0x004537A0/0x140062400
static int32_t Script_UnitPower(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPower(\"unit\"[, type])");
        return 0;
    }

    uint32_t powerType = 7;

    if (lua_isnumber(L, 2)) {
        powerType = static_cast<uint32_t>(lua_tointeger(L, 2));

        if (powerType >= 8) {
            lua_pushnumber(L, 0.0);
            return 1;
        }
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    double power = 0.0;

    if (unit) {
        if (powerType == 7) {
            powerType = unit->m_unit->m_unitFieldBytes0[3];
        }

        if ((!(unit->m_unit->m_unitDynamicFlags & 0x20) || CGGameUI::IsRaidMemberOrPet(unit->m_obj->m_guid)) && (powerType == unit->m_unit->m_unitFieldBytes0[3] || unit->IsActivePlayer())) {
            power = static_cast<uint32_t>(unit->GetPredictedPower(powerType)) / Spell_C_GetPowerDisplayMod(powerType);
        }
    } else if (PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid)) {
        if (powerType == 7 || powerType == state->m_powerType) {
            power = state->m_power / Spell_C_GetPowerDisplayMod(state->m_powerType);
        }
    } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid)) {
        if (powerType == 7 || powerType == petState->m_powerType) {
            power = petState->m_power / Spell_C_GetPowerDisplayMod(petState->m_powerType);
        }
    }

    lua_pushnumber(L, power);
    return 1;
}

// OFFSET: 0x00453980/0x140062600
static int32_t Script_UnitPowerMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPowerMax(\"unit\"[, type])");
        return 0;
    }

    uint32_t powerType = 7;

    if (lua_isnumber(L, 2)) {
        powerType = static_cast<uint32_t>(lua_tointeger(L, 2));

        if (powerType >= 8) {
            lua_pushnumber(L, 0.0);
            return 1;
        }
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    double power = 0.0;

    if (unit) {
        if (powerType == 7) {
            powerType = unit->m_unit->m_unitFieldBytes0[3];
        }

        power = unit->m_unit->m_unitFieldMaxpower[powerType] / Spell_C_GetPowerDisplayMod(powerType);
    } else if (PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid)) {
        if (powerType == 7 || powerType == state->m_powerType) {
            power = state->m_maxPower / Spell_C_GetPowerDisplayMod(state->m_powerType);
        }
    } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid)) {
        if (powerType == 7 || powerType == petState->m_powerType) {
            power = petState->m_maxPower / Spell_C_GetPowerDisplayMod(petState->m_powerType);
        }
    }

    lua_pushnumber(L, power);
    return 1;
}

// OFFSET: 0x00453B00/0x1400627A0
static int32_t Script_UnitPowerType(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPowerType(\"unit\"[, index])");
        return 0;
    }

    uint32_t index = 0;

    if (lua_isnumber(L, 2)) {
        index = static_cast<uint32_t>(lua_tointeger(L, 2) - 1);
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_vehicle && unit->m_vehicle->m_vehicleRec && (unit->m_vehicle->m_vehicleRec->m_flags & 0x100000) && index < 3) {
        PowerDisplayRec* display = g_powerDisplayDB.GetRecord(unit->m_vehicle->m_vehicleRec->m_powerDisplayID[index]);

        if (display && display->m_actualType == unit->m_unit->m_unitFieldBytes0[3]) {
            lua_pushnumber(L, display->m_actualType);
            lua_pushstring(L, display->m_globalStringBaseTag);
            lua_pushnumber(L, static_cast<uint8_t>(display->m_red) * 0.0039215689f);
            lua_pushnumber(L, static_cast<uint8_t>(display->m_green) * 0.0039215689f);
            lua_pushnumber(L, static_cast<uint8_t>(display->m_blue) * 0.0039215689f);
            return 5;
        }
    }

    if (!index) {
        if (unit) {
            lua_pushnumber(L, unit->m_unit->m_unitFieldBytes0[3]);
            lua_pushstring(L, GetPowerString(unit->m_unit->m_unitFieldBytes0[3]));
            return 2;
        }

        if (PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid)) {
            lua_pushnumber(L, state->m_powerType);
            lua_pushstring(L, GetPowerString(state->m_powerType));
            return 2;
        }

        if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid)) {
            lua_pushnumber(L, petState->m_powerType);
            lua_pushstring(L, GetPowerString(petState->m_powerType));
            return 2;
        }
    }

    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    return 2;
}

// OFFSET: 0x00451960/0x14005FFE0
static int32_t Script_UnitFactionGroup(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitFactionGroup(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    int32_t factionTemplate = 0;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        ChrRacesRec* race = g_chrRacesDB.GetRecord(ClientServices::GetCharacterRace());

        if (!race) {
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }

        factionTemplate = race->m_factionID;
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false, false);
        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
            factionTemplate = static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldFactiontemplate;
        } else if (object && (object->m_obj->m_type & TYPEMASK_CORPSE)) {
            factionTemplate = static_cast<CGCorpse_C*>(object)->GetFactionTemplate();
        } else {
            if (!guid) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            WOWGUID none = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);
            ChrRacesRec* race = record ? g_chrRacesDB.GetRecord(record->m_race) : NULL;

            if (!race) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            factionTemplate = race->m_factionID;
        }
    }

    FactionTemplateRec* faction = factionTemplate ? g_factionTemplateDB.GetRecord(factionTemplate) : NULL;

    if (faction) {
        for (int32_t i = 0; i < g_factionGroupDB.m_numRecords; i++) {
            FactionGroupRec* group = g_factionGroupDB.GetRecordByIndex(i);

            if (((1 << group->m_maskID) & faction->m_factionGroup) && *group->m_name) {
                lua_pushstring(L, group->m_internalName);
                lua_pushstring(L, group->m_name);
                return 2;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x0044E890/0x14005CB90
static int32_t Script_IsLoggedIn(lua_State* L) {
    if (CGGameUI::m_loggingIn) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// PROVEN: a65fb5b9 IDENTICAL/NONE
// OFFSET: 0x0044C0F0/0x140059730
static int32_t Script_CanSwitchVehicleSeat(lua_State* L) {
    CGInputControl* inputControl = CGInputControl::GetActive();

    if (inputControl) {
        lua_pushboolean(L, inputControl->IsVehicleSeatFlagSet(0x4000000));
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x00458C10/0x140068AC0
static int32_t Script_IsXPUserDisabled(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushboolean(L, (player->m_player->m_playerFlags >> 25) & 1);
    return 1;
}

// OFFSET: 0x004524A0/0x140060EE0
static int32_t Script_IsInGuild(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004528B0/0x140061320
static int32_t Script_GetDodgeChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerDodgePercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452910/0x140061390
static int32_t Script_GetBlockChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerBlockPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004529F0/0x140061470
static int32_t Script_GetParryChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerParryPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452AF0/0x140061570
static int32_t Script_GetCritChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerCritPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452C50/0x140061780
static int32_t Script_GetRangedCritChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerRangedCritPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452DF0/0x140061980
static int32_t Script_GetSpellBonusHealing(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerFieldModHealingDonePos));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452E60/0x1400619F0
static int32_t Script_GetPetSpellBonusDamage(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerPetSpellPower));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// PROVEN: 51a82bdd IDENTICAL/NONE
// OFFSET: 0x00456A50/0x140066250
static int32_t Script_IsIndoors(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !player->IsOutdoors()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456AB0/0x1400662C0
static int32_t Script_IsOutdoors(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->IsOutdoors()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456C10/0x140066470
static int32_t Script_IsSwimming(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_clientFlags & 0x200000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456E80/0x1400667A0
static int32_t Script_IsStealthed(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_unit->m_unitFieldBytes1[2] & 2)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00452510/0x140060F50
static int32_t Script_IsGuildLeader(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID() && !player->m_player->m_playerGuildrank) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004526C0/0x140061120
static int32_t Script_IsResting(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x20)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00452EC0/0x140061A60
static int32_t Script_GetSpellPenetration(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, -static_cast<int32_t>(player->m_player->m_playerFieldModTargetResistance));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00455DE0/0x1400652A0
static int32_t Script_HasFullControl(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_combatModeFlags & 1) && player->m_unit->m_unitFieldHealth > 0 && CGGameUI::m_hasControl) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456390/0x140065960
static int32_t Script_UnitPVPRank(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPVPRank(\"unit\")");
        return 0;
    }

    Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00456B10/0x140066330
static int32_t Script_IsOutOfBounds(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x4000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456B80/0x1400663A0
static int32_t Script_IsFalling(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t flags = player->m_movement->m_flags;

        if ((flags & 0x1000) && !(flags & 0x800)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00456E10/0x140066730
static int32_t Script_IsMounted(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_mountDisplayId <= 0 || (player->m_clientFlags & 0x10000000)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// OFFSET: 0x004511D0/0x14005F7C0
static int32_t Script_UnitInParty(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID member = guid;

    if (CGPartyInfo::IsMember(member)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004521E0/0x140060BC0
static int32_t Script_PlayerCanTeleport(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t flags = player->m_unit->m_unitFieldFlags;

    if ((flags & 0x40000) || (flags & 0x800000) || (flags & 0x400000)) {
        lua_pushboolean(L, 0);
    } else {
        lua_pushboolean(L, 1);
    }

    return 1;
}

// OFFSET: 0x00452570/0x140060FC0
static int32_t Script_IsInArenaTeam(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        for (uint32_t i = 0; i < 3; i++) {
            if (player->m_player->m_playerFieldArenaTeamInfo11[i * 7]) {
                lua_pushboolean(L, 1);
                return 1;
            }
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x00452970/0x140061400
static int32_t Script_GetShieldBlock(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerShieldBlock);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452CB0/0x1400617F0
static int32_t Script_GetSpellCritChance(lua_State* L) {
    uint32_t school = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (school > 6) {
        luaL_error(L, "Usage: GetSpellCritChance(school)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    lua_pushnumber(L, player ? player->m_player->m_playerSpellCritPercentage1[school] : 0.0f);
    return 1;
}

// OFFSET: 0x004558D0/0x140064C70
static int32_t Script_UnitIsTapped(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTapped(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_unit->m_unitDynamicFlags & 4)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00455950/0x140064D20
static int32_t Script_UnitIsTappedByPlayer(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTappedByPlayer(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_unit->m_unitDynamicFlags & 8)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004559D0/0x140064DD0
static int32_t Script_UnitIsTappedByAllThreatList(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTappedByAllThreatList(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_unit->m_unitDynamicFlags & 0x80)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00455A50/0x140064E80
static int32_t Script_UnitIsTrivial(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTrivial(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && CGPlayer_C::UnitIsTrivial(unit)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451020/0x14005F5B0
static int32_t Script_UnitIsPartyLeader(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if ((player && (player->m_player->m_playerFlags & 1)) || CGPartyInfo::m_leader == guid) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004517E0/0x14005FE60
static int32_t Script_UnitIsPVPSanctuary(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(token, guid, false, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (unit->m_unit->m_unitFieldBytes2[1] & 8)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451C10/0x1400603C0
static int32_t Script_UnitIsEnemy(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsEnemy(\"unit\", \"otherUnit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    CGUnit_C* other = Script_GetUnitFromName(lua_tolstring(L, 2, NULL));

    if (unit && other && unit->UnitReaction(other) <= 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451F80/0x140060820
static int32_t Script_UnitCanAssist(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitCanAssist(\"unit\", \"otherUnit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    CGUnit_C* other = Script_GetUnitFromName(lua_tolstring(L, 2, NULL));

    if (unit && other && unit->CanAssist(other, false)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00452020/0x140060960
static int32_t Script_UnitCanAttack(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitCanAttack(\"unit\", \"otherUnit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    CGUnit_C* other = Script_GetUnitFromName(lua_tolstring(L, 2, NULL));

    if (unit && other && unit->CanAttack(other)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00455F70/0x140065470
static int32_t Script_UnitCreatureType(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCreatureType(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    int32_t type;
    CreatureTypeRec* rec;

    if (unit && (type = unit->GetCreatureType()) && (rec = g_creatureTypeDB.GetRecord(type))) {
        lua_pushstring(L, rec->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456010/0x140065560
static int32_t Script_UnitCreatureFamily(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCreatureFamily(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    uint32_t family;
    CreatureFamilyRec* rec;

    if (unit && (family = unit->GetCreatureFamily()) && (rec = g_creatureFamilyDB.GetRecord(family))) {
        lua_pushstring(L, rec->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456D20/0x140066610
static int32_t Script_IsFlying(lua_State* L) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->m_state == 3) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(unit->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (unit && (unit->m_movement->m_flags & 0x2000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004570F0/0x140066AC0
static int32_t Script_GetUnitMaxHealthModifier(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitMaxHealthModifier(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        lua_pushnumber(L, unit->m_unit->m_unitFieldMaxhealthmodifier + 1.0);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004574B0/0x140066F60
static int32_t Script_GetExpertisePercent(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerExpertise) * 0.25);
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerOffhandExpertise) * 0.25);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// OFFSET: 0x00457B80/0x140067710
static int32_t Script_GetUnitSpeed(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitSpeed(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    lua_pushnumber(L, unit ? unit->m_movement->m_currentSpeed : 0.0f);
    return 1;
}

// OFFSET: 0x00457C90/0x140067860
static int32_t Script_GetUnitPitch(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitPitch(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    lua_pushnumber(L, unit ? unit->GetPitch() : 0.0f);
    return 1;
}

// OFFSET: 0x00457EF0/0x140067AE0
static int32_t Script_UnitControllingVehicle(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    int32_t controlling = 0;
    WOWGUID guid;

    if (Script_GetGUIDFromToken(token, guid, false, false)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->m_state == 3 && unit->GetTransportGUID() == unit->m_unit->m_unitFieldCharm.m_guid) {
            controlling = 1;
        }
    }

    lua_pushboolean(L, controlling);
    return 1;
}

// OFFSET: 0x004581F0/0x140067E50
static int32_t Script_UnitVehicleSeatCount(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid;
    CGUnit_C* unit;
    CGUnit_C* root;

    if (Script_GetGUIDFromToken(token, guid, false, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (root = unit->FindRootVehicle(NULL)) && root->m_vehicle && root->m_vehicle->m_vehicleRec) {
        lua_pushnumber(L, root->m_vehicle->GetVirtualSeatCount());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00458760/0x1400684D0
static int32_t Script_UnitIsControlling(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid;

    if (Script_GetGUIDFromToken(token, guid, false, false)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            CGUnitData* data = unit->m_unit;

            if (data->m_unitFieldCharm ? data->m_unitFieldCharm : data->m_unitFieldSummon) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0044BD70/0x140059370
static int32_t Script_GetMaxCombatRatingBonus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMaxCombatRatingBonus(ratingIndex)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 25) {
        luaL_error(L, "ratingIndex is in the range 1 .. %d", 25);
        return 0;
    }

    lua_pushnumber(L, index < 14 || index > 16 ? -1.0 : 33.000001311302185);
    return 1;
}

// OFFSET: 0x00451B60/0x140060280
static int32_t Script_UnitReaction(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitReaction(\"unit\", \"otherUnit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    CGUnit_C* other = Script_GetUnitFromName(lua_tolstring(L, 2, NULL));

    if (unit && other) {
        lua_pushnumber(L, unit->UnitReaction(other) + 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00457290/0x140066D40
static int32_t Script_GetManaRegen(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_unit->m_unitFieldBytes0[3]) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    lua_pushnumber(L, player->GetPowerRegen(0, false) + 0.001f);
    lua_pushnumber(L, player->GetPowerRegen(0, true) + 0.001f);
    return 2;
}

// OFFSET: 0x00457340/0x140066DF0
static int32_t Script_GetPowerRegen(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint8_t powerType = player->m_unit->m_unitFieldBytes0[3];
        lua_pushnumber(L, player->GetPowerRegen(powerType, false) + 0.001f);
        lua_pushnumber(L, player->GetPowerRegen(powerType, true) + 0.001f);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// OFFSET: 0x00452730/0x140061190
static int32_t Script_GetCombatRating(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCombatRating(ratingIndex)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 25) {
        luaL_error(L, "ratingIndex is in the range 1 .. %d", 25);
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerFieldCombatRating1[index]));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004573F0/0x140066EC0
static int32_t Script_GetExpertise(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerExpertise);
        lua_pushnumber(L, player->m_player->m_playerOffhandExpertise);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// OFFSET: 0x004561F0/0x1400657E0
static int32_t Script_GetPVPSessionStats(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    lua_pushnumber(L, player ? player->m_player->m_playerFieldKills[0] : 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x00456260/0x140065850
static int32_t Script_GetPVPYesterdayStats(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerFieldKills[1]);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// OFFSET: 0x004562D0/0x1400658C0
static int32_t Script_GetPVPLifetimeStats(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerFieldLifetimeHonorbaleKills);
        uint8_t rank = player->m_player->m_playerFieldBytes[3];
        lua_pushnumber(L, rank < 5 ? 0.0 : rank);
        return 2;
    }

    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x004525F0/0x140061050
static int32_t Script_IsArenaTeamCaptain(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsArenaTeamCaptain(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t* info = player && index < 3 ? &player->m_player->m_playerFieldArenaTeamInfo11[index * 7] : NULL;

    if (!info || info[2]) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// OFFSET: 0x00451600/0x14005FC80
static int32_t Script_UnitIsDND(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsDND(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID stateGuid = guid;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t dnd;

    if (player) {
        dnd = player->m_player->m_playerFlags >> 2;
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(stateGuid);

        if (!state) {
            lua_pushnil(L);
            return 1;
        }

        dnd = static_cast<uint8_t>(state->m_flags) >> 7;
    }

    if (dnd & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00457D30/0x140067920
static int32_t Script_UnitInVehicle(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitInVehicle(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID stateGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!unit->m_vehiclePassenger || unit->m_vehiclePassenger->m_state != 3) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(stateGuid);

        if (!state || !(state->m_flags & 0x200)) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00457E10/0x140067A00
static int32_t Script_UnitUsingVehicle(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitUsingVehicle(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID stateGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!unit->m_vehiclePassenger || !unit->m_vehiclePassenger->m_state) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(stateGuid);

        if (!state || !(state->m_flags & 0x200)) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00455AD0/0x140064F30
static int32_t Script_UnitHasRelicSlot(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHasRelicSlot(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    ChrClassesRec* rec;

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER) && (rec = g_chrClassesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[1])) && (rec->m_flags & 8)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00453400/0x140061FC0
static int32_t Script_UnitPVPName(lua_State* L) {
    char buffer[1024];

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPVPName(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        lua_pushstring(L, unit->GetUnitChosenTitle(buffer, sizeof(buffer), 1, NULL));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451870/0x14005FEF0
static int32_t Script_UnitIsPVPFreeForAll(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsPVPFreeForAll(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID stateGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    uint32_t ffa;

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        ffa = static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldBytes2[1] >> 2;
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(stateGuid);

        if (!state) {
            lua_pushnil(L);
            return 1;
        }

        ffa = static_cast<uint8_t>(state->m_flags) >> 4;
    }

    if (ffa & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00455750/0x140064A60
static int32_t Script_UnitArmor(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitArmor(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    int32_t total = 0;
    int32_t effective = 0;
    int32_t positive = 0;
    int32_t negative = 0;
    int32_t base = 0;

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        unit->GetResistanceAndBuffs(GetPhysicalDamageClassID(), &base, &total, &effective, &positive, &negative);
    }

    lua_pushnumber(L, base);
    lua_pushnumber(L, effective);
    lua_pushnumber(L, total);
    lua_pushnumber(L, positive);
    lua_pushnumber(L, negative);
    return 5;
}

// OFFSET: 0x004587F0/0x140068570
static int32_t Script_CanEjectPassengerFromSeat(lua_State* L) {
    int32_t canEject = 0;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanEjectPassengerFromSeat(seatIndex)");
        return 0;
    }

    int32_t seatIndex = static_cast<int32_t>(lua_tonumber(L, 1));
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGUnit_C* root;
    const CVehicle_C* vehicle;
    uint8_t seat;
    CGUnit_C* passenger;
    VehicleSeatRec* seatRec;

    if (mover && (root = mover->FindRootVehicle(NULL)) && root->m_vehicle && root->m_vehicle->m_vehicleRec && root->m_vehicle->GetVehicleAndSeatFromVirtualSeat(seatIndex - 1, vehicle, seat) && (passenger = const_cast<CVehicle_C*>(vehicle)->FindPassengerUnit(seat)) && passenger->FindRootVehicle(mover) == mover && (seatRec = vehicle->GetVehicleSeatRec(seat)) && (seatRec->m_flagsB & 0x20)) {
        canEject = 1;
    }

    lua_pushboolean(L, canEject);
    return 1;
}

// OFFSET: 0x00454D00/0x140063D70
static int32_t Script_UnitStat(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UnitStat(\"unit\", statIndex)");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    uint32_t stat = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (stat > 4) {
        luaL_error(L, "Invalid stat index in UnitStat");
        return 0;
    }

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        lua_pushnumber(L, unit->m_unit->m_unitFieldStat[stat]);
        lua_pushnumber(L, unit->GetEffectiveStat(stat));
        lua_pushnumber(L, unit->m_unit->m_unitFieldPosstat[stat]);
        lua_pushnumber(L, unit->m_unit->m_unitFieldNegstat[stat]);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 4;
}

// OFFSET: 0x00454BB0/0x140063B80
static int32_t Script_UnitResistance(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UnitResistance(\"unit\", resistanceIndex)");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    uint32_t school = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (school > 6) {
        luaL_error(L, "Invalid resistance index in UnitResistance");
        return 0;
    }

    int32_t total = 0;
    int32_t positive = 0;
    int32_t negative = 0;
    int32_t base = 0;
    int32_t effective;

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            unit->GetResistanceAndBuffs(school, &base, &total, &effective, &positive, &negative);
        } else {
            int32_t resistance = unit->m_unit->m_unitFieldResistances[school];
            total = resistance <= 0 ? 0 : resistance;
            base = total;
        }
    }

    lua_pushnumber(L, base);
    lua_pushnumber(L, total);
    lua_pushnumber(L, positive);
    lua_pushnumber(L, negative);
    return 4;
}

// OFFSET: 0x00455670/0x140064940
static int32_t Script_UnitDefense(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitDefense(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    int32_t rank = 0;
    int32_t bonus = 0;

    if (unit && (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() || unit->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer())) {
        unit->GetDefenseSkillRank(rank, bonus);
    }

    if (rank + bonus < 0) {
        bonus = -rank;
    }

    lua_pushnumber(L, rank);
    lua_pushnumber(L, bonus);
    return 2;
}

// OFFSET: 0x00454E40/0x140063F40
static int32_t Script_UnitAttackBothHands(lua_State* L) {
    int32_t bonus[2];
    int32_t rank[2];

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAttackBothHands(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() || unit->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer())) {
        for (int32_t i = 0; i < 2; i++) {
            unit->GetAttackSkillRank(i, rank[i], bonus[i]);

            if (rank[i] + bonus[i] < 0) {
                bonus[i] = -rank[i];
            }
        }
    } else {
        bonus[0] = 0;
        rank[0] = 0;
        bonus[1] = 0;
        rank[1] = 0;
    }

    for (int32_t i = 0; i < 2; i++) {
        lua_pushnumber(L, rank[i]);
        lua_pushnumber(L, bonus[i]);
    }

    return 4;
}

// OFFSET: 0x00456540/0x140065B70
static int32_t Script_UnitCastingInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCastingInfo(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit) {
        return 0;
    }

    uint32_t now = OsGetAsyncTimeMs();
    SpellRec* spell;

    if (!(spell = g_spellDB.GetRecord(unit->m_castingSpellId)) || static_cast<int32_t>(now - unit->m_castEndTime) >= 0) {
        return 0;
    }

    const char* name = spell->m_name;
    const char* rank = spell->m_nameSubtext;
    const char* displayName = spell->m_attributesExC & 4 ? "" : spell->m_name;
    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(unit->m_castingObject, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (gameObject && (gameObject->m_obj->m_type & TYPEMASK_GAMEOBJECT)) {
        const char* text = "";
        LockRec* lock = gameObject->GetLockRec();

        if (lock) {
            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

            for (uint32_t i = 0; i < count; i++) {
                const SpellEffectRec* effect = effects->m_data[i];

                if (!effect || effect->m_effect != 33) {
                    continue;
                }

                for (uint32_t j = 0; j < 8; j++) {
                    if (lock->m_type[j] == 2 && effect->m_effectMiscValue == lock->m_index[j]) {
                        int32_t action = lock->m_action[j];

                        if (action >= 0) {
                            if (action <= 1) {
                                text = gameObject->GetOpenText();
                            } else if (action == 2) {
                                text = gameObject->GetCloseText();
                            }
                        }

                        break;
                    }
                }
            }

            if (text && *text) {
                displayName = text;
            }
        }
    }

    const char* icon = "";
    SpellIconRec* iconRec = g_spellIconDB.GetRecord(spell->m_spellIconID);

    if (iconRec) {
        icon = iconRec->m_textureFilename;
    }

    bool isTradeSkill = (spell->m_attributes & 0x20) != 0;
    bool notInterruptible = unit->IsCastNotInterruptible();
    lua_pushstring(L, name);
    lua_pushstring(L, rank);
    lua_pushstring(L, displayName);
    lua_pushstring(L, icon);
    lua_pushnumber(L, unit->m_castStartTime);
    lua_pushnumber(L, unit->m_castEndTime);
    lua_pushboolean(L, isTradeSkill);
    lua_pushnumber(L, unit->m_castCount);
    lua_pushboolean(L, notInterruptible);
    return 9;
}

// OFFSET: 0x00457A30/0x1400675A0
static int32_t Script_GetRuneCount(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRuneCount(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 7) {
        return 0;
    }

    lua_pushnumber(L, (CGRuneInfo::m_predictedRunes & (1 << slot)) != 0);
    return 1;
}

// OFFSET: 0x00451350/0x14005F970
static int32_t Script_UnitInBattleground(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    int32_t index = CGRaidInfo::GetMemberIndexByGuid(guid);

    if (index >= 0 && CGRaidInfo::IsPVPRaid()) {
        lua_pushnumber(L, index);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451E10/0x140060690
static int32_t Script_UnitCanCooperate(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitCanCooperate(\"unit\", \"otherUnit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    const char* otherToken = lua_tolstring(L, 2, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    WOWGUID otherGuid = WOWGUID();
    Script_GetGUIDFromToken(otherToken, otherGuid, false, false);
    CGUnit_C* other = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(otherGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if ((unit && other && unit->CanCooperate(other)) || (!SStrCmpI(token, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(otherGuid)) || (!SStrCmpI(otherToken, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(guid))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004543E0/0x140063230
static int32_t Script_UnitLevel(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitLevel(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t level;

    if (unit) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        level = unit->m_unit->m_unitFieldLevel;

        if ((player && unit->UnitReaction(player) <= 1 && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) <= static_cast<int32_t>(unit->m_unit->m_unitFieldLevel) - 10) || unit->IsBossMob()) {
            level = -1;
        } else if (level > 0 && player && (player->m_player->m_playerBytes3[1] || player->m_player->m_playerFakeInebriation) && player->CanAttack(unit)) {
            level -= RoundToInt64(player->GetDrunknessTurnValue() * 5.0f);

            if (level < 1) {
                level = 1;
            }
        }
    } else if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        level = ClientServices::GetCharacterLevel();
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(guid);

        if (!state) {
            lua_pushnumber(L, 0.0);
            return 1;
        }

        level = state->m_level;
    }

    lua_pushnumber(L, level);
    return 1;
}

// OFFSET: 0x00458420/0x140068100
static int32_t Script_UnitThreatSituation(lua_State* L) {
    uint8_t status = 0;
    WOWGUID guid;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false)) {
        luaL_error(L, "Usage: UnitThreatSituation(\"unit\" [, \"mob\"])");
        return 0;
    }

    if (!guid) {
        return 0;
    }

    if (lua_isstring(L, 2)) {
        CGUnit_C* mob;

        if (!GetUnitByKeyword(lua_tolstring(L, 2, NULL), &mob, false)) {
            luaL_error(L, "Usage: UnitThreatSituation(\"unit\" [, \"mob\"])");
            return 0;
        }

        if (mob) {
            mob->CalculateThreat(guid, &status, NULL, NULL, NULL);
        }
    } else {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return 0;
        }

        status = unit->GetThreatSituation();
    }

    if (!status) {
        return 0;
    }

    lua_pushnumber(L, status - 1);
    return 1;
}

// OFFSET: 0x00458510/0x140068230
static int32_t Script_UnitDetailedThreatSituation(lua_State* L) {
    WOWGUID unitGuid;
    WOWGUID mobGuid;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), unitGuid, false, false) || !Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), mobGuid, false, false)) {
        luaL_error(L, "Usage: UnitDetailedThreatSituation(\"unit\" [, \"mob\"])");
        return 0;
    }

    CGUnit_C* mob = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(mobGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mob) {
        return 0;
    }

    uint8_t status;
    uint8_t rawPercent;
    float percent;
    int32_t threat;
    mob->CalculateThreat(unitGuid, &status, &rawPercent, &percent, &threat);

    if (!status) {
        return 0;
    }

    if (mob->m_highestThreatGuid == unitGuid) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, status - 1);
    lua_pushnumber(L, percent);
    lua_pushnumber(L, rawPercent);
    lua_pushnumber(L, threat);

    return 5;
}

// OFFSET: 0x0045A7A0
static int32_t Script_UnitBuff(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: UnitBuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    info.m_filter = (info.m_filter & ~0x3) | 0x1;
    return PushUnitAura(L, &info);
}

// OFFSET: 0x0045A7F0
static int32_t Script_UnitDebuff(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: UnitDebuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    info.m_filter = (info.m_filter & ~0x1) | 0x2;
    return PushUnitAura(L, &info);
}

// OFFSET: 0x0045A840
static int32_t Script_UnitAura(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: UnitAura(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    return PushUnitAura(L, &info);
}

// OFFSET: 0x00457F80
VehicleSeatRec* GetVehicleSeatRecAsPassengerFromLua(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid;
    guid.m_guid = 0;
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid);

        if (state && state->m_vehicleSeatId > 0) {
            return g_vehicleSeatDB.GetRecord(state->m_vehicleSeatId);
        }

        return NULL;
    }

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

// OFFSET: 0x004580B0/0x140067D00
static int32_t Script_UnitHasVehicleUI(lua_State* L) {
    VehicleSeatRec* seat = GetVehicleSeatRecAsPassengerFromLua(L);

    if (seat) {
        lua_pushboolean(L, seat->m_flags & 0x20000000);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x00450CB0/0x14005F190
static int32_t Script_UnitIsUnit(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsUnit(\"unit\", \"otherUnit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    const char* otherToken = lua_tolstring(L, 2, NULL);

    if (SStrCmpI(token, otherToken, STORM_MAX_STR)) {
        WOWGUID guid = GetGuidByKeyword(token);
        WOWGUID otherGuid = GetGuidByKeyword(otherToken);

        if (!guid || guid.m_guid == 0xFFFFFFFFFFFFFFFEull || guid != otherGuid) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x004567F0/0x140065F80
static int32_t Script_UnitChannelInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitChannelInfo(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit) {
        return 0;
    }

    uint32_t now = OsGetAsyncTimeMs();
    SpellRec* spell;

    if (!(spell = g_spellDB.GetRecord(unit->m_channelSpellId)) || static_cast<int32_t>(now - unit->m_channelEndTime) >= 0) {
        return 0;
    }

    const char* name = spell->m_name;
    const char* rank = spell->m_nameSubtext;
    const char* displayName;

    if (SStrCmpI(lua_tolstring(L, 1, NULL), "player", STORM_MAX_STR) || (spell->m_attributesEx & 0x20000000)) {
        displayName = name;
    } else {
        displayName = FrameScript_GetText("CHANNELING", -1, GENDER_NOT_APPLICABLE);
    }

    const char* icon = "";
    SpellIconRec* iconRec = g_spellIconDB.GetRecord(spell->m_spellIconID);

    if (iconRec) {
        icon = iconRec->m_textureFilename;
    }

    bool tradeSkill = (spell->m_attributes & 0x20) != 0;
    bool notInterruptible = unit->IsCastNotInterruptible();
    lua_pushstring(L, name);
    lua_pushstring(L, rank);
    lua_pushstring(L, displayName);
    lua_pushstring(L, icon);
    lua_pushnumber(L, unit->m_channelStartTime);
    lua_pushnumber(L, unit->m_channelEndTime);
    lua_pushboolean(L, tradeSkill);
    lua_pushboolean(L, notInterruptible);
    return 8;
}

// OFFSET: 0x00455E50/0x140065320
static int32_t Script_GetComboPoints(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetComboPoints(\"unit\"[, \"target\"]");
        return 0;
    }

    WOWGUID unit = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), unit, false, false);
    WOWGUID target = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, false, false);
    double points = 0.0;

    if (unit == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        uint8_t classId = player ? player->m_unit->m_unitFieldBytes0[1] : 0;

        if (!player || classId == 4 || classId == 11) {
            points = CGGameUI::GetComboPoints(target);
        }
    } else if (unit == CGPetInfo::GetPet(0)) {
        points = CGPetInfo::GetComboPoints(target);
    }

    lua_pushnumber(L, points);
    return 1;
}

// OFFSET: 0x00456400/0x140065A00
static int32_t Script_GetPVPRankInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPVPRankInfo(rank [, unit])");
        return 0;
    }

    int32_t rank = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t faction = 0;

    if (lua_isnumber(L, 2)) {
        faction = static_cast<int32_t>(lua_tonumber(L, 2));
    } else {
        CGPlayer_C* player = NULL;
        bool valid = true;

        if (lua_isstring(L, 2)) {
            player = static_cast<CGPlayer_C*>(Script_GetUnitFromName(lua_tolstring(L, 2, NULL)));
            valid = player && (player->m_obj->m_type & TYPEMASK_PLAYER);
        } else {
            player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        }

        if (valid && player) {
            faction = player->GetPVPFactionIndex();
        }
    }

    char token[32];
    const char* text;

    if (static_cast<uint32_t>(rank - 1) <= 17 && (SStrPrintf(token, sizeof(token), "PVP_RANK_%d_%d", rank, faction), (text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE)) != NULL) && *text) {
        lua_pushstring(L, text);
        lua_pushnumber(L, rank >= 5 ? rank - 4 : rank - 5);
    } else {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// PROVEN: 107dbf5a IDENTICAL/NONE
// OFFSET: 0x0080C320/0x1405229B0
static int32_t Script_GetPVPRankProgress(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x004510D0/0x14005F660
static int32_t Script_UnitGroupRolesAssigned(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    CGRaidMember* member = CGRaidInfo::FindMemberByGuid(guid);
    uint32_t role = member ? member->m_groupRole : CGPartyInfo::GetGroupRole(guid);
    lua_pushboolean(L, (role >> 1) & 1);
    lua_pushboolean(L, (role >> 2) & 1);
    lua_pushboolean(L, (role >> 3) & 1);
    return 3;
}

// OFFSET: 0x00457AD0/0x140067640
static int32_t Script_GetRuneType(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_GetRuneType(slot)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(index) > 7 || !CGRuneInfo::IsRuneIndexValid(index)) {
        return 0;
    }

    lua_pushnumber(L, CGRuneInfo::GetRuneTypeByIndex(index, false) + 1);
    return 1;
}

// OFFSET: 0x00452BD0/0x1400616B0
static int32_t Script_GetSpellCritChanceFromIntellect(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetSpellCritChanceFromIntellect(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        lua_pushnumber(L, unit->GetSpellChanceToCrit());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00452D30/0x1400618A0
static int32_t Script_GetSpellBonusDamage(lua_State* L) {
    uint32_t school = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (school > 6) {
        luaL_error(L, "Usage: GetSpellBonusDamage(school)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        int32_t positive = player->GetModDamageDonePos(school);
        lua_pushnumber(L, player->GetModDamageDoneNeg(school) + positive);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00457900/0x140067450
static int32_t Script_GetRuneCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRuneCooldown(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 7) {
        luaL_error(L, "Invalid slot");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    uint32_t lastUsed = CGRuneInfo::GetRuneLastUsedTime(slot);
    float regen = player->m_player->m_playerRuneRegen1[CGRuneInfo::GetRuneTypeByIndex(slot, false)];
    float duration = regen > 0.01f ? 1.0f / regen : 0.0f;
    lua_pushnumber(L, lastUsed * 0.001);
    lua_pushnumber(L, duration);
    lua_pushboolean(L, lastUsed == 0);
    return 3;
}

// OFFSET: 0x004516E0/0x14005FD60
static int32_t Script_UnitIsPVP(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsPVP(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID stateGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    bool pvp;

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        pvp = static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldBytes2[1] & 1;
    } else if (object && (object->m_obj->m_type & TYPEMASK_CORPSE)) {
        pvp = static_cast<CGCorpse_C*>(object)->m_corpse->m_corpseFieldFlags & 4;
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(stateGuid);
        pvp = state && (state->m_flags & 2);
    }

    if (pvp) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00451CB0/0x140060500
static int32_t Script_UnitIsFriend(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsFriend(\"unit\", \"otherUnit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    const char* otherToken = lua_tolstring(L, 2, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    guid = WOWGUID();
    Script_GetGUIDFromToken(otherToken, guid, false, false);
    WOWGUID otherGuid = guid;
    CGUnit_C* other = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if ((unit && other && unit->IsFriendly(other)) || (!SStrCmpI(token, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(otherGuid)) || (!SStrCmpI(otherToken, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(unitGuid))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: cc73a513 IDENTICAL/NONE
// OFFSET: 0x0044C0D0/0x140059710
static void PortraitQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGGameUI::UnitModelUpdate(guid, 3);
    }
}

// OFFSET: 0x00455B80/0x140065020
static int32_t Script_SetPortraitTexture(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetPortraitTexture(texture, \"unit\")");
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), guid, false, false);
    WOWGUID unitGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            SetPortraitTexture(texture, static_cast<CGUnit_C*>(object));
            lua_pushboolean(L, 1);
            return 1;
        }

        if (object->m_obj->m_type & TYPEMASK_CORPSE) {
            CGCorpse_C* corpse = static_cast<CGCorpse_C*>(object);
            const char* portrait = corpse->GetPortraitTexture();

            if (portrait && *portrait) {
                char fileName[260];
                SStrPrintf(fileName, sizeof(fileName), "interface\\icons\\%s", portrait);
                SetPortraitTexture(texture, fileName);
                lua_pushboolean(L, 1);
                return 1;
            }

            WOWGUID requester = WOWGUID();
            NameCache* name = g_nameDBCache.GetRecord(corpse->m_corpse->m_corpseFieldOwner.m_guid, requester, NULL, NULL, false);

            if (name) {
                SetPortraitTexture(texture, name->m_race, name->m_sex, name->m_guid);
                lua_pushboolean(L, 1);
                return 1;
            }
        }
    } else if ((unitGuid.m_guid & 0xF000000000000000ull) || !(unitGuid.m_guid & 0xF07FFFFFFFFFFFFFull)) {
        PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(unitGuid);

        if (petState) {
            SetPortraitTexture(texture, petState->m_displayId, petState->m_guid);
            lua_pushboolean(L, 1);
            return 1;
        }
    } else {
        NameCache* name = g_nameDBCache.GetRecord(unitGuid.m_guid, unitGuid, PortraitQueryCallback, NULL, false);

        if (name) {
            SetPortraitTexture(texture, name->m_race, name->m_sex, name->m_guid);
            lua_pushboolean(L, 1);
            return 1;
        }
    }

    texture->SetTexture("", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x004527F0/0x140061260
static int32_t Script_GetCombatRatingBonus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCombatRatingBonus(ratingIndex)");
        return 0;
    }

    int32_t rating = static_cast<int32_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(rating - 1) >= 25) {
        luaL_error(L, "ratingIndex is in the range 1 .. %d", 25);
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    lua_pushnumber(L, player ? player->GetCombatRatingPct(rating - 1) : 0.0);
    return 1;
}

// OFFSET: 0x00452FE0/0x140061B60
static int32_t Script_GetAttackPowerForStat(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: Script_GetAttackPowerForStat(stat, value)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t stat = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t value = static_cast<int32_t>(lua_tonumber(L, 2));
        lua_pushnumber(L, player->GetAttackPowerContributionFromStat(stat, value));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00454FD0/0x1400641A0
static int32_t Script_UnitDamage(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitDamage(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 7;
    }

    lua_pushnumber(L, unit->m_unit->m_unitFieldMindamage);
    lua_pushnumber(L, unit->m_unit->m_unitFieldMaxdamage);
    lua_pushnumber(L, unit->m_unit->m_unitFieldMinoffhanddamage);
    lua_pushnumber(L, unit->m_unit->m_unitFieldMaxoffhanddamage);

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
        lua_pushnumber(L, player->GetModDamageDonePos(0));
        lua_pushnumber(L, player->GetModDamageDoneNeg(0));
        lua_pushnumber(L, player->GetModDamageDonePct(0));
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
    }

    return 7;
}

// OFFSET: 0x00455170/0x140064390
static int32_t Script_UnitAttackSpeed(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAttackSpeed(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit) {
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnumber(L, unit->m_unit->m_unitFieldBaseattacktime[0] * 0.001);

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        WOWGUID offHand;
        offHand.m_guid = static_cast<CGPlayer_C*>(unit)->m_bag.GetItemGuid(16);
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(offHand, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && item->GetClassID() == 2 && (ClntObjMgrGetActivePlayerObj() != unit || CGPlayer_C::CanDualWield())) {
            lua_pushnumber(L, unit->m_unit->m_unitFieldBaseattacktime[1] * 0.001);
            return 2;
        }
    }

    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x00452380/0x140060DA0
static int32_t Script_UnitSelectionColor(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitSelectionColor(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0;
    CGGameUI::GetSelectionHighlightColor(guid, &color, 0);
    lua_pushnumber(L, color.r * 0.00392156);
    lua_pushnumber(L, color.g * 0.00392156);
    lua_pushnumber(L, color.b * 0.00392156);
    lua_pushnumber(L, color.a * 0.00392156);
    return 4;
}

// OFFSET: 0x00452B50/0x1400615E0
static int32_t Script_GetCritChanceFromAgility(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCritChanceFromAgility(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        lua_pushnumber(L, unit->GetMeleeChanceToCrit());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004552D0/0x140064510
static int32_t Script_UnitAttackPower(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAttackPower(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_UNIT)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 3;
    }

    float multiplier = unit->m_unit->m_unitFieldAttackPowerMultiplier + 1.0f;
    lua_pushnumber(L, multiplier * unit->m_unit->m_unitFieldAttackPower);
    lua_pushnumber(L, (unit->m_unit->m_unitFieldAttackPowerMultiplier + 1.0f) * unit->m_unit->m_unitFieldAttackPowerModPos);
    lua_pushnumber(L, (unit->m_unit->m_unitFieldAttackPowerMultiplier + 1.0f) * unit->m_unit->m_unitFieldAttackPowerModNeg);
    return 3;
}

// OFFSET: 0x00455440/0x1400646B0
static int32_t Script_UnitRangedAttackPower(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAttackPower(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_UNIT)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 3;
    }

    float multiplier = unit->m_unit->m_unitFieldRangedAttackPowerMultiplier + 1.0f;
    lua_pushnumber(L, multiplier * unit->m_unit->m_unitFieldRangedAttackPower);
    lua_pushnumber(L, (unit->m_unit->m_unitFieldRangedAttackPowerMultiplier + 1.0f) * unit->m_unit->m_unitFieldRangedAttackPowerModPos);
    lua_pushnumber(L, (unit->m_unit->m_unitFieldRangedAttackPowerMultiplier + 1.0f) * unit->m_unit->m_unitFieldRangedAttackPowerModNeg);
    return 3;
}

// OFFSET: 0x00452F30/0x140061AD0
static int32_t Script_GetArmorPenetration(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    lua_pushnumber(L, player->GetCombatRatingPct(24) > 100.0f ? 100.0f : player->GetCombatRatingPct(24));
    return 1;
}

// OFFSET: 0x0044C140/0x140059780
static int32_t PushItemStatsTable(lua_State* L, const CGItemStatsSummary* summary) {
    float dps = summary->m_dps;

    if (dps > 0.00000011920929f || dps < -0.00000011920929f) {
        lua_pushstring(L, "ITEM_MOD_DAMAGE_PER_SECOND_SHORT");
        lua_pushnumber(L, dps);
        lua_settable(L, -3);
    }

    for (uint32_t i = 0; i < 89; i++) {
        if (i == 49 || i == 50) {
            continue;
        }

        int32_t value = summary->m_stats[i];
        char buffer[1024];
        const char* name = CGItemStatsSummary::GetStatIntGlobalString(i, buffer, sizeof(buffer));

        if (value) {
            lua_pushstring(L, name);
            lua_pushnumber(L, value);
            lua_settable(L, -3);
        }
    }

    return 1;
}

// OFFSET: 0x0044C200/0x140059870
static int32_t Script_GetItemStats(lua_State* L) {
    if (!lua_isstring(L, 1) || (lua_type(L, 2) > LUA_TNIL && lua_type(L, 2) != LUA_TTABLE)) {
        luaL_error(L, "Usage: GetItemStats(itemLink[, statTable])");
        return 0;
    }

    const char* link = lua_tolstring(L, 1, NULL);
    CGItemStatsSummary summary;
    summary.Clear();

    if (!summary.Create(link)) {
        return 0;
    }

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_settop(L, 2);
    } else {
        lua_createtable(L, 0, 0);
    }

    return PushItemStatsTable(L, &summary);
}

// OFFSET: 0x0044C2D0/0x140059970
static int32_t Script_GetItemStatDelta(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || (lua_type(L, 3) > LUA_TNIL && lua_type(L, 3) != LUA_TTABLE)) {
        luaL_error(L, "Usage: GetItemStatSummary(itemLink1, itemLink2[, statTable])");
        return 0;
    }

    const char* link1 = lua_tolstring(L, 1, NULL);
    const char* link2 = lua_tolstring(L, 2, NULL);
    CGItemStatsSummary first;
    first.Clear();
    CGItemStatsSummary second;
    second.Clear();

    if (!first.Create(link1) || !second.Create(link2)) {
        return 0;
    }

    if (lua_type(L, 3) == LUA_TTABLE) {
        lua_settop(L, 3);
    } else {
        lua_createtable(L, 0, 0);
    }

    CGItemStatsSummary delta = first - second;
    return PushItemStatsTable(L, &delta);
}

// OFFSET: 0x0044E8D0/0x14005CBD0
static int32_t Script_GetPlayerFacing(lua_State* L) {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
    float facing;

    if (target) {
        facing = target->GetFacing();
    } else {
        facing = camera->m_yaw;
    }

    lua_pushnumber(L, facing);
    return 1;
}

// OFFSET: 0x00450E20/0x14005F370
static int32_t Script_UnitIsInMyGuild(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsInMyGuild(\"name\")");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID match = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 3, 1, FLT_MAX, 0);
    CGPlayer_C* target = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(match, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (target && player) {
        uint64_t guild = target->GetGuildGUID().m_guid;

        if (guild && guild == player->GetGuildGUID().m_guid) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    WOWGUID guid = WOWGUID();

    if (Script_GetGUIDFromToken(name, guid, true, false)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            name = unit->GetUnitName(NULL, 1);
        } else {
            WOWGUID none = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

            if (record) {
                name = record->m_name;
            }
        }
    }

    if (name) {
        for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
            GuildMemberInfo* member = i < CGGuildInfo::m_numMembers ? CGGuildInfo::m_members.m_data[i] : NULL;

            if (!SStrCmpUTF8I(name, member->m_name, STORM_MAX_STR)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00451150/0x14005F710
static int32_t Script_UnitIsRaidOfficer(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    CGRaidMember* member = CGRaidInfo::FindMemberByGuid(guid);

    if (member && member->m_rank > 0) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00451250/0x14005F840
static int32_t Script_UnitPlayerOrPetInParty(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID check = guid;

    if (CGPartyInfo::IsPlayerOrPetInParty(check)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004512D0/0x14005F8C0
static int32_t Script_UnitInRaid(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    int32_t index = CGRaidInfo::GetMemberIndexByGuid(guid);

    if (index < 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, index);
    }

    return 1;
}

// OFFSET: 0x004513D0/0x14005FA20
static int32_t Script_UnitPlayerOrPetInRaid(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID check = guid;

    if (CGRaidInfo::IsPlayerOrPetInRaid(check)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004532F0/0x140061E90
static int32_t Script_UnitGUID(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitGUID(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID display = guid;

    if (!guid || guid.m_guid == 0xFFFFFFFFFFFFFFFEULL) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0300000 || (high & 0xF0F00000) == 0xF0500000) {
        CreatureStats_C* creature = g_creatureDBCache.GetRecord((guid.m_guid >> 24) & 0xFFFFFFF, display, NULL, NULL, false);

        if (creature && (creature->m_typeFlags[0] & 0x4000)) {
            display.m_guid &= 0xFFFFFFFFFF000000ULL;
        }
    }

    char buffer[68];
    lua_pushstring(L, GUIDToString(display, buffer));
    return 1;
}

// OFFSET: 0x00454A50/0x1400639B0
static int32_t Script_UnitClassBase(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitClassBase(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    ChrClassesRec* rec;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        rec = g_chrClassesDB.GetRecord(ClientServices::GetCharacterClass());
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            rec = g_chrClassesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[1]);
        } else {
            WOWGUID none = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

            if (!record) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            rec = g_chrClassesDB.GetRecord(record->m_class);
        }
    }

    if (rec) {
        lua_pushstring(L, rec->m_name);
        lua_pushstring(L, rec->m_filename);
        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x0045B2E0/0x14006BC70
static int32_t Script_UnitRangedDamage(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitRangedDamage(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 6;
    }

    lua_pushnumber(L, unit->m_unit->m_unitFieldRangedattacktime * 0.001);
    lua_pushnumber(L, unit->m_unit->m_unitFieldMinrangeddamage);
    lua_pushnumber(L, unit->m_unit->m_unitFieldMaxrangeddamage);

    if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        return 6;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
    uint32_t school = 0;
    const VisibleItem_C* item = player->GetVisibleItem(17);

    if (item) {
        WOWGUID none = WOWGUID();
        ItemStats_C* stats = g_itemDBCache.GetRecord(abs(item->m_itemId), none, NULL, NULL, false);

        if (stats) {
            for (uint32_t i = 0; i < 2; i++) {
                if (stats->m_damageMin[i] > 0.0f) {
                    school = stats->m_damageType[i];
                    break;
                }
            }
        }
    }

    uint32_t positive = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerFieldModDamageDonePos[school] : 0;
    lua_pushnumber(L, positive);
    uint32_t negative = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerFieldModDamageDoneNeg[school] : 0;
    lua_pushnumber(L, negative);

    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        lua_pushnumber(L, *reinterpret_cast<float*>(&player->m_player->m_playerFieldModDamageDonePct[school]));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 6;
}

// OFFSET: 0x00454F40/0x1400640C0
static int32_t Script_UnitRangedAttack(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitRangedAttack(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    int32_t bonus = 0;
    int32_t rank = 0;

    if (unit) {
        unit->GetRangedAttackSkillRank(rank, bonus);
    }

    lua_pushnumber(L, rank);
    lua_pushnumber(L, bonus);
    return 2;
}

// OFFSET: 0x004560B0/0x140065650
static int32_t Script_GetResSicknessDuration(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    uint32_t level = 0;
    int32_t spellId = 15007;

    if (player) {
        ChrRacesRec* race = g_chrRacesDB.GetRecord(player->m_unit->m_unitFieldBytes0[0]);
        level = player->m_unit->m_unitFieldLevel;

        if (race) {
            spellId = race->m_resSicknessSpellID;
        }
    }

    SpellRec* spell;
    SpellDurationRec* duration;

    if ((spell = g_spellDB.GetRecord(spellId)) && (duration = g_spellDurationDB.GetRecord(spell->m_durationIndex)) != NULL) {
        int32_t value = duration->m_duration + level * duration->m_durationPerLevel;

        if (value >= duration->m_maxDuration) {
            value = duration->m_maxDuration;
        }

        Spell_C_ModifySpellValue(spell, value, 1);

        if (value >= 1000) {
            char buffer[256];
            CGTooltip::GetDurationString(buffer, sizeof(buffer), value, "INT_GENERAL_DURATION", NULL, 1, 0);
            lua_pushstring(L, buffer);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004569B0/0x1400661E0
static int32_t Script_IsFlyableArea(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        if (player->IsInFlyableZone(1)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    if (IsInFlyableArea()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00456EE0/0x140066810
static int32_t Script_UnitIsSameServer(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsSameServer(\"unit\", \"otherUnit\")");
        return 0;
    }

    const char* first = lua_tolstring(L, 1, NULL);
    const char* second = lua_tolstring(L, 2, NULL);
    WOWGUID firstGuid;
    WOWGUID secondGuid;

    if (!SStrCmpI(first, second, STORM_MAX_STR) || (firstGuid = GetGuidByKeyword(first), secondGuid = GetGuidByKeyword(second), firstGuid == secondGuid)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    WOWGUID none = WOWGUID();
    NameCache* firstName = g_nameDBCache.GetRecord(firstGuid.m_guid, none, NULL, NULL, false);
    WOWGUID none2 = WOWGUID();
    NameCache* secondName = g_nameDBCache.GetRecord(secondGuid.m_guid, none2, NULL, NULL, false);

    if ((firstName || (secondName && secondName->m_realmName[0])) && (secondName || firstName->m_realmName[0]) && (!firstName || !secondName || SStrCmp(firstName->m_realmName, secondName->m_realmName, STORM_MAX_STR))) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00457070/0x1400669F0
static int32_t Script_GetUnitHealthModifier(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitHealthModifier(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    double modifier;

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        modifier = unit->GetHealthModifier();
    } else {
        modifier = 0.0;
    }

    lua_pushnumber(L, modifier);
    return 1;
}

// OFFSET: 0x00457180/0x140066BA0
static int32_t Script_GetUnitPowerModifier(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitPowerModifier(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));
    double modifier;

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        modifier = unit->GetPowerModifier();
    } else {
        modifier = 0.0;
    }

    lua_pushnumber(L, modifier);
    return 1;
}

// OFFSET: 0x004575B0/0x140067080
static int32_t Script_ReportPlayerIsPVPAFK(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ReportPlayerIsPVPAFK(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID target = guid;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && player->m_obj->m_guid != target && (CGPartyInfo::IsMember(target) || IsGuidInMyRaid(target))) {
        CGPlayer_C::ReportPVPAFK(target);
    }

    return 0;
}

// OFFSET: 0x00457680/0x140067150
static int32_t Script_PlayerIsPVPInactive(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PlayerIsPVPInactive(\"unit\")");
        return 0;
    }

    int32_t inactive = 0;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);
    WOWGUID target = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->HasAura(43681, WOWGUID())) {
            inactive = 1;
        }
    } else {
        TSGrowableArray<PartyMemberAuraStates>* auras = NULL;
        int32_t count = 0;

        if (PartyMemberState* state = CGGameUI::GetPartyMemberState(target)) {
            if (state->m_flags & 0x1) {
                count = state->m_numAuras;
                auras = &state->m_auras;
            }
        } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(target)) {
            count = petState->m_numAuras;
            auras = &petState->m_auras;
        }

        for (int32_t i = 0; i < count; i++) {
            if (auras->m_data[i].m_spellId == 43681) {
                inactive = 1;
                break;
            }
        }
    }

    lua_pushboolean(L, inactive);
    return 1;
}

// OFFSET: 0x004577A0/0x140067290
static int32_t Script_UnitInRange(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitInRange(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit) {
        WOWGUID guid = unit->m_obj->m_guid;

        if (CGGameUI::IsRaidMemberOrPet(guid)) {
            CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (mover) {
                C3Vector unitPosition = unit->GetPosition();
                C3Vector moverPosition = mover->GetPosition();
                float dy = moverPosition.y - unitPosition.y;
                float dz = moverPosition.z - unitPosition.z;
                float dx = moverPosition.x - unitPosition.x;

                if (sqrt(dx * dx + dz * dz + dy * dy) <= 40.0) {
                    lua_pushnumber(L, 1.0);
                    return 1;
                }
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00458070/0x140067CB0
static int32_t Script_UnitInVehicleControlSeat(lua_State* L) {
    VehicleSeatRec* seat = GetVehicleSeatRecAsPassengerFromLua(L);

    if (seat) {
        lua_pushboolean(L, seat->m_flags & 0x800);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x00458140/0x140067DA0
static int32_t Script_UnitTargetsVehicleInRaidUI(lua_State* L) {
    VehicleSeatRec* seat = GetVehicleSeatRecAsPassengerFromLua(L);

    if (seat) {
        lua_pushboolean(L, seat->m_flagsB & 0x8);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x00458190/0x140067DF0
static int32_t Script_UnitVehicleSkin(lua_State* L) {
    VehicleSeatRec* seat = GetVehicleSeatRecAsPassengerFromLua(L);

    if (!seat) {
        return 0;
    }

    uint32_t skin = seat->m_uiSkin;

    if (skin >= 2) {
        lua_pushstring(L, "");
    } else {
        lua_pushstring(L, s_vehicleSkinNames[skin]);
    }

    return 1;
}

// OFFSET: 0x004588E0/0x140068690
static int32_t Script_EjectPassengerFromSeat(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EjectPassengerFromSeat(seatIndex)");
        return 0;
    }

    int32_t seatIndex = static_cast<int32_t>(lua_tonumber(L, 1));
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGUnit_C* root = mover->FindRootVehicle(NULL);

    if (!root || !root->m_vehicle || !root->m_vehicle->m_vehicleRec) {
        return 0;
    }

    const CVehicle_C* vehicle;
    uint8_t seat;

    if (root->m_vehicle->GetVehicleAndSeatFromVirtualSeat(seatIndex - 1, vehicle, seat)) {
        CGUnit_C* passenger = const_cast<CVehicle_C*>(vehicle)->FindPassengerUnit(seat);

        if (passenger) {
            CVehicle_C::EjectPassenger(passenger->m_obj->m_guid);
        }
    }

    return 0;
}

// OFFSET: 0x004589A0/0x140068750
static int32_t Script_RespondInstanceLock(lua_State* L) {
    int32_t accept = StringToBOOL(L, 1, 0);
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_INSTANCE_LOCK_RESPONSE));
    msg.Put(static_cast<uint8_t>(accept));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00458A60/0x140068850
static int32_t Script_GetPlayerInfoByGUID(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetPlayerInfoByGUID(\"playerGUID\")");
        return 0;
    }

    WOWGUID guid = StringToGUID(lua_tolstring(L, 1, NULL));

    if (!guid) {
        luaL_error(L, "Usage: GetPlayerInfoByGUID(\"playerGUID\")");
        return 0;
    }

    if (guid.m_guid & 0xF000000000000000ULL) {
        return 0;
    }

    if (!(guid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    NameCache* record;

    if (player && (player->m_player->m_playerFlags & 0x80000)) {
        record = g_nameDBCache.GetRecord(guid.m_guid, guid, &NameQueryCallback, NULL, true);
    } else {
        WOWGUID none = WOWGUID();
        record = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);
    }

    if (!record) {
        return 0;
    }

    const char* className = CGUnit_C::GetDisplayClassName(NULL, record);
    ChrClassesRec* classRec = g_chrClassesDB.GetRecord(record->m_class);
    lua_pushstring(L, className);

    if (classRec) {
        lua_pushstring(L, classRec->m_filename);
    } else {
        lua_pushnil(L);
    }

    const char* raceName = CGUnit_C::GetDisplayRaceName(NULL, record);
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(record->m_race);
    lua_pushstring(L, raceName);

    if (raceRec) {
        lua_pushstring(L, raceRec->m_clientFileString);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, s_unitSexValues[record->m_sex]);
    lua_pushstring(L, record->m_name);
    lua_pushstring(L, record->m_realmName);
    return 7;
}

// OFFSET: #0x00614D90
static void CacheVehicleUIIndicatorRec(int32_t id) {
    VehicleUIIndicatorRec* indicator = g_vehicleUIIndicatorDB.GetRecord(id);

    if (!indicator) {
        s_vehicleUIIndicatorID = 0;
        s_vehicleUIIndicatorSeats.m_count = 0;
        return;
    }

    s_vehicleUIIndicatorID = indicator->m_id;
    s_vehicleUIIndicatorTexture = indicator->m_backgroundTexture;
    s_vehicleUIIndicatorSeats.m_count = 0;

    for (int32_t i = 1; i < g_vehicleUIIndSeatDB.m_maxID + 1; i++) {
        VehicleUIIndSeatRec* seat = g_vehicleUIIndSeatDB.GetRecord(i);

        if (seat && seat->m_vehicleUiindicatorID == id) {
            *s_vehicleUIIndicatorSeats.New() = seat;
        }
    }
}

// OFFSET: 0x0045A940/0x14006AFC0
static int32_t Script_GetVehicleUIIndicator(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetVehicleUIIndicator(indicatorID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

    if (s_vehicleUIIndicatorID != id) {
        CacheVehicleUIIndicatorRec(id);
    }

    if (s_vehicleUIIndicatorID <= 0) {
        return 0;
    }

    lua_pushstring(L, s_vehicleUIIndicatorTexture);
    lua_pushnumber(L, s_vehicleUIIndicatorSeats.m_count);
    return 2;
}

// OFFSET: 0x0045A9E0/0x14006B070
static int32_t Script_GetVehicleUIIndicatorSeat(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetVehicleUIIndicatorSeat(indicatorID, indicatorSeatIndex)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (s_vehicleUIIndicatorID != id) {
        CacheVehicleUIIndicatorRec(id);
    }

    if (index >= s_vehicleUIIndicatorSeats.m_count) {
        return 0;
    }

    lua_pushnumber(L, s_vehicleUIIndicatorSeats.m_data[index]->m_virtualSeatIndex);
    lua_pushnumber(L, s_vehicleUIIndicatorSeats.m_data[index]->m_xpos);
    lua_pushnumber(L, s_vehicleUIIndicatorSeats.m_data[index]->m_ypos);
    return 3;
}

// OFFSET: 0x00458280/0x140067F00
static int32_t Script_UnitVehicleSeatInfo(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit = NULL;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false)) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    int32_t seat = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (!unit) {
        return 0;
    }

    CGUnit_C* root = unit->FindRootVehicle(NULL);

    if (!root) {
        return 0;
    }

    CVehicle_C* vehicle = root->m_vehicle;
    const char* type;
    const char* occupant;
    const char* realm;
    int32_t canEject;
    int32_t canSwitch;

    if (!vehicle || !vehicle->m_vehicleRec || !vehicle->GetVirtualSeatInfo(seat, type, occupant, realm, canEject, canSwitch)) {
        return 0;
    }

    lua_pushstring(L, type);
    lua_pushstring(L, occupant);
    lua_pushstring(L, realm);
    lua_pushboolean(L, canEject);
    lua_pushboolean(L, canSwitch);
    return 5;
}

// OFFSET: 0x00458370/0x140068030
static int32_t Script_UnitSwitchToVehicleSeat(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit = NULL;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false)) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    double seat = lua_tonumber(L, 2);

    if (!unit || unit->m_obj->m_guid != ClntObjMgrGetActivePlayer() || !unit->m_vehiclePassenger) {
        return 0;
    }

    CGUnit_C* vehicle = unit->m_vehiclePassenger->GetVehicleUnit();

    if (!vehicle) {
        return 0;
    }

    CGUnit_C* root = vehicle->FindRootVehicle(NULL);

    if (root) {
        unit->VehiclePassengerRequestSwitchToVirtualSeat(root, static_cast<int32_t>(seat - 1.0));
    }

    return 0;
}

// OFFSET: 0x0044BCD0/0x1400592F0
static int32_t Script_GetSessionTime(lua_State* L) {
    lua_pushnumber(L, FrameTime::GetSessionTimeMs() * 0.001);
    return 1;
}

// OFFSET: 0x0044BE30/0x140059430
static int32_t Script_ResistancePercent(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ResistancePercent(resistance, casterLevel)");
    }

    float resistance = lua_tonumber(L, 1);
    int32_t level = lua_tointeger(L, 2);
    float total;

    if (level < 20) {
        level = 20;
        total = static_cast<float>(level) * 2.5f + resistance;
    } else if (level <= 60) {
        total = static_cast<float>(level) * 2.5f + resistance;
    } else {
        total = static_cast<float>(level * level) + resistance - static_cast<float>(level) * 127.5f + 4200.0f;
    }

    if (total != 0.0f) {
        resistance = resistance / total;
    }

    if (resistance > 0.75f) {
        resistance = 0.75f;
    }

    lua_pushnumber(L, resistance * 100.0f);
    return 1;
}

// OFFSET: 0x0044C3F0/0x140059AE0
static int32_t Script_GetMaxPlayerLevel(lua_State* L) {
    lua_pushnumber(L, 85.0);
    return 1;
}

// OFFSET: 0x0044E670/0x14005C8B0
static int32_t PushPowerBarInfo(lua_State* L, int32_t barID) {
    const UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(barID);

    if (!bar) {
        return 0;
    }

    lua_pushnumber(L, bar->m_barType);
    lua_pushnumber(L, bar->m_minPower);
    lua_pushnumber(L, bar->unk_0064);
    lua_pushnumber(L, bar->unk_0068);
    lua_pushboolean(L, bar->m_flags & 2);
    lua_pushboolean(L, bar->m_flags & 4);
    lua_pushboolean(L, bar->m_flags & 8);
    lua_pushboolean(L, bar->m_flags & 0x20);
    lua_pushboolean(L, bar->m_flags & 0x40);
    lua_pushstring(L, bar->unk_0054);
    lua_pushstring(L, bar->unk_0060);
    lua_pushstring(L, bar->m_costString);
    return 12;
}

// OFFSET: 0x0044E740/0x14005CA00
static int32_t Script_GetAlternatePowerInfoByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAlternatePowerInfoByID(id)");
    }

    return PushPowerBarInfo(L, lua_tointeger(L, 1));
}

// OFFSET: 0x0044E780/0x14005CA50
static int32_t Script_SetAchievementComparisonPortrait(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    int32_t failed = 1;

    uint32_t inspectHigh = static_cast<uint32_t>(CGAchievementInfo::m_inspect.m_guid >> 32);

    if (!(inspectHigh & 0xF0000000) && ((inspectHigh & 0xF07FFFFF) | static_cast<uint32_t>(CGAchievementInfo::m_inspect.m_guid))) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGAchievementInfo::m_inspect, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            SetPortraitTexture(texture, unit);
            failed = 0;
        } else {
            NameCache* name = g_nameDBCache.GetRecord(CGAchievementInfo::m_inspect.m_guid, CGAchievementInfo::m_inspect, PortraitQueryCallback, NULL, false);

            if (name) {
                SetPortraitTexture(texture, name->m_race, name->m_sex, name->m_guid);
                failed = 0;
            } else {
                texture->SetTexture("", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
            }
        }
    } else {
        texture->SetTexture("", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    }

    lua_pushboolean(L, failed == 0);
    return 1;
}

// OFFSET: 0x0044EA30/0x14005CD50
static int32_t Script_InitiateRolePoll(lua_State* L) {
    static ThrottleObject s_throttle(2, 10);

    if (!s_throttle.IncrementAndCheck()) {
        return 0;
    }

    WOWGUID player = ClntObjMgrGetActivePlayer();

    if (!CGPartyInfo::NumMembers() && !CGRaidInfo::m_numMembers) {
        return 0;
    }

    if (!CGPartyInfo::m_leader && !CGRaidInfo::IsPlayerRaidOfficer(player)) {
        return 0;
    }

    UserClientInitiateRolePoll msg;
    ClientServices::Send(&msg);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x0044EAF0/0x14005CE20
static int32_t Script_LoadURLIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LoadURLIndex(index)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    static ThrottleObject s_throttle(3, 5);

    if (!s_throttle.IncrementAndCheck()) {
        return 0;
    }

    char url[1024];
    memset(url, 0, sizeof(url));

    if (index == 1) {
        Login* login = ClientServices::LoginConnection();
        const char* account = login ? login->GetAccountName() : ClientServices::GetAccountName();
        const char* format = GetVisitableURL(1);
        WOWGUID player = ClntObjMgrGetActivePlayer();
        SStrPrintf(url, sizeof(url), format, account, CGGameUI::s_cfgRealmRecID, static_cast<uint32_t>(player.m_guid >> 32) & 0xF07FFFFF, static_cast<uint32_t>(player.m_guid));
    } else if (index == 3) {
        SStrPrintf(url, sizeof(url), GetVisitableURL(3));
    } else {
        FormatVisitableURL(index, url, sizeof(url));
    }

    if (url[0]) {
        ClientServices::LaunchURL(url);
    }

    return 0;
}

// OFFSET: 0x004522F0/0x140060CD0
static int32_t Script_UnitIsQuestBoss(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsQuestBoss(\"unit\")");
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));
    CreatureStats_C* stats;

    if (unit && (stats = unit->m_creatureCacheEntry) != NULL && !unit->m_unit->m_unitFieldPetnumber && static_cast<int32_t>(stats->m_typeFlags[0]) < 0) {
        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x00452A50/0x1400614E0
static int32_t Script_IsDualWielding(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGBag_C* bag;
    CGItem_C* item;

    if (player && (bag = player->GetBag()) != NULL && (item = bag->GetItemPointer(16)) != NULL && item->GetClassID() == 2 && CGPlayer_C::CanDualWield()) {
        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x004530B0/0x140061C40
static int32_t Script_GetMeleeHaste(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    float haste = player->m_player->m_playerFieldModHaste;

    if (haste != 0.0f) {
        haste = 1.0f / haste * 100.0f - 100.0f;
    }

    lua_pushnumber(L, haste);
    return 1;
}

// OFFSET: 0x00453150/0x140061CE0
static int32_t Script_GetRangedHaste(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    float haste = player->m_player->m_playerFieldModRangedHaste;

    if (haste != 0.0f) {
        haste = 1.0f / haste * 100.0f - 100.0f;
    }

    lua_pushnumber(L, haste);
    return 1;
}

// OFFSET: 0x004531F0/0x140061D80
static int32_t Script_GetPetMeleeHaste(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    float haste = player->m_player->m_playerFieldModPetHaste;

    if (haste != 0.0f) {
        haste = 1.0f / haste * 100.0f - 100.0f;
    }

    lua_pushnumber(L, haste);
    return 1;
}

// OFFSET: 0x00453290/0x140061E20
static int32_t Script_GetOverrideSpellPowerByAP(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->HasAuraEffect(366)) {
        return 0;
    }

    lua_pushnumber(L, player->m_player->m_playerFieldOverrideSpellPowerByApPct * 0.01f);
    return 1;
}

// OFFSET: 0x004545A0/0x140063430
static int32_t Script_UnitInPhase(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitInPhase(\"unit\")");
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tostring(L, 1), guid, false, false);
    WOWGUID member = guid;
    PartyMemberState* state;

    if (ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__) || ((state = CGGameUI::GetPartyMemberState(member)) != NULL && PhaseShift_C_HasSharedPhase(&state->m_phaseShift))) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004555B0/0x140064850
static int32_t Script_UnitSpellHaste(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitSpellHaste(\"unit\")");
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));
    float haste;

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        float value = unit->m_unit->m_unitModCastHaste;
        haste = value;

        if (value != 0.0f) {
            haste = 1.0f / value * 100.0f - 100.0f;
        }
    } else {
        haste = 0.0f;
    }

    lua_pushnumber(L, haste);
    return 1;
}

// OFFSET: 0x00456CA0/0x140066540
static int32_t Script_IsSubmerged(lua_State* L) {
    CGUnit_C* unit;

    if (lua_isstring(L, 1)) {
        unit = Script_GetUnitFromName(lua_tostring(L, 1));
    } else {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    }

    if (unit && (unit->m_clientFlags & 0x200000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00457550/0x140067010
static int32_t Script_GetMastery(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerMastery);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004580F0/0x140067D50
static int32_t Script_UnitHasVehiclePlayerFrameUI(lua_State* L) {
    VehicleSeatRec* seat = GetVehicleSeatRecAsPassengerFromLua(L);

    if (seat) {
        lua_pushboolean(L, seat->m_flagsB >= 0);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x00458660/0x1400683A0
static int32_t Script_UnitThreatPercentageOfLead(lua_State* L) {
    WOWGUID unitGUID;
    WOWGUID mobGUID;

    if (!Script_GetGUIDFromToken(lua_tostring(L, 1), unitGUID, false, false) || !Script_GetGUIDFromToken(lua_tostring(L, 2), mobGUID, false, false)) {
        luaL_error(L, "Usage: UnitThreatLeadPercentage(\"unit\" [, \"mob\"])");
    }

    CGUnit_C* mob = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(mobGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mob) {
        return 0;
    }

    uint8_t status;
    uint8_t rawPercent;
    float percent;
    int32_t threat;
    mob->CalculateThreat(unitGUID, &status, &rawPercent, &percent, &threat);
    int32_t lead = mob->GetHighestThreatExcluding(unitGUID);

    if (lead) {
        lua_pushnumber(L, threat * 100.0 / lead);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 0;
}

// OFFSET: 0x00458C60/0x140068B20
static int32_t Script_UnitGetGuildXP(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitGetGuildXP(\"unit\")");
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushboolean(L, 0);
        return 7;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
    lua_pushnumber(L, static_cast<double>(player->GetGuildXP()));
    lua_pushnumber(L, static_cast<double>(player->GetGuildRemainingXP()));
    lua_pushnumber(L, static_cast<double>(player->GetGuildDailyXP()));
    lua_pushnumber(L, static_cast<double>(player->GetGuildMaxDailyXP()));
    lua_pushnumber(L, static_cast<double>(player->GetGuildPlayerWeeklyXP()));
    lua_pushnumber(L, static_cast<double>(player->GetGuildPlayerTotalXP()));
    lua_pushboolean(L, player->m_player->m_playerGuildlevel >= 20);
    return 7;
}

// OFFSET: 0x00458DF0/0x140068CF0
static int32_t Script_UnitGetGuildLevel(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitGetGuildLevel(\"unit\")");
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));
    double level;

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        level = static_cast<int32_t>(static_cast<CGPlayer_C*>(unit)->m_player->m_playerGuildlevel);
    } else {
        level = 0.0;
    }

    lua_pushnumber(L, level);
    return 1;
}

// OFFSET: 0x00458E70/0x140068DB0
static int32_t Script_UnitGetIncomingHeals(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitGetIncomingHeals(\"unit\"[, \"healer\"]");
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit || !player || !player->IsFriendly(unit)) {
        return 0;
    }

    int32_t heals;

    if (lua_isstring(L, 2)) {
        WOWGUID healer = Script_GetGUIDFromName(lua_tostring(L, 2));
        heals = unit->GetIncomingHealsFrom(healer);
    } else {
        heals = unit->GetIncomingHeals();
    }

    lua_pushnumber(L, heals);
    return 1;
}

// OFFSET: 0x00458F50/0x140068F00
static int32_t Script_UnitSetRole(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitSetRole(\"unit\", \"role\"");
    }

    uint32_t role = 0;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tostring(L, 1), guid, false, false);

    if (!guid) {
        return 0;
    }

    if (lua_isstring(L, 2)) {
        role = GetRoleFlagFromToken(lua_tostring(L, 2));
    }

    WOWGUID player = ClntObjMgrGetActivePlayer();
    bool officer = CGPartyInfo::m_leader == player || CGRaidInfo::IsPlayerRaidOfficer(player);

    if ((!CGPartyInfo::NumMembers() && !CGRaidInfo::m_numMembers) || (guid != player && !officer)) {
        return 0;
    }

    static ThrottleObject s_throttle(2, 10);

    if (officer) {
        s_throttle.m_limit = 45;
        s_throttle.m_period = 60;
    } else {
        s_throttle.m_limit = 2;
        s_throttle.m_period = 10;
    }

    if (!s_throttle.IncrementAndCheck()) {
        return 0;
    }

    UserClientSetRole msg;
    memcpy(msg.m_targetGUID, &guid, sizeof(msg.m_targetGUID));
    msg.m_role = role;
    ClientServices::Send(&msg);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x004590E0/0x1400690B0
static int32_t Script_GetHitModifier(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushnumber(L, player->m_player->m_playerFieldUiHitModifier);
    return 1;
}

// OFFSET: 0x00459130/0x140069110
static int32_t Script_GetSpellHitModifier(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushnumber(L, player->m_player->m_playerFieldUiSpellHitModifier);
    return 1;
}

// OFFSET: 0x00459180/0x140069170
static int32_t Script_GetEclipseDirection(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (player->HasAura(67483, WOWGUID())) {
        lua_pushstring(L, "sun");
    } else if (player->HasAura(67484, WOWGUID())) {
        lua_pushstring(L, "moon");
    } else {
        lua_pushstring(L, "none");
    }

    return 1;
}

// OFFSET: 0x00459220/0x140069210
static int32_t Script_UnitHPPerStamina(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHPPerStamina(\"unit\")");
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tostring(L, 1));

    if (!unit) {
        return 0;
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        lua_pushnumber(L, GameFormulaEvaluateTable(9, unit->m_unit->m_unitFieldLevel - 1, 0));
    } else {
        lua_pushnumber(L, 10.0);
    }

    return 1;
}

// OFFSET: 0x00459950/0x1400699D0
static int32_t GetUnitPowerBarID(const char* token, int32_t timerIndex) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);

    if (!guid) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    if (timerIndex < 0) {
        return unit->m_powerBarID;
    }

    if (!unit->HasAuraEffect(369)) {
        return 0;
    }

    int32_t index = 0;

    for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
        AuraData* aura = unit->GetAura(i);
        const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 369 && (aura->m_flags & (1 << j))) {
                if (index == timerIndex) {
                    return effects->m_data[j]->m_effectMiscValue;
                }

                index++;
            }
        }
    }

    return 0;
}

// OFFSET: 0x00459AB0/0x140069B70
static int32_t Script_UnitAlternatePowerInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAlternatePowerInfo(\"unit\")");
    }

    return PushPowerBarInfo(L, GetUnitPowerBarID(lua_tostring(L, 1), -1));
}

// OFFSET: 0x00459B00/0x140069C10
static int32_t Script_UnitAlternatePowerCounterInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAlternatePowerInfo(\"unit\")");
    }

    const UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(GetUnitPowerBarID(lua_tostring(L, 1), -1));

    if (!bar) {
        return 0;
    }

    lua_pushboolean(L, bar->m_flags & 0x80);
    lua_pushboolean(L, bar->m_flags & 0x100);
    return 2;
}

// OFFSET: 0x00459BA0/0x140069D00
static int32_t Script_UnitAlternatePowerTextureInfo(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UnitAlternatePowerTextureInfo(\"unit\", textureIndex, [timerIndex])");
    }

    int32_t timerIndex = -1;

    if (lua_isnumber(L, 3)) {
        timerIndex = lua_tointeger(L, 3) - 1;
    }

    int32_t barID = GetUnitPowerBarID(lua_tostring(L, 1), timerIndex);

    if (!barID) {
        return 0;
    }

    const UnitPowerBarRec* bar = g_unitPowerBarDB.GetRecord(barID);
    int32_t textureIndex = lua_tointeger(L, 2);

    if (textureIndex > 6) {
        luaL_error(L, "Invalid texture index");
    }

    int32_t fileDataID = bar->m_fileDataID[textureIndex];

    if (!fileDataID) {
        lua_pushnil(L);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        return 5;
    }

    const FileDataRec* fileData = g_fileDataDB.GetRecord(fileDataID);
    char texture[260];
    SStrCopy(texture, fileData->m_filepath, sizeof(texture));
    SStrPack(texture, fileData->m_filename, sizeof(texture));
    lua_pushstring(L, texture);
    uint32_t color = bar->m_color[textureIndex];
    lua_pushnumber(L, ((color >> 16) & 0xFF) * 0.0039215689);
    lua_pushnumber(L, ((color >> 8) & 0xFF) * 0.0039215689);
    lua_pushnumber(L, (color & 0xFF) * 0.0039215689);
    lua_pushnumber(L, (color >> 24) * 0.0039215689);
    return 5;
}

// OFFSET: 0x00459DF0/0x14006A020
static int32_t Script_UnitPowerBarTimerInfo(lua_State* L) {
    const char* token = lua_tostring(L, 1);
    CGUnit_C* unit = NULL;
    WOWGUID guid;

    if (Script_GetGUIDFromToken(token, guid, false, false)) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    int32_t timerIndex = 0;

    if (lua_isnumber(L, 2)) {
        timerIndex = lua_tointeger(L, 2) - 1;
    }

    int32_t index = 0;

    if (!unit || !unit->HasAuraEffect(369)) {
        return 0;
    }

    for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
        AuraData* aura = unit->GetAura(i);
        const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect || effect->m_effectAura != 369 || !(aura->m_flags & (1 << j))) {
                continue;
            }

            if (index != timerIndex) {
                index++;
                continue;
            }

            lua_pushnumber(L, aura->m_duration * 0.001);
            lua_pushnumber(L, static_cast<uint32_t>(aura->m_endTime) * 0.001);
            lua_pushinteger(L, effects->m_data[j]->m_effectMiscValue);
            lua_pushinteger(L, aura->m_spellId);
            return 4;
        }
    }

    return 0;
}

// OFFSET: 0x00459FD0/0x14006A230
static int32_t Script_UnitNumPowerBarTimers(lua_State* L) {
    const char* token = lua_tostring(L, 1);
    CGUnit_C* unit = NULL;
    WOWGUID guid;

    if (Script_GetGUIDFromToken(token, guid, false, false)) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    int32_t timers = 0;

    if (unit && unit->HasAuraEffect(369)) {
        for (uint32_t i = 0; i < unit->GetAuraCount(); i++) {
            AuraData* aura = unit->GetAura(i);
            const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

            if (!spell) {
                continue;
            }

            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

            for (uint32_t j = 0; j < count; j++) {
                const SpellEffectRec* effect = effects->m_data[j];

                if (effect && effect->m_effectAura == 369 && (aura->m_flags & (1 << j))) {
                    timers++;
                }
            }
        }
    }

    lua_pushnumber(L, timers);
    return 1;
}

// OFFSET: 0x00807310/0x140631100
static int32_t Script_IsGMClient(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

// STATIC: 0x00C99510
static FrameScript_Method s_SystemFunctions[] = {
    { "GetTime", &Script_GetTime },
    { "GetGameTime", &Script_GetGameTime },
    { "ConsoleExec", &Script_ConsoleExec },
    { "ReadFile", &Script_ReadFile },
    { "DeleteFile", &Script_DeleteFile },
    { "AppendToFile", &Script_AppendToFile },
    { "GetSessionTime", &Script_GetSessionTime },
};

// PROVEN: 130561a8 IDENTICAL/NONE
// OFFSET: 0x0044E0F0/0x14005C340
void SystemRegisterFunctions() {
    for (uint32_t i = 0; i < sizeof(s_SystemFunctions) / sizeof(s_SystemFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_SystemFunctions[i].m_name, s_SystemFunctions[i].m_method);
    }
}

// PROVEN: 88e5031e IDENTICAL/NONE
// OFFSET: 0x0044E120/0x14005C390
void SystemUnregisterFunctions() {
    for (uint32_t i = 0; i < sizeof(s_SystemFunctions) / sizeof(s_SystemFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_SystemFunctions[i].m_name);
    }
}

// OFFSET: 0x00457200/0x140066C70
static int32_t Script_GetUnitManaRegenRateFromSpirit(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitManaRegenRateFromSpirit(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = Script_GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        lua_pushnumber(L, unit->GetManaRegenRateFromSpirit() + 0.001);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004514E0/0x14005FB30
static int32_t Script_UnitIsAFK(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsAFK(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    WOWGUID unitGuid = guid;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
    bool afk;

    if (player) {
        CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!activePlayer || activePlayer->CanAttack(player)) {
            lua_pushnil(L);
            return 1;
        }

        if (player->IsActivePlayer() && CGChat::m_afk) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        afk = (player->m_player->m_playerFlags & 0x2) != 0;
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(unitGuid);

        if (!state) {
            lua_pushnil(L);
            return 1;
        }

        afk = (state->m_flags & 0x40) != 0;
    }

    if (afk) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00C99560
static FrameScript_Method s_UnitFunctions[] = {
    { "UnitExists", &Script_UnitExists },
    { "UnitIsVisible", &Script_UnitIsVisible },
    { "UnitIsUnit", &Script_UnitIsUnit },
    { "UnitIsPlayer", &Script_UnitIsPlayer },
    { "UnitIsInMyGuild", &Script_UnitIsInMyGuild },
    { "UnitIsCorpse", &Script_UnitIsCorpse },
    { "UnitIsPartyLeader", &Script_UnitIsPartyLeader },
    { "UnitGroupRolesAssigned", &Script_UnitGroupRolesAssigned },
    { "UnitIsRaidOfficer", &Script_UnitIsRaidOfficer },
    { "UnitInParty", &Script_UnitInParty },
    { "UnitPlayerOrPetInParty", &Script_UnitPlayerOrPetInParty },
    { "UnitInRaid", &Script_UnitInRaid },
    { "UnitPlayerOrPetInRaid", &Script_UnitPlayerOrPetInRaid },
    { "UnitPlayerControlled", &Script_UnitPlayerControlled },
    { "UnitIsAFK", &Script_UnitIsAFK },
    { "UnitIsDND", &Script_UnitIsDND },
    { "UnitIsPVP", &Script_UnitIsPVP },
    { "UnitIsPVPSanctuary", &Script_UnitIsPVPSanctuary },
    { "UnitIsPVPFreeForAll", &Script_UnitIsPVPFreeForAll },
    { "UnitFactionGroup", &Script_UnitFactionGroup },
    { "UnitReaction", &Script_UnitReaction },
    { "UnitIsEnemy", &Script_UnitIsEnemy },
    { "UnitIsFriend", &Script_UnitIsFriend },
    { "UnitCanCooperate", &Script_UnitCanCooperate },
    { "UnitCanAssist", &Script_UnitCanAssist },
    { "UnitCanAttack", &Script_UnitCanAttack },
    { "UnitIsCharmed", &Script_UnitIsCharmed },
    { "UnitIsPossessed", &Script_UnitIsPossessed },
    { "PlayerCanTeleport", &Script_PlayerCanTeleport },
    { "UnitClassification", &Script_UnitClassification },
    { "UnitIsQuestBoss", &Script_UnitIsQuestBoss },
    { "UnitSelectionColor", &Script_UnitSelectionColor },
    { "UnitGUID", &Script_UnitGUID },
    { "UnitName", &Script_UnitName },
    { "UnitPVPName", &Script_UnitPVPName },
    { "UnitXP", &Script_UnitXP },
    { "UnitXPMax", &Script_UnitXPMax },
    { "UnitHealth", &Script_UnitHealth },
    { "UnitHealthMax", &Script_UnitHealthMax },
    { "UnitMana", &Script_UnitPower },
    { "UnitManaMax", &Script_UnitPowerMax },
    { "UnitPower", &Script_UnitPower },
    { "UnitPowerMax", &Script_UnitPowerMax },
    { "UnitPowerType", &Script_UnitPowerType },
    { "UnitAlternatePowerInfo", &Script_UnitAlternatePowerInfo },
    { "UnitAlternatePowerCounterInfo", &Script_UnitAlternatePowerCounterInfo },
    { "GetAlternatePowerInfoByID", &Script_GetAlternatePowerInfoByID },
    { "UnitAlternatePowerTextureInfo", &Script_UnitAlternatePowerTextureInfo },
    { "UnitPowerBarTimerInfo", &Script_UnitPowerBarTimerInfo },
    { "UnitNumPowerBarTimers", &Script_UnitNumPowerBarTimers },
    { "UnitOnTaxi", &Script_UnitOnTaxi },
    { "UnitIsFeignDeath", &Script_UnitIsFeignDeath },
    { "UnitIsDead", &Script_UnitIsDead },
    { "UnitIsGhost", &Script_UnitIsGhost },
    { "UnitIsDeadOrGhost", &Script_UnitIsDeadOrGhost },
    { "UnitIsConnected", &Script_UnitIsConnected },
    { "UnitAffectingCombat", &Script_UnitAffectingCombat },
    { "UnitSex", &Script_UnitSex },
    { "UnitLevel", &Script_UnitLevel },
    { "UnitInPhase", &Script_UnitInPhase },
    { "GetMoney", &Script_GetMoney },
    { "UnitRace", &Script_UnitRace },
    { "UnitClass", &Script_UnitClass },
    { "UnitClassBase", &Script_UnitClassBase },
    { "UnitResistance", &Script_UnitResistance },
    { "ResistancePercent", &Script_ResistancePercent },
    { "UnitStat", &Script_UnitStat },
    { "UnitAttackBothHands", &Script_UnitAttackBothHands },
    { "UnitDamage", &Script_UnitDamage },
    { "UnitRangedDamage", &Script_UnitRangedDamage },
    { "UnitRangedAttack", &Script_UnitRangedAttack },
    { "UnitAttackSpeed", &Script_UnitAttackSpeed },
    { "UnitAttackPower", &Script_UnitAttackPower },
    { "UnitRangedAttackPower", &Script_UnitRangedAttackPower },
    { "UnitSpellHaste", &Script_UnitSpellHaste },
    { "UnitDefense", &Script_UnitDefense },
    { "UnitArmor", &Script_UnitArmor },
    { "UnitBuff", &Script_UnitBuff },
    { "UnitDebuff", &Script_UnitDebuff },
    { "UnitAura", &Script_UnitAura },
    { "UnitIsTapped", &Script_UnitIsTapped },
    { "UnitIsTappedByPlayer", &Script_UnitIsTappedByPlayer },
    { "UnitIsTappedByAllThreatList", &Script_UnitIsTappedByAllThreatList },
    { "UnitIsTrivial", &Script_UnitIsTrivial },
    { "UnitHasRelicSlot", &Script_UnitHasRelicSlot },
    { "SetPortraitTexture", &Script_SetPortraitTexture },
    { "SetAchievementComparisonPortrait", &Script_SetAchievementComparisonPortrait },
    { "HasFullControl", &Script_HasFullControl },
    { "GetComboPoints", &Script_GetComboPoints },
    { "IsInGuild", &Script_IsInGuild },
    { "IsGuildLeader", &Script_IsGuildLeader },
    { "IsArenaTeamCaptain", &Script_IsArenaTeamCaptain },
    { "IsInArenaTeam", &Script_IsInArenaTeam },
    { "IsResting", &Script_IsResting },
    { "GetCombatRating", &Script_GetCombatRating },
    { "GetCombatRatingBonus", &Script_GetCombatRatingBonus },
    { "GetMaxCombatRatingBonus", &Script_GetMaxCombatRatingBonus },
    { "GetDodgeChance", &Script_GetDodgeChance },
    { "GetBlockChance", &Script_GetBlockChance },
    { "GetShieldBlock", &Script_GetShieldBlock },
    { "GetParryChance", &Script_GetParryChance },
    { "IsDualWielding", &Script_IsDualWielding },
    { "GetCritChanceFromAgility", &Script_GetCritChanceFromAgility },
    { "GetSpellCritChanceFromIntellect", &Script_GetSpellCritChanceFromIntellect },
    { "GetCritChance", &Script_GetCritChance },
    { "GetRangedCritChance", &Script_GetRangedCritChance },
    { "GetSpellCritChance", &Script_GetSpellCritChance },
    { "GetSpellBonusDamage", &Script_GetSpellBonusDamage },
    { "GetSpellBonusHealing", &Script_GetSpellBonusHealing },
    { "GetPetSpellBonusDamage", &Script_GetPetSpellBonusDamage },
    { "GetSpellPenetration", &Script_GetSpellPenetration },
    { "GetArmorPenetration", &Script_GetArmorPenetration },
    { "GetAttackPowerForStat", &Script_GetAttackPowerForStat },
    { "GetMeleeHaste", &Script_GetMeleeHaste },
    { "GetRangedHaste", &Script_GetRangedHaste },
    { "GetPetMeleeHaste", &Script_GetPetMeleeHaste },
    { "GetOverrideSpellPowerByAP", &Script_GetOverrideSpellPowerByAP },
    { "UnitCreatureType", &Script_UnitCreatureType },
    { "UnitCreatureFamily", &Script_UnitCreatureFamily },
    { "GetResSicknessDuration", &Script_GetResSicknessDuration },
    { "GetPVPSessionStats", &Script_GetPVPSessionStats },
    { "GetPVPYesterdayStats", &Script_GetPVPYesterdayStats },
    { "GetPVPLifetimeStats", &Script_GetPVPLifetimeStats },
    { "UnitPVPRank", &Script_UnitPVPRank },
    { "GetPVPRankInfo", &Script_GetPVPRankInfo },
    { "GetPVPRankProgress", &Script_GetPVPRankProgress },
    { "UnitCastingInfo", &Script_UnitCastingInfo },
    { "UnitChannelInfo", &Script_UnitChannelInfo },
    { "IsLoggedIn", &Script_IsLoggedIn },
    { "IsFlyableArea", &Script_IsFlyableArea },
    { "IsIndoors", &Script_IsIndoors },
    { "IsOutdoors", &Script_IsOutdoors },
    { "IsOutOfBounds", &Script_IsOutOfBounds },
    { "IsFalling", &Script_IsFalling },
    { "IsSwimming", &Script_IsSwimming },
    { "IsSubmerged", &Script_IsSubmerged },
    { "IsFlying", &Script_IsFlying },
    { "IsMounted", &Script_IsMounted },
    { "IsStealthed", &Script_IsStealthed },
    { "UnitIsSameServer", &Script_UnitIsSameServer },
    { "GetUnitHealthModifier", &Script_GetUnitHealthModifier },
    { "GetUnitMaxHealthModifier", &Script_GetUnitMaxHealthModifier },
    { "GetUnitPowerModifier", &Script_GetUnitPowerModifier },
    { "GetUnitManaRegenRateFromSpirit", &Script_GetUnitManaRegenRateFromSpirit },
    { "GetManaRegen", &Script_GetManaRegen },
    { "GetPowerRegen", &Script_GetPowerRegen },
    { "GetRuneCooldown", &Script_GetRuneCooldown },
    { "GetRuneCount", &Script_GetRuneCount },
    { "GetRuneType", &Script_GetRuneType },
    { "ReportPlayerIsPVPAFK", &Script_ReportPlayerIsPVPAFK },
    { "PlayerIsPVPInactive", &Script_PlayerIsPVPInactive },
    { "GetExpertise", &Script_GetExpertise },
    { "GetExpertisePercent", &Script_GetExpertisePercent },
    { "GetMastery", &Script_GetMastery },
    { "UnitInBattleground", &Script_UnitInBattleground },
    { "UnitInRange", &Script_UnitInRange },
    { "GetUnitSpeed", &Script_GetUnitSpeed },
    { "GetUnitPitch", &Script_GetUnitPitch },
    { "UnitInVehicle", &Script_UnitInVehicle },
    { "UnitUsingVehicle", &Script_UnitUsingVehicle },
    { "UnitControllingVehicle", &Script_UnitControllingVehicle },
    { "UnitInVehicleControlSeat", &Script_UnitInVehicleControlSeat },
    { "UnitHasVehicleUI", &Script_UnitHasVehicleUI },
    { "UnitHasVehiclePlayerFrameUI", &Script_UnitHasVehiclePlayerFrameUI },
    { "UnitTargetsVehicleInRaidUI", &Script_UnitTargetsVehicleInRaidUI },
    { "UnitVehicleSkin", &Script_UnitVehicleSkin },
    { "UnitVehicleSeatCount", &Script_UnitVehicleSeatCount },
    { "UnitVehicleSeatInfo", &Script_UnitVehicleSeatInfo },
    { "UnitSwitchToVehicleSeat", &Script_UnitSwitchToVehicleSeat },
    { "CanSwitchVehicleSeat", &Script_CanSwitchVehicleSeat },
    { "GetVehicleUIIndicator", &Script_GetVehicleUIIndicator },
    { "GetVehicleUIIndicatorSeat", &Script_GetVehicleUIIndicatorSeat },
    { "UnitThreatSituation", &Script_UnitThreatSituation },
    { "UnitDetailedThreatSituation", &Script_UnitDetailedThreatSituation },
    { "UnitThreatPercentageOfLead", &Script_UnitThreatPercentageOfLead },
    { "UnitIsControlling", &Script_UnitIsControlling },
    { "EjectPassengerFromSeat", &Script_EjectPassengerFromSeat },
    { "CanEjectPassengerFromSeat", &Script_CanEjectPassengerFromSeat },
    { "RespondInstanceLock", &Script_RespondInstanceLock },
    { "GetPlayerFacing", &Script_GetPlayerFacing },
    { "GetPlayerInfoByGUID", &Script_GetPlayerInfoByGUID },
    { "GetItemStats", &Script_GetItemStats },
    { "GetItemStatDelta", &Script_GetItemStatDelta },
    { "IsXPUserDisabled", &Script_IsXPUserDisabled },
    { "FillLocalizedClassList", &Script_FillLocalizedClassList },
    { "UnitGetGuildXP", &Script_UnitGetGuildXP },
    { "UnitGetGuildLevel", &Script_UnitGetGuildLevel },
    { "UnitGetIncomingHeals", &Script_UnitGetIncomingHeals },
    { "IsGMClient", &Script_IsGMClient },
    { "UnitSetRole", &Script_UnitSetRole },
    { "InitiateRolePoll", &Script_InitiateRolePoll },
    { "GetHitModifier", &Script_GetHitModifier },
    { "GetSpellHitModifier", &Script_GetSpellHitModifier },
    { "GetEclipseDirection", &Script_GetEclipseDirection },
    { "UnitHPPerStamina", &Script_UnitHPPerStamina },
    { "GetMaxPlayerLevel", &Script_GetMaxPlayerLevel },
    { "LoadURLIndex", &Script_LoadURLIndex },
};

// OFFSET: 0x0044E140
void ScriptEventsRegisterFunctions() {
    SystemRegisterFunctions();

    for (uint32_t i = 0; i < sizeof(s_UnitFunctions) / sizeof(s_UnitFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_UnitFunctions[i].m_name, s_UnitFunctions[i].m_method);
    }
}

// OFFSET: 0x0045AAC0
void ScriptEventsUnregisterFunctions() {
    SystemUnregisterFunctions();

    for (uint32_t i = 0; i < sizeof(s_UnitFunctions) / sizeof(s_UnitFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_UnitFunctions[i].m_name);
    }

    ClearUniqueSignals();
}
