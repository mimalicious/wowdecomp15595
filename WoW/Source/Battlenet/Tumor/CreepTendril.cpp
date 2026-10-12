#include "WoW/Source/Battlenet/Tumor/CreepTendril.h"
#include <cstring>
#include <malloc.h>
#include <new>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00E4D3F4/0x140D361E0
uint32_t Battlenet::CreepTendril::s_instances;

// STATIC: 0x00E4D3F8/0x140D361E8
Battlenet::CreepTendril::CreepInitializeFunc Battlenet::CreepTendril::s_creepInitialize;

// STATIC: 0x00E4D3FC/0x140D361F0
Battlenet::CreepTendril::CreepTeardownFunc Battlenet::CreepTendril::s_creepTeardown;

// STATIC: 0x00E4D400/0x140D361F8
Battlenet::CreepTendril::CreepCreateFunc Battlenet::CreepTendril::s_creepCreate;

// STATIC: 0x00E4D404/0x140D36200
Battlenet::CreepTendril::CreepDestroyFunc Battlenet::CreepTendril::s_creepDestroy;

// STATIC: 0x00E4D408/0x140D36208
Battlenet::Library* Battlenet::CreepTendril::s_creepModule;

// OFFSET: 0x00662290/0x140307FB0
Battlenet::CreepTendril::Host::~Host() {
}

// OFFSET: 0x00662270/0x140307FA0
void Battlenet::CreepTendril::Host::Log(const char* message) {
    this->LogLevel(message, 5);
}

// OFFSET: 0x00662EB0/0x140308D80
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

// OFFSET: [0x00662430/0x1403081D0, 0x006626C0]
Battlenet::CreepTendril::Instance::~Instance() {
    Battlenet::CreepTendril::s_creepDestroy(this->m_creep);
    this->m_creep = NULL;

    if (!--Battlenet::CreepTendril::s_instances && Battlenet::CreepTendril::s_creepModule) {
        Battlenet::CreepTendril::s_creepTeardown();
        Battlenet::CreepTendril::s_creepModule->Destroy();
        Battlenet::CreepTendril::s_creepModule = NULL;
    }
}

// OFFSET: 0x00662690/0x140308440
void Battlenet::CreepTendril::Instance::Destroy() {
    Battlenet::Delete(this);
}

// OFFSET: 0x006624E0/0x140308290
void Battlenet::CreepTendril::Instance::EnumFunctions(EnumFunctionsCallback callback, void* param) {
    this->m_creep->EnumFunctions(callback, param);
}

// OFFSET: 0x00662520/0x1403082D0
void Battlenet::CreepTendril::Instance::DoSubscriptions() {
    this->m_eventAdapter->Subscribe(Battlenet::Event::DATAGRAM_CONNECTION_INIT, this->m_tendrilId);
    this->m_eventAdapter->Subscribe(Battlenet::Event::S2_LOBBY, this->m_tendrilId);
}

// OFFSET: 0x00662F80/0x140308E80
void Battlenet::CreepTendril::Instance::RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) {
    BSN::BSNEncodedObjectPtr<Event::Header, ::Protocol, 0> object(&header);
    object.IncRef();
    this->m_creep->RecvEvent(&object, data);
}

// OFFSET: 0x00662780/0x140308580
void Battlenet::CreepTendril::Instance::Resolved(uint32_t request, const Client::DNSInfo& info) {
    BSN::Hard::SizeEncoder sizeEncoder;
    ProtocolHard::TypeInfoOf<Client::DNSInfo>::Encode(info, sizeEncoder);

    BSN::Hard::Encoder encoder;
    encoder.SetData(_alloca(sizeEncoder.m_size), sizeEncoder.m_size);
    ProtocolHard::TypeInfoOf<Client::DNSInfo>::Encode(info, encoder);

    this->m_creep->Resolved(request, encoder.m_data, (encoder.m_position + 7) >> 3);
}

