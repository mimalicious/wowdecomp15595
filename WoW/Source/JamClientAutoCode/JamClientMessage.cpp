#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"

// PROVEN: 9f34f71d IDENTICAL/NONE
// OFFSET: [0x00847950]
void JamClientMessage::Put(CDataStore* msg) {
}

// PROVEN: 7f7d9fdd IDENTICAL/NONE
// OFFSET: [0x006705F0]
bool JamClientMessage::Get(CDataStore* msg, bool full) {
    return false;
}

// OFFSET: [0x006F8320/0x1403C1250]
JamValue JamClientMessage::GetInfo() {
    JamValue info;
    info.m_type = &g_jamTypeNone;
    info.m_data = NULL;
    info.m_meta = NULL;

    return info;
}

// OFFSET: [0x0066BC00/0x140312430]
JamClientRideTicket::JamClientRideTicket() {
}

// OFFSET: [0x00401000/0x1407D7E80]
JamClientRideTicket::~JamClientRideTicket() {
}

// OFFSET: 0x00401000
JamThrottleState::~JamThrottleState() {
}

// OFFSET: 0x00A3AA30
bool JamClientRideTicket::IsValid() const {
    return true;
}

// OFFSET: 0x00401000
JamCliCUFProfile::~JamCliCUFProfile() {
}
