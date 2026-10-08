#include "WoW/Source/Battlenet/Tumor/Tumor.h"
#include <cstring>
#include <new>
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x00660860/0x140305E00
Battlenet::Tumor::Tumor(Application& application) {
    this->m_application = &application;
    this->m_nextTendrilId = 1;
    memset(this->m_adapters, 0, sizeof(this->m_adapters));
    memset(this->m_eventAdapters, 0, sizeof(this->m_eventAdapters));
}

// OFFSET: [0x00661A30/0x140307480, 0x00660910/0x140305EE0]
Battlenet::Tumor::~Tumor() {
    for (int32_t i = 0; i < 20; i++) {
        if (this->m_adapters[i]) {
            Battlenet::Delete(this->m_adapters[i]);
            this->m_adapters[i] = NULL;
        }
    }

    for (int32_t i = 0; i < 20; i++) {
        if (this->m_eventAdapters[i]) {
            Battlenet::Delete(this->m_eventAdapters[i]);
            this->m_eventAdapters[i] = NULL;
        }
    }
}

// PROVEN: 886170e7 IDENTICAL/NONE
// OFFSET: 0x00661700/0x140307170
bool Battlenet::Tumor::AllocTendrilId(uint32_t& tendrilId) {
    tendrilId = this->m_nextTendrilId++;
    uint32_t first = tendrilId;

    do {
        uint32_t i = 0;

        while (!this->m_eventAdapters[i] || this->m_eventAdapters[i]->m_tendrilId != tendrilId) {
            if (++i >= 20) {
                return true;
            }
        }

        tendrilId = this->m_nextTendrilId++;
    } while (tendrilId != first);

    return false;
}

// OFFSET: 0x00661760/0x140307200
int32_t Battlenet::Tumor::Process(uint32_t time) {
    int32_t error = 0;

    for (int32_t i = 0; i < 20; i++) {
        if (this->m_adapters[i]) {
            int32_t result = static_cast<SystemClient*>(this->m_adapters[i])->Process(time);

            if (result) {
                error = result;
            }
        }
    }

    return error;
}

// OFFSET: 0x006617B0/0x140307260
int32_t Battlenet::Tumor::GetLastError() const {
    for (uint32_t i = 0; i < 20; i++) {
        if (this->m_adapters[i]) {
            int32_t result = static_cast<const SystemClient*>(this->m_adapters[i])->GetLastError();

            if (result) {
                return result;
            }
        }
    }

    return 0;
}

// PROVEN: 7220af67 IDENTICAL/NONE
// OFFSET: 0x006617E0
Battlenet::Tumor::ConnectionBinding* Battlenet::Tumor::FindConnectionBinding(uint32_t connection) {
    uint32_t i = 0;

    while (!this->m_connectionBindings[i].m_active || this->m_connectionBindings[i].m_connection != connection) {
        if (++i >= 64) {
            return NULL;
        }
    }

    return &this->m_connectionBindings[i];
}

// PROVEN: 2b51fbc8 IDENTICAL/NONE
// OFFSET: 0x00661820
Battlenet::Tumor::TokenBinding* Battlenet::Tumor::FindTokenBinding(uint32_t token) {
    uint32_t i = 0;

    while (!this->m_tokenBindings[i].m_active || this->m_tokenBindings[i].m_token != token) {
        if (++i >= 64) {
            return NULL;
        }
    }

    return &this->m_tokenBindings[i];
}

// INLINED
Battlenet::Tumor::EventBinding* Battlenet::Tumor::AllocEventBinding() {
    uint32_t i = 0;

    while (this->m_eventBindings[i].m_active) {
        if (++i >= 20) {
            DebugLog("Ran out of room in Tumor::m_eventBindings");
            return NULL;
        }
    }

    return &this->m_eventBindings[i];
}

// INLINED
Battlenet::Tumor::ConnectionBinding* Battlenet::Tumor::AllocConnectionBinding() {
    uint32_t i = 0;

    while (this->m_connectionBindings[i].m_active) {
        if (++i >= 64) {
            DebugLog("Ran out of room in Tumor::m_connectionBindings");
            return NULL;
        }
    }

    return &this->m_connectionBindings[i];
}

// OFFSET: 0x00661860
Battlenet::Tumor::TokenBinding* Battlenet::Tumor::AllocTokenBinding() {
    uint32_t i = 0;

    while (this->m_tokenBindings[i].m_active) {
        if (++i >= 64) {
            DebugLog("Ran out of room in Tumor::m_tokenBindings");
            return NULL;
        }
    }

    TokenBinding* binding = &this->m_tokenBindings[i];
    binding->m_token = i;
    return binding;
}

