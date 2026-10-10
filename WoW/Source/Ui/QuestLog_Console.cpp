#include "QuestLog.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// OFFSET: 0x00964E00/0x1406DA5C0
int32_t UpdateQuestPOIHandler(const char* command, const char* arguments) {
    if (ClntObjMgrGetActivePlayerObj()) {
        CGQuestLog::UpdateQuestPOIFromUserServer();
    }

    return 1;
}
