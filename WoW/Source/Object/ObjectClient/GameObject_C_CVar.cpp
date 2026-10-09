#include "GameObject_C.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// STATIC: 0x00DD5CE8
CVar* CGGameObject_C::s_cvUseAnimKits;

// STATIC: 0x00DD5CEC
CVar* CGGameObject_C::s_cvForceMouseOver;

// PROVEN: ca0b093c IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090EBB0
static int32_t UseAnimKitsEnumProc(WOWGUID guid, void* param) {
    return 1;
}

// PROVEN: e8597fdd IDENTICAL/NONE
// OFFSET: 0x00599110/0x140207570
bool CGGameObject_C::UseAnimKitsChangedCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ClntObjMgrEnumVisibleObjects(UseAnimKitsEnumProc, reinterpret_cast<void*>(static_cast<intptr_t>(SStrToInt(value))));
    return true;
}
