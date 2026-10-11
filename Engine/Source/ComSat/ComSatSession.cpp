#include "Engine/Source/ComSat/ComSatSession.h"

// INLINED
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

// PROVEN: f19f84bd IDENTICAL/DIFFERENT
// OFFSET: 0x00B41A60/0x14090EBB0
ComSat::Network::Session* ComSat::Session::GetNetworkSession() {
    return this->m_networkSession;
}

// PROVEN: 97a6b7a8 IDENTICAL/IDENTICAL
// OFFSET: 0x00A392C0/0x1407E56A0
bool ComSat::Session::LocalAdd(uint64_t talker) {
    return this->m_networkSession && this->m_networkSession->LocalAdd(talker);
}

// PROVEN: a55c7efe IDENTICAL/IDENTICAL
// OFFSET: 0x00A392F0/0x1407E56D0
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

// PROVEN: 18e0b577 IDENTICAL/UNPROVEN
// OFFSET: 0x00A39350/0x1407E5720
bool ComSat::Session::Send(uint64_t talker, const uint8_t* data, uint32_t size) {
    return this->m_networkSession && this->m_networkSession->Send(talker, data, size);
}

// PROVEN: e73ec9e1 IDENTICAL/DIFFERENT
// OFFSET: 0x00A39390/0x1407E57C0
void ComSat::Session::Recv(uint64_t talker, const uint8_t* data, uint32_t size) {
    if (data && size) {
        this->m_callback->IncomingVoiceData(this->m_id, talker, data, size);
    }
}

// OFFSET: 0x00A393D0/0x1407E5790
ComSat::Session::Session(ComSat::SessionCallback& callback, uint64_t id, ComSat::Allocator& allocator) {
    this->m_callback = &callback;
    this->m_id = id;
    this->m_networkSession = NULL;
    this->m_allocator = &allocator;
}

// OFFSET: 0x00A39400
ComSat::Session::~Session() {
}
