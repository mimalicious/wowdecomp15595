#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "Engine/Source/Base/CDataStore.h"

// STATIC: 0x00C45F90
static const uint32_t s_messageInfoDesc[1] = { 0 };

// PROVEN: 9f34f71d IDENTICAL/NONE
// OFFSET: [0x00847950]
void JamClientMessage::Put(CDataStore* msg) {
}

// PROVEN: 7f7d9fdd IDENTICAL/NONE
// OFFSET: [0x006705F0]
bool JamClientMessage::Get(CDataStore* msg, bool full) {
    return false;
}

// PROVEN: 23ba05d7 IDENTICAL/NONE
// OFFSET: [0x006F8320/0x1403C1250]
JAMMESSAGEINFO JamClientMessage::GetInfo() {
    JAMMESSAGEINFO info;
    info.m_desc = s_messageInfoDesc;
    info.m_size = 0;
    info.m_flags = 0;

    return info;
}