// OFFSET: 0x00662810/0x140308640
void Battlenet::CreepTendril::Instance::Connected(uint32_t connection, const IP4::AddressPort& address) {
    BSN::Hard::SizeEncoder sizeEncoder;
    ProtocolHard::TypeInfoOf<IP4::AddressPort>::Encode(address, sizeEncoder);

    BSN::Hard::Encoder encoder;
    encoder.SetData(_alloca(sizeEncoder.m_size), sizeEncoder.m_size);
    ProtocolHard::TypeInfoOf<IP4::AddressPort>::Encode(address, encoder);

    this->m_creep->Connected(connection, encoder.m_data, (encoder.m_position + 7) >> 3);
}

// OFFSET: 0x006624F0/0x1403070E0
void Battlenet::CreepTendril::Instance::Disconnected(uint32_t connection) {
    this->m_creep->Disconnected(connection);
}

// OFFSET: 0x00662500/0x1403082A0
bool Battlenet::CreepTendril::Instance::Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) {
    return this->m_creep->Recv(connection, data, size, used);
}

// OFFSET: 0x006628A0/0x140308700
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

// OFFSET: 0x00662510/0x1403082C0
int32_t Battlenet::CreepTendril::Instance::Process(uint32_t time) {
    return this->m_creep->Process();
}

// OFFSET: 0x006624D0/0x140308260
int32_t Battlenet::CreepTendril::Instance::GetLastError() const {
    return this->m_creep->GetLastError();
}

// OFFSET: 0x00662550/0x140308310
uint32_t Battlenet::CreepTendril::Instance::Resolve(uint32_t token, const char* hostname) {
    return this->m_adapter->Resolve(token, hostname);
}

// OFFSET: 0x00662930/0x1403087E0
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

// OFFSET: 0x00662560/0x140308320
void Battlenet::CreepTendril::Instance::Disconnect(uint32_t connection) {
    this->m_adapter->Disconnect(connection);
}

// OFFSET: 0x00662570/0x140308330
bool Battlenet::CreepTendril::Instance::Send(uint32_t connection, const void* data, uint32_t size) {
    return this->m_adapter->Send(connection, data, size);
}

// OFFSET: 0x00662580/0x140308340
uint32_t Battlenet::CreepTendril::Instance::OpenDatagram(int32_t port) {
    return this->m_adapter->OpenDatagram(port, this->m_tendrilId);
}

// OFFSET: 0x006625A0/0x140308360
void Battlenet::CreepTendril::Instance::CloseDatagram(uint32_t connection) {
    this->m_adapter->CloseDatagram(connection);
}

// OFFSET: 0x006629B0/0x140308860
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

// OFFSET: 0x006625B0/0x140308370
uint32_t Battlenet::CreepTendril::Instance::GetCurrentTime() const {
    return this->m_adapter->GetCurrentTime();
}

// OFFSET: 0x006625C0/0x140308380
uint64_t Battlenet::CreepTendril::Instance::GetCurrentTick() const {
    return this->m_adapter->GetCurrentTick();
}

// OFFSET: 0x006625D0/0x140308390
void Battlenet::CreepTendril::Instance::LogLevel(const char* message, int32_t level) {
    this->m_adapter->LogLevel(message, level);
}

// OFFSET: 0x006625E0/0x1403083A0
Battlenet::ApplicationFile* Battlenet::CreepTendril::Instance::OpenApplicationFile(const char* name) {
    return this->m_adapter->OpenApplicationFile(name);
}

// OFFSET: 0x006625F0/0x1403083B0
bool Battlenet::CreepTendril::Instance::CloseApplicationFile(ApplicationFile* file) {
    return this->m_adapter->CloseApplicationFile(file);
}

// OFFSET: 0x006631C0/0x140309220
void Battlenet::CreepTendril::Instance::SendEvent(const BSN::EncodedObject* header, const BSN::EncodedObject* data) {
    BSN::BSNEncodedObject<Event::Header, ::Protocol> decoded;

    if (!decoded.EncodedObject::WalkAndGet(header)) {
        DebugLog("Bad event from Creep");
        return;
    }

    EventHandler handler = this->m_eventHandlers[decoded.m_data.m_id];
    (this->*handler)(decoded.m_data, data);
}
