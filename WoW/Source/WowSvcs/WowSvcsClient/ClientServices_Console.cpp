#include "ClientServices.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 0b5f5137 IDENTICAL/DIFFERENT
// OFFSET: 0x004D0AA0/0x14010D390
int32_t ConsoleCommand_Logout(const char* command, const char* arguments) {
    if (ClientServices::s_currentConnection->m_inGame) {
        ClientServices::s_currentConnection->CharacterLogout(false, true);
    }

    return 1;
}
