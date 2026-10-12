#include <cstdint>
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/Storm/H/storm.h"

static const _ARGLIST s_argList[] = {
    { SCMD_TYPE_BOOL, CMD_D3D, "d3d", NULL },
    { SCMD_TYPE_BOOL, CMD_D3D9EX, "d3d9ex", NULL },
    { SCMD_TYPE_STRING, CMD_DATA_DIR, "datadir", NULL },
    { SCMD_TYPE_BOOL, CMD_NO_LAG_FIX, "nolagfix", NULL },
    { SCMD_TYPE_STRING, CMD_LOADFILE, "loadfile", NULL },
    { SCMD_TYPE_STRING, CMD_GAMETYPE, "gametype", NULL },
    { SCMD_TYPE_BOOL, CMD_OPENGL, "opengl", NULL },
    { SCMD_TYPE_BOOL, CMD_SW_TNL, "swtnl", NULL },
    { SCMD_TYPE_BOOL, CMD_TIMEDEMO, "timedemo", NULL },
    { SCMD_TYPE_STRING, CMD_DEMOREZ, "rez", NULL },
    { SCMD_TYPE_STRING, CMD_DEMODEPTH, "depth", NULL },
    { SCMD_TYPE_STRING, CMD_DEMODETAIL, "detail", NULL },
    { SCMD_TYPE_STRING, CMD_DEMOSOUND, "sound", NULL },
    { SCMD_TYPE_BOOL, CMD_FULL_SCREEN, "fullscreen", NULL },
    { SCMD_TYPE_BOOL, CMD_22050HZ, "22050", NULL },
    { SCMD_TYPE_BOOL, CMD_NO_WARNINGS, "nowarnings", NULL },
};

// OFFSET: 0x00871F70/0x1405A8250
int32_t CmdLineGetBool(CMDOPT opt) {
    return SCmdGetBool(opt);
}

// OFFSET: 0x00871F80/0x1405A8260
const char* CmdLineGetString(CMDOPT opt) {
    // STATIC: 0x00EC6A10/0x140DB8D10
    static char buffer[260];

    SCmdGetString(opt, buffer, sizeof(buffer));
    return buffer;
}

// OFFSET: 0x00871FA0/0x1405A8290
int32_t CmdLineProcess() {
    SCmdRegisterArgList(s_argList, sizeof(s_argList) / sizeof(s_argList[0]));
    return SCmdProcessCommandLine(NULL, NULL);
}
