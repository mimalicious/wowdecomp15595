#include "SkillInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// OFFSET: 0x009E2180/0x1407793A0
void PlayerSignalSkillStatChanges() {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SKILL_LINES_CHANGED, NULL);
    }
}

// OFFSET: 0x00401000/0x1407D7E80
void CGSkillInfo::LeaveWorld() {
}
