#include "DuelInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include <cfloat>
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// OFFSET: #0x005CF950
static int32_t Script_StartDuel(lua_State* L) {
    int32_t exact = StringToBOOL(L, 2, 0);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(lua_tolstring(L, 1, NULL), TYPEMASK_PLAYER, 0, exact, FLT_MAX, 0);

    if (guid) {
        Spell_C::CastSpell(CGSpellBook::m_duelSpell, NULL, guid, false);
        CGDuelInfo::m_dueler = guid;
    }

    return 0;
}

// OFFSET: %0x009321D0
static int32_t Script_AcceptDuel(lua_State* L) {
    CGDuelInfo::AcceptDuel();
    return 0;
}

// OFFSET: %0x009321E0
static int32_t Script_CancelDuel(lua_State* L) {
    CGDuelInfo::CancelDuel();
    return 0;
}

// STATIC: #0x00AD0AC4
static FrameScript_Method s_ScriptFunctions[] = {
    { "StartDuel", &Script_StartDuel },
    { "AcceptDuel", &Script_AcceptDuel },
    { "CancelDuel", &Script_CancelDuel },
};

// PROVEN: db9065e7 IDENTICAL/NONE
// OFFSET: 0x009F0650
void DuelInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 8fefd220 IDENTICAL/NONE
// OFFSET: 0x009F0680
void DuelInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