// OFFSET: 0x00661AE0/0x140307550
void Battlenet::Tumor::Subscribe(BSN::FourCC eventId, EventSubscriber& subscriber, const uint32_t& tendrilId) {
    EventBinding* binding = this->AllocEventBinding();

    if (binding) {
        binding->m_eventId = eventId;
        binding->m_subscriber = &subscriber;
        binding->m_active = true;
        binding->m_tendrilId = tendrilId;
    }
}

// OFFSET: 0x00661B40/0x1403075C0
Battlenet::Tumor::Adapter* Battlenet::Tumor::AllocAdapter(SystemClient& client, const uint32_t& tendrilId) {
    uint32_t index = 0;

    while (this->m_adapters[index]) {
        if (++index >= 20) {
            DebugLog("No space in Tumor::m_adapters for new Tendril");
            return NULL;
        }
    }

    void* memory = Battlenet::g_allocator->Alloc(sizeof(Adapter), ".\\Tumor.cpp(367):Adapter");
    Adapter* adapter = memory ? new (memory) Adapter(client, this, index, tendrilId) : NULL;
    this->m_adapters[index] = adapter;
    return adapter;
}

// OFFSET: 0x00661BD0/0x140307680
Battlenet::Tumor::EventAdapter* Battlenet::Tumor::AllocEventAdapter(BSN::FourCC eventId, EventSubscriber& subscriber, const uint32_t& tendrilId) {
    uint32_t index = 0;

    while (this->m_eventAdapters[index]) {
        if (++index >= 20) {
            DebugLog("No space in Tumor::m_eventAdapters for new Tendril");
            return NULL;
        }
    }

    void* memory = Battlenet::g_allocator->Alloc(sizeof(EventAdapter), ".\\Tumor.cpp(386):EventAdapter");
    EventAdapter* adapter = memory ? new (memory) EventAdapter(eventId, subscriber, this, index, tendrilId) : NULL;
    this->m_eventAdapters[index] = adapter;
    return adapter;
}

// OFFSET: 0x00661C70/0x140307770
void Battlenet::Tumor::RemoveTendril(const uint32_t& tendrilId) {
    for (int32_t i = 0; i < 20; i++) {
        if (this->m_eventAdapters[i] && this->m_eventAdapters[i]->m_tendrilId == tendrilId) {
            Battlenet::Delete(this->m_eventAdapters[i]);
            this->m_eventAdapters[i] = NULL;
        }
    }

    for (int32_t i = 0; i < 20; i++) {
        if (this->m_adapters[i] && this->m_adapters[i]->m_tendrilId == tendrilId) {
            Battlenet::Delete(this->m_adapters[i]);
            this->m_adapters[i] = NULL;
        }
    }

    for (int32_t i = 0; i < 20; i++) {
        if (this->m_eventBindings[i].m_active && this->m_eventBindings[i].m_tendrilId == tendrilId) {
            this->m_eventBindings[i].m_active = false;
        }
    }

    for (int32_t i = 0; i < 64; i++) {
        if (this->m_connectionBindings[i].m_active && this->m_connectionBindings[i].m_tendrilId == tendrilId) {
            this->m_connectionBindings[i].m_active = false;
        }
    }
}

// OFFSET: 0x00661D80
uint32_t Battlenet::Tumor::Connect(uint32_t index, const IP4::AddressPort& address, const uint32_t& tendrilId) {
    ConnectionBinding* binding = this->AllocConnectionBinding();

    if (!binding) {
        DebugLog("Couldn't get a connectionId!");
        return static_cast<uint32_t>(-1);
    }

    uint32_t connection = this->m_application->Connect(address, tendrilId);
    binding->m_connection = connection;
    binding->m_client = this->m_adapters[index];
    binding->m_active = true;
    binding->m_tendrilId = tendrilId;
    return connection;
}

// OFFSET: 0x00661E30
uint32_t Battlenet::Tumor::OpenDatagram(uint32_t index, int32_t port, const uint32_t& tendrilId) {
    ConnectionBinding* binding = this->AllocConnectionBinding();

    if (!binding) {
        DebugLog("Couldn't get a connectionId!");
        return static_cast<uint32_t>(-1);
    }

    uint32_t connection = this->m_application->OpenDatagram(port, tendrilId);
    binding->m_connection = connection;
    binding->m_client = this->m_adapters[index];
    binding->m_active = true;
    binding->m_tendrilId = tendrilId;
    return connection;
}

// PROVEN: 3faf57fe IDENTICAL/NONE
// OFFSET: 0x00661EE0/0x140307B20
void Battlenet::Tumor::Resolved(uint32_t request, const Client::DNSInfo& info) {
    TokenBinding* binding = this->FindTokenBinding(request);

    if (binding) {
        binding->m_client->Resolved(binding->m_request, info);
        binding->m_token = 0;
        binding->m_client = NULL;
        binding->m_active = false;
    }
}

