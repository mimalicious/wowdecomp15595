#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"

// PROVEN: 9f34f71d IDENTICAL/IDENTICAL
// OFFSET: [0x00847950/0x1407D7E80]
void JamClientMessage::Put(CDataStore* msg) {
}

// PROVEN: 7f7d9fdd IDENTICAL/IDENTICAL
// OFFSET: [0x006705F0/0x140317F90]
bool JamClientMessage::Get(CDataStore* msg, bool full) {
    return false;
}

// PROVEN: 0dd7513d IDENTICAL/IDENTICAL
// OFFSET: [0x006F8320/0x1403C1250]
JamValue JamClientMessage::GetInfo() {
    JamValue info;
    info.m_type = &g_jamTypeNone;
    info.m_data = NULL;
    info.m_meta = NULL;

    return info;
}

// PROVEN: 556d25a6 IDENTICAL/IDENTICAL
// OFFSET: [0x0066BC00/0x140312430]
JamClientRideTicket::JamClientRideTicket() {
}

// PROVEN: e4477cfe IDENTICAL/IDENTICAL
// OFFSET: [0x00401000/0x1407D7E80]
JamClientRideTicket::~JamClientRideTicket() {
}

// PROVEN: 9cff3e9e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
JamThrottleState::~JamThrottleState() {
}

// PROVEN: 7a4c8e57 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool JamClientRideTicket::IsValid() const {
    return true;
}

// PROVEN: 9e923dc0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
JamCliCUFProfile::~JamCliCUFProfile() {
}
