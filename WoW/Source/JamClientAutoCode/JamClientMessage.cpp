#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"

// OFFSET: [0x00847950/0x1407D7E80]
void JamClientMessage::Put(CDataStore* msg) {
}

// OFFSET: [0x006705F0/0x140317F90]
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

// OFFSET: 0x00401000/0x1407D7E80
JamThrottleState::~JamThrottleState() {
}

// OFFSET: 0x00A3AA30/0x1404BDD20
bool JamClientRideTicket::IsValid() const {
    return true;
}

// OFFSET: 0x00401000/0x1407D7E80
JamCliCUFProfile::~JamCliCUFProfile() {
}