// PROVEN: 5a4cec35 IDENTICAL/NONE
// OFFSET: 0x00661F20/0x140307B90
void Battlenet::Tumor::Connected(uint32_t connection, const IP4::AddressPort& address) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        binding->m_client->Connected(connection, address);
    }
}

// PROVEN: b8f025a9 IDENTICAL/NONE
// OFFSET: 0x00661F50/0x140307BE0
void Battlenet::Tumor::Disconnected(uint32_t connection) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        binding->m_client->Disconnected(connection);
        binding->m_connection = 0;
        binding->m_client = NULL;
        binding->m_active = false;
    }
}

// PROVEN: 173ee0a4 IDENTICAL/NONE
// OFFSET: 0x00661F90/0x140307C50
bool Battlenet::Tumor::Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        return binding->m_client->Recv(connection, data, size, used);
    }

    return false;
}

// PROVEN: 64f976f5 IDENTICAL/NONE
// OFFSET: 0x00661FD0/0x140307CB0
bool Battlenet::Tumor::RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        return binding->m_client->RecvDatagram(connection, data, size, address);
    }

    return false;
}

// OFFSET: 0x00662010
void Battlenet::Tumor::SendEvent(const Event::Header& header, const BSN::EncodedObject* data) {
    if (header.m_destination.m_tag == 0) {
        for (int32_t i = 0; i < 20; i++) {
            if (this->m_eventBindings[i].m_active && this->m_eventBindings[i].m_eventId.m_value == header.m_destination.Value<uint32_t>()) {
                this->m_eventBindings[i].m_subscriber->RecvEvent(header, data);
            }
        }
    } else if (header.m_destination.m_tag == 1) {
        EventAdapter* adapter = this->m_eventAdapters[header.m_destination.Value<uint32_t>()];

        if (adapter) {
            static_cast<EventSubscriber*>(adapter)->RecvEvent(header, data);
        }
    }
}

// INLINED
Battlenet::Tumor::Adapter::Adapter(SystemClient& client, Tumor* tumor, uint32_t index, const uint32_t& tendrilId) {
    this->m_tendrilId = tendrilId;
    this->m_index = index;
    this->m_tumor = tumor;
    this->m_client = &client;
}

// OFFSET: 0x00662080
uint32_t Battlenet::Tumor::Adapter::Resolve(uint32_t token, const char* hostname) {
    Tumor* tumor = this->m_tumor;
    uint32_t index = this->m_index;
    TokenBinding* binding = tumor->AllocTokenBinding();

    if (!binding) {
        return 0;
    }

    binding->m_request = token;
    binding->m_client = tumor->m_adapters[index];
    binding->m_active = true;
    return tumor->m_application->Resolve(binding->m_token, hostname);
}

// PROVEN: cf8fbf4b IDENTICAL/NONE
// OFFSET: 0x006620D0
uint32_t Battlenet::Tumor::Adapter::Connect(const IP4::AddressPort& address, const uint32_t& tendrilId) {
    return this->m_tumor->Connect(this->m_index, address, tendrilId);
}

// OFFSET: 0x006620F0
void Battlenet::Tumor::Adapter::Disconnect(uint32_t connection) {
    this->m_tumor->m_application->Disconnect(connection);
}

// OFFSET: 0x00662110
bool Battlenet::Tumor::Adapter::Send(uint32_t connection, const void* data, uint32_t size) {
    return this->m_tumor->m_application->Send(connection, data, size);
}

// PROVEN: 38938bfb IDENTICAL/NONE
// OFFSET: 0x00662130
uint32_t Battlenet::Tumor::Adapter::OpenDatagram(int32_t port, const uint32_t& tendrilId) {
    return this->m_tumor->OpenDatagram(this->m_index, port, tendrilId);
}

// OFFSET: 0x00661920
void Battlenet::Tumor::Adapter::CloseDatagram(uint32_t connection) {
    this->m_tumor->m_application->CloseDatagram(connection);
}

// OFFSET: 0x00661940
bool Battlenet::Tumor::Adapter::SendDatagram(uint32_t connection, const void* data, uint32_t size, const IP4::AddressPort& address) {
    return this->m_tumor->m_application->SendDatagram(connection, data, size, address);
}

// OFFSET: 0x00661960
uint64_t Battlenet::Tumor::Adapter::GetCurrentTime() const {
    return this->m_tumor->m_application->GetCurrentTime();
}

// OFFSET: 0x00661980
uint64_t Battlenet::Tumor::Adapter::GetCurrentTick() const {
    return this->m_tumor->m_application->GetCurrentTick();
}

// OFFSET: 0x006619A0
void Battlenet::Tumor::Adapter::LogLevel(const char* message, int32_t level) {
    this->m_tumor->m_application->LogLevel(message, level);
}

