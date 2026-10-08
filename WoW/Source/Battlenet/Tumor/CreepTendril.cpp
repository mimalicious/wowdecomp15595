#include "WoW/Source/Battlenet/Tumor/CreepTendril.h"
#include <cstring>
#include <malloc.h>
#include <new>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: #0x00DCED30
uint32_t Battlenet::CreepTendril::s_instances;

// STATIC: #0x00DCED34
Battlenet::CreepTendril::CreepInitializeFunc Battlenet::CreepTendril::s_creepInitialize;

// STATIC: #0x00DCED38
Battlenet::CreepTendril::CreepTeardownFunc Battlenet::CreepTendril::s_creepTeardown;

// STATIC: #0x00DCED3C
Battlenet::CreepTendril::CreepCreateFunc Battlenet::CreepTendril::s_creepCreate;

// STATIC: #0x00DCED40
Battlenet::CreepTendril::CreepDestroyFunc Battlenet::CreepTendril::s_creepDestroy;

// STATIC: #0x00DCED44
Battlenet::Library* Battlenet::CreepTendril::s_creepModule;

// OFFSET: 0x00662290
Battlenet::CreepTendril::Host::~Host() {
}

// PROVEN: 235c13d3 IDENTICAL/NONE
// OFFSET: 0x00662270
void Battlenet::CreepTendril::Host::Log(const char* message) {
    this->LogLevel(message, 5);
}

// OFFSET: 0x00662EB0
bool Battlenet::CreepTendril::Initialize() {
    if (Battlenet::CreepTendril::s_creepModule) {
        return true;
    }

    Battlenet::CreepTendril::s_creepModule = Battlenet::Library::LoadApplicationBundle("Battle.net-prearxan", *Battlenet::g_allocator);

    if (!Battlenet::CreepTendril::s_creepModule) {
        Battlenet::CreepTendril::s_creepModule = Battlenet::Library::LoadApplicationBundle("Battle.net", *Battlenet::g_allocator);

        if (!Battlenet::CreepTendril::s_creepModule) {
            return false;
        }
    }

    Battlenet::CreepTendril::s_creepInitialize = reinterpret_cast<CreepInitializeFunc>(Battlenet::CreepTendril::s_creepModule->GetProc(1u));

    if (Battlenet::CreepTendril::s_creepInitialize) {
        Battlenet::CreepTendril::s_creepTeardown = reinterpret_cast<CreepTeardownFunc>(Battlenet::CreepTendril::s_creepModule->GetProc(2u));

        if (Battlenet::CreepTendril::s_creepTeardown) {
            Battlenet::CreepTendril::s_creepCreate = reinterpret_cast<CreepCreateFunc>(Battlenet::CreepTendril::s_creepModule->GetProc(3u));

            if (Battlenet::CreepTendril::s_creepCreate) {
                if (Battlenet::GetProc(Battlenet::CreepTendril::s_creepModule, reinterpret_cast<void*&>(Battlenet::CreepTendril::s_creepDestroy), 4) && Battlenet::CreepTendril::s_creepInitialize()) {
                    return true;
                }
            }
        }
    }

    Battlenet::CreepTendril::s_creepModule->Destroy();
    Battlenet::CreepTendril::s_creepModule = NULL;
    return false;
}

// OFFSET: 0x00663030/0x140308F50
Battlenet::CreepTendril::Instance::Instance(const Authentication::Settings& settings, TumorManager& manager) : Tendril(Battlenet::Event::CREEP, manager), m_creep(NULL), m_settings(settings) {
    Battlenet::CreepTendril::s_instances++;

    BSN::Hard::SizeEncoder sizeEncoder;
    ProtocolHard::TypeInfoOf<Authentication::Settings>::Encode(this->m_settings, sizeEncoder);

    uint32_t size = (sizeEncoder.m_size + 7) >> 3;
    BSN::Hard::Encoder encoder;
    encoder.SetData(_alloca(size), size);
    ProtocolHard::TypeInfoOf<Authentication::Settings>::Encode(this->m_settings, encoder);

    this->m_creep = Battlenet::CreepTendril::s_creepCreate(this, encoder.m_data, (encoder.m_position + 7) >> 3);

    this->m_eventHandlers[1] = &Instance::ProcessEventData<1>;
    this->m_eventHandlers[2] = &Instance::ProcessEventData<2>;
    this->m_eventHandlers[3] = &Instance::ProcessEventData<3>;
    this->m_eventHandlers[10] = &Instance::ProcessEventData<10>;
    this->m_eventHandlers[11] = &Instance::ProcessEventData<11>;
    this->m_eventHandlers[5] = &Instance::ProcessEventData<5>;
    this->m_eventHandlers[6] = &Instance::ProcessEventData<6>;
    this->m_eventHandlers[9] = &Instance::ProcessEventData<9>;
    this->m_eventHandlers[8] = &Instance::ProcessEventData<8>;
    this->m_eventHandlers[14] = &Instance::ProcessEventData<14>;
    this->m_eventHandlers[15] = &Instance::ProcessEventData<15>;
}

