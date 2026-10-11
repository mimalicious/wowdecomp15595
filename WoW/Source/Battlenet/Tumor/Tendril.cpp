#include "WoW/Source/Battlenet/Tumor/Tendril.h"
#include "WoW/Source/Battlenet/Tumor/TumorManager.h"

// PROVEN: 71c346c6 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void Battlenet::Service::Interface::RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) {
}

// OFFSET: 0x00662340
Battlenet::Service::Interface::~Interface() {
    if (this->m_tendrilId) {
        this->m_manager->GetTumor()->RemoveTendril(this->m_tendrilId);
    }
}

// PROVEN: eb74f12e IDENTICAL/NONE
// OFFSET: 0x006622C0/0x140307FF0
void Battlenet::Service::Interface::Register() {
    if (this->m_eventId.m_value) {
        Tumor* tumor = this->m_manager->GetTumor();
        tumor->AllocTendrilId(this->m_tendrilId);
        this->m_eventAdapter = tumor->AllocEventAdapter(this->m_eventId, *this, this->m_tendrilId);
        tumor->Subscribe(this->m_eventId, *this, this->m_tendrilId);
    }
}

// PROVEN: 2a0132f2 IDENTICAL/NONE
// OFFSET: 0x00662310/0x140308270
void Battlenet::Service::Interface::SendEvent(const uint8_t& eventType, const BSN::FourCC& eventId, const BSN::EncodedObject* data) {
    if (this->m_eventAdapter) {
        this->m_eventAdapter->SendEvent(eventType, eventId, data);
    }
}

// OFFSET: 0x00662390
Battlenet::Tendril::Tendril(BSN::FourCC eventId, TumorManager& manager) : Service::Interface(manager) {
    this->m_eventId = eventId;

    if (this->m_eventId.m_value) {
        Tumor* tumor = this->m_manager->GetTumor();
        tumor->AllocTendrilId(this->m_tendrilId);
        this->m_eventAdapter = tumor->AllocEventAdapter(this->m_eventId, *this, this->m_tendrilId);
        tumor->Subscribe(this->m_eventId, *this, this->m_tendrilId);
    }

    this->m_adapter = manager.GetTumor()->AllocAdapter(*this, this->m_tendrilId);
}

// INLINED
Battlenet::Tendril::~Tendril() {
}

// PROVEN: 84c27b8e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Battlenet::Tendril::DoSubscriptions() {
}
