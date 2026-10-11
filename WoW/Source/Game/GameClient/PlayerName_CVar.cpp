#include "PlayerName.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x004E0FA0
bool ObjectNameShowCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t mask = s_nameMask;

    if (SStrToInt(value)) {
        s_nameMask |= static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
    } else {
        s_nameMask &= ~static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
    }

    if (s_nameMask == mask) {
        return true;
    }

    for (PLAYERNAMEDESC* desc = s_playerNames.Head(); desc; desc = s_playerNames.Next(desc)) {
        desc->m_flags |= 1;
    }

    return true;
}

// OFFSET: 0x004E1010
void PlayerNameRegisterCVars() {
    CVar::Register("UnitNameOwn", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x1), false);
    CVar::Register("UnitNameNPC", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x2), false);
    CVar::Register("UnitNamePlayerGuild", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x4), false);
    CVar::Register("UnitNamePlayerPVPTitle", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x8), false);
    CVar::Register("UnitNameEnemyPlayerName", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x10), false);
    CVar::Register("UnitNameEnemyPetName", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x20), false);
    CVar::Register("UnitNameEnemyGuardianName", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x1000), false);
    CVar::Register("UnitNameEnemyTotemName", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x40), false);
    CVar::Register("UnitNameFriendlyPlayerName", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x80), false);
    CVar::Register("UnitNameFriendlyPetName", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x100), false);
    CVar::Register("UnitNameFriendlyGuardianName", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x2000), false);
    CVar::Register("UnitNameFriendlyTotemName", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x200), false);
    CVar::Register("UnitNameNonCombatCreatureName", NULL, 0x10, "0", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x400), false);
    CVar::Register("UnitNameFriendlySpecialNPCName", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x4000), false);
    CVar::Register("UnitNameGuildTitle", NULL, 0x10, "1", ObjectNameShowCallback, CATEGORY_GAME, false, reinterpret_cast<void*>(0x8000), false);
}