// OFFSET: 0x006632A0/0x140309360
Battlenet::CreepTendril::Instance* Battlenet::CreepTendril::Instance::Create(const Authentication::Settings& settings, TumorManager& manager) {
    if (!Battlenet::CreepTendril::s_instances && !Battlenet::CreepTendril::Initialize()) {
        return NULL;
    }

    void* memory = Battlenet::g_allocator->Alloc(sizeof(Instance), ".\\CreepTendril.cpp(185):Instance");

    if (!memory) {
        return NULL;
    }

    return new (memory) Instance(settings, manager);
}

// OFFSET: [#0x009C1A40, #0x009C1C50, #0x009C1CF0]
Battlenet::CreepTendril::Instance::~Instance() {
    Battlenet::CreepTendril::s_creepDestroy(this->m_creep);
    this->m_creep = NULL;

    if (!--Battlenet::CreepTendril::s_instances && Battlenet::CreepTendril::s_creepModule) {
        Battlenet::CreepTendril::s_creepTeardown();
        Battlenet::CreepTendril::s_creepModule->Destroy();
        Battlenet::CreepTendril::s_creepModule = NULL;
    }
}

// OFFSET: #0x009C1CC0
void Battlenet::CreepTendril::Instance::Destroy() {
    Battlenet::Delete(this);
}

// PROVEN: 8ea34d2a IDENTICAL/NONE
// OFFSET: 0x006624E0/0x140308290
void Battlenet::CreepTendril::Instance::EnumFunctions(EnumFunctionsCallback callback, void* param) {
    this->m_creep->EnumFunctions(callback, param);
}

// OFFSET: 0x00662520
void Battlenet::CreepTendril::Instance::DoSubscriptions() {
    this->m_eventAdapter->Subscribe(Battlenet::Event::DATAGRAM_CONNECTION_INIT, this->m_tendrilId);
    this->m_eventAdapter->Subscribe(Battlenet::Event::S2_LOBBY, this->m_tendrilId);
}

// OFFSET: 0x00662F80
void Battlenet::CreepTendril::Instance::RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) {
    BSN::BSNEncodedObjectPtr<Event::Header, ::Protocol, 0> object(&header);
    object.IncRef();
    this->m_creep->RecvEvent(&object, data);
}

// OFFSET: 0x00662780
void Battlenet::CreepTendril::Instance::Resolved(uint32_t request, const Client::DNSInfo& info) {
    BSN::Hard::SizeEncoder sizeEncoder;
    ProtocolHard::TypeInfoOf<Client::DNSInfo>::Encode(info, sizeEncoder);

    BSN::Hard::Encoder encoder;
    encoder.SetData(_alloca(sizeEncoder.m_size), sizeEncoder.m_size);
    ProtocolHard::TypeInfoOf<Client::DNSInfo>::Encode(info, encoder);

    this->m_creep->Resolved(request, encoder.m_data, (encoder.m_position + 7) >> 3);
}

// OFFSET: 0x00662810
void Battlenet::CreepTendril::Instance::Connected(uint32_t connection, const IP4::AddressPort& address) {
    BSN::Hard::SizeEncoder sizeEncoder;
    ProtocolHard::TypeInfoOf<IP4::AddressPort>::Encode(address, sizeEncoder);

    BSN::Hard::Encoder encoder;
    encoder.SetData(_alloca(sizeEncoder.m_size), sizeEncoder.m_size);
    ProtocolHard::TypeInfoOf<IP4::AddressPort>::Encode(address, encoder);

    this->m_creep->Connected(connection, encoder.m_data, (encoder.m_position + 7) >> 3);
}

// PROVEN: 566ce43c IDENTICAL/NONE
// OFFSET: 0x006624F0
void Battlenet::CreepTendril::Instance::Disconnected(uint32_t connection) {
    this->m_creep->Disconnected(connection);
}

// PROVEN: 826b8b83 IDENTICAL/NONE
// OFFSET: 0x00662500
bool Battlenet::CreepTendril::Instance::Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) {
    return this->m_creep->Recv(connection, data, size, used);
}

// OFFSET: 0x006628A0
bool Battlenet::CreepTendril::Instance::RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) {
    BSN::Hard::SizeEncoder sizeEncoder;
    ProtocolHard::TypeInfoOf<IP4::AddressPort>::Encode(address, sizeEncoder);

    uint32_t addressSize = sizeEncoder.m_size;
    uint8_t* buffer = static_cast<uint8_t*>(_alloca(addressSize));

    BSN::Hard::Encoder encoder;
    encoder.SetData(buffer, addressSize);
    ProtocolHard::TypeInfoOf<IP4::AddressPort>::Encode(address, encoder);

    return this->m_creep->RecvDatagram(connection, data, size, buffer, addressSize);
}

