#include "Engine/Source/ComSat/ComSatSession.h"

// OFFSET: #0x0087F960
ComSat::SessionCallback::~SessionCallback() {
}

// INLINED
ComSat::Network::Session::Callback::~Callback() {
}

// PROVEN: 445d5124 IDENTICAL/NONE
// OFFSET: 0x007A95B0
void ComSat::Session::SetNetworkSession(ComSat::Network::Session* session) {
    this->m_networkSession = session;
}

// PROVEN: f19f84bd IDENTICAL/NONE
// OFFSET: 0x00B41A60/0x14090EBB0
ComSat::Network::Session* ComSat::Session::GetNetworkSession() {
    return this->m_networkSession;
}

// PROVEN: 97a6b7a8 IDENTICAL/NONE
// OFFSET: 0x00A392C0
bool ComSat::Session::LocalAdd(uint64_t talker) {
    return this->m_networkSession && this->m_networkSession->LocalAdd(talker);
}

// PROVEN: a55c7efe IDENTICAL/NONE
// OFFSET: 0x00A392F0
bool ComSat::Session::Add(uint64_t talker, uint8_t networkId, const uint8_t (&key)[16], const ComSat::Network::Address& address) {
    return this->m_networkSession && this->m_networkSession->Add(talker, networkId, key, address);
}

// PROVEN: 076ad1c5 IDENTICAL/NONE
// OFFSET: 0x00A39330
void ComSat::Session::Remove(uint64_t talker) {
    if (this->m_networkSession) {
        this->m_networkSession->Remove(talker);
    }
}

// OFFSET: %0x00979660
bool ComSat::Session::Send(uint64_t talker, const uint8_t* data, uint32_t size) {
    return this->m_networkSession && this->m_networkSession->Send(talker, data, size);
}

// PROVEN: e73ec9e1 IDENTICAL/NONE
// OFFSET: 0x00A39390
void ComSat::Session::Recv(uint64_t talker, const uint8_t* data, uint32_t size) {
    if (data && size) {
        this->m_callback->IncomingVoiceData(this->m_id, talker, data, size);
    }
}

// OFFSET: %0x009796E0
ComSat::Session::Session(ComSat::SessionCallback& callback, uint64_t id, ComSat::Allocator& allocator) {
    this->m_callback = &callback;
    this->m_id = id;
    this->m_networkSession = NULL;
    this->m_allocator = &allocator;
}

// OFFSET: %0x00979710
ComSat::Session::~Session() {
}