// OFFSET: 0x006619C0
Battlenet::ApplicationFile* Battlenet::Tumor::Adapter::OpenApplicationFile(const char* name) {
    return this->m_tumor->m_application->OpenApplicationFile(name);
}

// OFFSET: 0x006619E0
void Battlenet::Tumor::Adapter::CloseApplicationFile(ApplicationFile* file) {
    this->m_tumor->m_application->CloseApplicationFile(file);
}

// OFFSET: 0x006618C0
Battlenet::Tumor::Adapter::~Adapter() {
}

// PROVEN: eda6052b IDENTICAL/NONE
// OFFSET: 0x00661A00
void Battlenet::Tumor::Adapter::Destroy() {
    Battlenet::Delete(this);
}

// PROVEN: 63a55997 IDENTICAL/NONE
// OFFSET: 0x00661620
void Battlenet::Tumor::Adapter::Resolved(uint32_t request, const Client::DNSInfo& info) {
    this->m_client->Resolved(request, info);
}

// PROVEN: 5a6d2eea IDENTICAL/NONE
// OFFSET: 0x00661630
void Battlenet::Tumor::Adapter::Connected(uint32_t connection, const IP4::AddressPort& address) {
    this->m_client->Connected(connection, address);
}

// PROVEN: 0a62ec3d IDENTICAL/NONE
// OFFSET: 0x00661640
void Battlenet::Tumor::Adapter::Disconnected(uint32_t connection) {
    this->m_client->Disconnected(connection);
}

// PROVEN: 44a89ee0 IDENTICAL/NONE
// OFFSET: 0x00661650
bool Battlenet::Tumor::Adapter::Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) {
    return this->m_client->Recv(connection, data, size, used);
}

// PROVEN: 9e616a2b IDENTICAL/NONE
// OFFSET: 0x00661660
bool Battlenet::Tumor::Adapter::RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) {
    return this->m_client->RecvDatagram(connection, data, size, address);
}

// PROVEN: 869f18c3 IDENTICAL/NONE
// OFFSET: 0x00661670
int32_t Battlenet::Tumor::Adapter::Process(uint32_t time) {
    return this->m_client->Process(time);
}

// PROVEN: 2d3d777b IDENTICAL/NONE
// OFFSET: 0x00661680
int32_t Battlenet::Tumor::Adapter::GetLastError() const {
    return this->m_client->GetLastError();
}

// INLINED
Battlenet::Tumor::EventAdapter::EventAdapter(BSN::FourCC eventId, EventSubscriber& subscriber, Tumor* tumor, uint32_t index, const uint32_t& tendrilId) {
    this->m_eventId = eventId;
    this->m_tendrilId = tendrilId;
    this->m_tumor = tumor;
    this->m_index = index;
    this->m_subscriber = &subscriber;
}

// PROVEN: 0df30c1a IDENTICAL/NONE
// OFFSET: 0x00662150
void Battlenet::Tumor::EventAdapter::SendEvent(uint8_t eventType, uint32_t adapter, const BSN::EncodedObject* data) {
    Event::Header header;
    header.m_id = eventType;
    header.m_sourceAddress.m_exists = true;
    header.m_sourceAddress.m_value = this->m_index;
    header.m_sourceName.m_value = this->m_eventId.m_value;
    header.m_destination.m_tag = 1;
    header.m_destination.Value<uint32_t>() = adapter;
    this->m_tumor->SendEvent(header, data);
}

// PROVEN: 9c48aa7a IDENTICAL/NONE
// OFFSET: 0x006621A0
void Battlenet::Tumor::EventAdapter::SendEvent(uint8_t eventType, BSN::FourCC eventId, const BSN::EncodedObject* data) {
    Event::Header header;
    header.m_id = eventType;
    header.m_sourceAddress.m_exists = true;
    header.m_sourceAddress.m_value = this->m_index;
    header.m_sourceName.m_value = this->m_eventId.m_value;
    header.m_destination.m_tag = 0;
    header.m_destination.Value<uint32_t>() = eventId.m_value;
    this->m_tumor->SendEvent(header, data);
}

// PROVEN: 16dcd556 IDENTICAL/NONE
// OFFSET: 0x00661690
void Battlenet::Tumor::EventAdapter::Subscribe(BSN::FourCC eventId, const uint32_t& tendrilId) {
    this->m_tumor->Subscribe(eventId, *this, tendrilId);
}

// OFFSET: 0x006618F0/0x140307040
Battlenet::Tumor::EventAdapter::~EventAdapter() {
}

// PROVEN: 046071e3 IDENTICAL/NONE
// OFFSET: 0x006616C0
void Battlenet::Tumor::EventAdapter::RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) {
    this->m_subscriber->RecvEvent(header, data);
}
