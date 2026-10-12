#include "GlueMgr.h"

// STATIC: 0x00EBBFB8/0x140DACD90
const char* g_glueScriptEvents[NUM_GLUE_SCRIPT_EVENTS];

// PROVEN: f3d1e815 IDENTICAL/UNPROVEN
// OFFSET: 0x00805930/0x14051AE30
void GlueScriptEventsInitialize() {
#define GLUE_SCRIPT_EVENT_NAME(name) g_glueScriptEvents[GLUE_EVENT_##name] = #name;
    GLUE_SCRIPT_EVENTS(GLUE_SCRIPT_EVENT_NAME)
#undef GLUE_SCRIPT_EVENT_NAME
}
