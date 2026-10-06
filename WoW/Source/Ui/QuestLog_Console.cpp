#include "QuestLog.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// OFFSET: #0x005E7170
int32_t UpdateQuestPOIHandler(const char* command, const char* arguments) {
    if (ClntObjMgrGetActivePlayerObj()) {
        CGQuestLog::UpdateQuestPOIFromUserServer();
    }

    return 1;
}
