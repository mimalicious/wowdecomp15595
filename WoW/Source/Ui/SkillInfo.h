#ifndef WOW_SOURCE_UI_SKILLINFO_H
#define WOW_SOURCE_UI_SKILLINFO_H

#include <cstdint>

struct lua_State;

class CGSkillInfo {
    public:
    static void LeaveWorld();
};

void SkillInfoRegisterScriptFunctions();
void SkillInfoUnregisterScriptFunctions();

#endif
