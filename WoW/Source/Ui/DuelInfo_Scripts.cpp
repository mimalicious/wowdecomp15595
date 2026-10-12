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

// PROVEN: 30863014 IDENTICAL/IDENTICAL
// OFFSET: 0x009F05D0/0x14078A3E0
static int32_t Script_StartDuel(lua_State* L) {
    WOWGUID guid = CGGameUI::ClosestObjectMatch(lua_tolstring(L, 1, NULL), TYPEMASK_PLAYER, 0, StringToBOOL(L, 2, 0), FLT_MAX, 0);

    if (guid) {
        Spell_C_CastSpell(CGSpellBook::m_duelSpell, NULL, guid, false);
        CGDuelInfo::m_dueler = guid;
    }

    return 0;
}

// PROVEN: f3f9a1e9 IDENTICAL/IDENTICAL
// OFFSET: 0x009F0AA0/0x14078A960
static int32_t Script_AcceptDuel(lua_State* L) {
    CGDuelInfo::AcceptDuel();
    return 0;
}

// PROVEN: 09bf1814 IDENTICAL/IDENTICAL
// OFFSET: 0x009F0AB0/0x14078A970
static int32_t Script_CancelDuel(lua_State* L) {
    CGDuelInfo::CancelDuel();
    return 0;
}

// STATIC: 0x00D16DE8/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "StartDuel", &Script_StartDuel },
    { "AcceptDuel", &Script_AcceptDuel },
    { "CancelDuel", &Script_CancelDuel },
};

// PROVEN: db9065e7 IDENTICAL/UNPROVEN
// OFFSET: 0x009F0650/0x14078A480
void DuelInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 8fefd220 IDENTICAL/UNPROVEN
// OFFSET: 0x009F0680/0x14078A4D0
void DuelInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
