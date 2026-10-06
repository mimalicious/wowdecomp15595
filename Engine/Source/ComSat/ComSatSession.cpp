#include "Engine/Source/ComSat/ComSatSession.h"

// OFFSET: #0x0087F960
ComSat::SessionCallback::~SessionCallback() {
}

// INLINED
ComSat::Network::Session::Callback::~Callback() {
}

// OFFSET: %0x009795B0
void ComSat::Session::SetNetworkSession(ComSat::Network::Session* session) {
    this->m_networkSession = session;
}

// OFFSET: %0x009795C0
ComSat::Network::Session* ComSat::Session::GetNetworkSession() {
    return this->m_networkSession;
}

// OFFSET: %0x009795D0
bool ComSat::Session::LocalAdd(uint64_t talker) {
    return this->m_networkSession && this->m_networkSession->LocalAdd(talker);
}

// OFFSET: %0x00979600
bool ComSat::Session::Add(uint64_t talker, uint8_t networkId, const uint8_t (&key)[16], const ComSat::Network::Address& address) {
    return this->m_networkSession && this->m_networkSession->Add(talker, networkId, key, address);
}

// OFFSET: %0x00979640
void ComSat::Session::Remove(uint64_t talker) {
    if (this->m_networkSession) {
        this->m_networkSession->Remove(talker);
    }
}

// OFFSET: %0x00979660
bool ComSat::Session::Send(uint64_t talker, const uint8_t* data, uint32_t size) {
    return this->m_networkSession && this->m_networkSession->Send(talker, data, size);
}

// OFFSET: %0x009796A0
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