// PROVEN: 1e19656d IDENTICAL/NONE
// OFFSET: 0x00662510
int32_t Battlenet::CreepTendril::Instance::Process(uint32_t time) {
    return this->m_creep->Process();
}

// PROVEN: 5217c9d3 IDENTICAL/NONE
// OFFSET: 0x006624D0
int32_t Battlenet::CreepTendril::Instance::GetLastError() const {
    return this->m_creep->GetLastError();
}

// PROVEN: c23f21ff IDENTICAL/NONE
// OFFSET: 0x00662550
uint32_t Battlenet::CreepTendril::Instance::Resolve(uint32_t token, const char* hostname) {
    return this->m_adapter->Resolve(token, hostname);
}

// OFFSET: 0x00662930
uint32_t Battlenet::CreepTendril::Instance::Connect(const void* address, uint32_t addressSize) {
    BSN::Hard::Decoder decoder;
    decoder.SetData(address, addressSize);

    IP4::AddressPort decoded;

    if (ProtocolHard::TypeInfoOf<IP4::AddressPort>::Decode(decoded, decoder)) {
        DebugLog("Bad address from Creep");
        return 0;
    }

    return this->m_adapter->Connect(decoded, this->m_tendrilId);
}

// PROVEN: ab571ee2 IDENTICAL/NONE
// OFFSET: 0x00662560
void Battlenet::CreepTendril::Instance::Disconnect(uint32_t connection) {
    this->m_adapter->Disconnect(connection);
}

// PROVEN: 316be5ee IDENTICAL/NONE
// OFFSET: 0x00662570
bool Battlenet::CreepTendril::Instance::Send(uint32_t connection, const void* data, uint32_t size) {
    return this->m_adapter->Send(connection, data, size);
}

// PROVEN: cb1c06fd IDENTICAL/NONE
// OFFSET: 0x00662580
uint32_t Battlenet::CreepTendril::Instance::OpenDatagram(int32_t port) {
    return this->m_adapter->OpenDatagram(port, this->m_tendrilId);
}

// PROVEN: cd29b7a0 IDENTICAL/NONE
// OFFSET: 0x006625A0
void Battlenet::CreepTendril::Instance::CloseDatagram(uint32_t connection) {
    this->m_adapter->CloseDatagram(connection);
}

// OFFSET: 0x006629B0
bool Battlenet::CreepTendril::Instance::SendDatagram(uint32_t connection, const void* data, uint32_t size, const void* address, uint32_t addressSize) {
    BSN::Hard::Decoder decoder;
    decoder.SetData(address, addressSize);

    IP4::AddressPort decoded;

    if (ProtocolHard::TypeInfoOf<IP4::AddressPort>::Decode(decoded, decoder)) {
        DebugLog("Bad address from Creep");
        return false;
    }

    return this->m_adapter->SendDatagram(connection, data, size, decoded);
}

// PROVEN: 5d9782c8 IDENTICAL/NONE
// OFFSET: 0x006625B0
uint64_t Battlenet::CreepTendril::Instance::GetCurrentTime() const {
    return this->m_adapter->GetCurrentTime();
}

// PROVEN: 0057ad59 IDENTICAL/NONE
// OFFSET: 0x006625C0
uint64_t Battlenet::CreepTendril::Instance::GetCurrentTick() const {
    return this->m_adapter->GetCurrentTick();
}

// PROVEN: f8c386ce IDENTICAL/NONE
// OFFSET: 0x006625D0
void Battlenet::CreepTendril::Instance::LogLevel(const char* message, int32_t level) {
    this->m_adapter->LogLevel(message, level);
}

// PROVEN: 2306d5ed IDENTICAL/NONE
// OFFSET: 0x006625E0
Battlenet::ApplicationFile* Battlenet::CreepTendril::Instance::OpenApplicationFile(const char* name) {
    return this->m_adapter->OpenApplicationFile(name);
}

// PROVEN: e45daf35 IDENTICAL/NONE
// OFFSET: 0x006625F0
void Battlenet::CreepTendril::Instance::CloseApplicationFile(ApplicationFile* file) {
    this->m_adapter->CloseApplicationFile(file);
}

// OFFSET: 0x006631C0
void Battlenet::CreepTendril::Instance::SendEvent(const BSN::EncodedObject* header, const BSN::EncodedObject* data) {
    BSN::BSNEncodedObject<Event::Header, ::Protocol> decoded;

    if (!decoded.EncodedObject::WalkAndGet(header)) {
        DebugLog("Bad event from Creep");
        return;
    }

    EventHandler handler = this->m_eventHandlers[decoded.m_data.m_id];
    (this->*handler)(decoded.m_data, data);
}
