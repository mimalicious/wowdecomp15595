#include "WoW/Source/Battlenet/Tumor/TumorManager.h"
#include <cstring>
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Battlenet/Tumor/CreepTendril.h"

// STATIC: 0x01008DDC
BSN::FourCC Battlenet::Event::CREEP("Crep");

// STATIC: 0x01008DE0
BSN::FourCC Battlenet::Event::APP("App");

BSN::FourCC Battlenet::Event::DATAGRAM_CONNECTION_ACK("DCAC");

// STATIC: 0x01008DE8
BSN::FourCC Battlenet::Event::S2_LOBBY("S2Lb");

BSN::FourCC Battlenet::Event::COMSAT_UPDATE("CMSU");

BSN::FourCC Battlenet::Event::TO_UI("ToUI");

BSN::FourCC Battlenet::Event::COMSAT_STATE("CMST");

BSN::FourCC Battlenet::Event::FROM_UI("FmUI");

// STATIC: 0x01008DFC
BSN::FourCC Battlenet::Event::DATAGRAM_CONNECTION_INIT("DCDI");

// PROVEN: 40b7f33f IDENTICAL/NONE
// OFFSET: 0x00660670
void Battlenet::TumorManagerRegisterFunction(void* param, const char* name, GeneralFunction::Function func, void* funcParam) {
    static_cast<TumorManager*>(param)->RegisterFunction(name, func, funcParam);
}

// PROVEN: 49cf5298 IDENTICAL/NONE
// OFFSET: 0x00660690/0x140305D50
Battlenet::Tumor* Battlenet::TumorManager::GetTumor() {
    return &this->m_tumor;
}

// OFFSET: 0x006606A0
void Battlenet::TumorManager::AddTendril(Tendril* tendril) {
    uint32_t i = 0;

    while (this->m_tendrils[i]) {
        if (++i >= 20) {
            DebugLog("No room for new tendril!");
            return;
        }
    }

    this->m_tendrils[i] = tendril;
    tendril->DoSubscriptions();
}

// PROVEN: 4f843ee0 IDENTICAL/NONE
// OFFSET: 0x006606F0
void Battlenet::TumorManager::DestroyTendrils() {
    for (int32_t i = 0; i < 20; i++) {
        if (this->m_tendrils[i]) {
            this->m_tendrils[i]->Destroy();
            this->m_tendrils[i] = NULL;
        }
    }
}

// PROVEN: 81c2e84d IDENTICAL/NONE
// OFFSET: 0x00660720/0x140305D60
void Battlenet::TumorManager::SendEvent(uint8_t eventType, const uint32_t& adapter, const BSN::EncodedObject* data) {
    this->m_eventAdapter->SendEvent(eventType, adapter, data);
}

// PROVEN: 269144f0 IDENTICAL/NONE
// OFFSET: 0x00660740/0x140305D70
void Battlenet::TumorManager::SendEvent(uint8_t eventType, const BSN::FourCC& eventId, const BSN::EncodedObject* data) {
    this->m_eventAdapter->SendEvent(eventType, eventId, data);
}

// OFFSET: 0x00660A40
GeneralFunction::Arg Battlenet::TumorManagerSendEvent(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    if (argc != 3) {
        DebugLog("Usage: SendEvent(eventId, dest, event)");
        return GeneralFunction::Arg();
    }

    const char* target = args[1].ToString();
    BSN::FourCC eventId;
    eventId.m_value = 0;

    for (uint8_t c = *target; *target; c = *target) {
        target++;
        eventId.m_value = c | (eventId.m_value << 8);
    }

    const BSN::EncodedObject* data = args[2].m_type == GeneralFunction::Arg::TYPE_OBJECT ? args[2].m_object : NULL;
    static_cast<TumorManager*>(param)->SendEvent(static_cast<uint8_t>(args[0].ToUInt64()), eventId, data);
    return GeneralFunction::Arg();
}

// OFFSET: 0x00660AE0/0x140306110
Battlenet::TumorManager::TumorManager(Application& application) : m_tumor(application) {
    this->m_eventAdapter = NULL;
    this->m_tumor.AllocTendrilId(this->m_tendrilId);
    this->m_eventAdapter = this->m_tumor.AllocEventAdapter(Battlenet::Event::APP, application, this->m_tendrilId);
    memset(this->m_tendrils, 0, sizeof(this->m_tendrils));
}

// OFFSET: 0x00660B50/0x1403061B0
Battlenet::TumorManager::~TumorManager() {
}

// OFFSET: 0x006613C0/0x140306D00
Battlenet::TumorManager* Battlenet::TumorManager::Create(Application& application, const Authentication::Settings& settings) {
    void* memory = Battlenet::g_allocator->Alloc(sizeof(TumorManagerImplementation), ".\\TumorManager.cpp(122):TumorManagerImplementation");
    TumorManagerImplementation* manager = memory ? new (memory) TumorManagerImplementation(application) : NULL;

    CreepTendril::Instance* creep = CreepTendril::Instance::Create(settings, *manager);

    if (!creep) {
        Battlenet::Delete(manager);
        return NULL;
    }

    manager->AddTendril(creep);
    manager->m_eventAdapter->Subscribe(Battlenet::Event::APP, manager->m_tendrilId);
    manager->m_eventAdapter->Subscribe(Battlenet::Event::TO_UI, manager->m_tendrilId);
    manager->RegisterFunction("SendEvent", Battlenet::TumorManagerSendEvent, manager);
    creep->EnumFunctions(Battlenet::TumorManagerRegisterFunction, manager);
    return manager;
}

// INLINED
Battlenet::TumorManagerImplementation::TumorManagerImplementation(Application& application) : TumorManager(application) {
}

// OFFSET: 0x00660F10/0x140306500
void Battlenet::TumorManagerImplementation::EnumFunctions(EnumFunctionsCallback callback, void* param) {
    MapKey key;
    FunctionEntry entry = { NULL, NULL };
    uint32_t iterator = 0;

    while (this->m_functions.Next(iterator, key, entry)) {
        callback(param, key.m_value.GetString(), entry.m_func, entry.m_param);
    }
}

// OFFSET: 0x00660FB0/0x140306620
GeneralFunction::Arg Battlenet::TumorManagerImplementation::CallFunction(const char* name, int32_t argc, GeneralFunction::Arg* args) {
    GeneralFunction::Function func = NULL;
    void* param = NULL;
    uint32_t index;
    bool found;

    {
        MapKey key((String(name)));

        if (this->m_functions.Find(key, index)) {
            func = this->m_functions.m_entries[index].m_value.m_func;
            param = this->m_functions.m_entries[index].m_value.m_param;
            found = true;
        } else {
            found = false;
        }
    }

    if (!found) {
        return GeneralFunction::Arg();
    }

    return func(param, argc, args);
}

// OFFSET: 0x00661370/0x140306CB0
Battlenet::TumorManagerImplementation::~TumorManagerImplementation() {
    this->DestroyTendrils();
}

// OFFSET: 0x006614A0/0x140306E60
void Battlenet::TumorManagerImplementation::RegisterFunction(const char* name, GeneralFunction::Function func, void* param) {
    MapKey key((String(name)));
    FunctionEntry& entry = this->m_functions[key];
    entry.m_func = func;
    entry.m_param = param;
}

// OFFSET: 0x00661520/0x140306ED0
void Battlenet::TumorManagerImplementation::UnregisterFunction(const char* name, GeneralFunction::Function func, void* param) {
    GeneralFunction::Function registeredFunc = NULL;
    void* registeredParam = NULL;
    uint32_t index;
    bool found;

    {
        MapKey key((String(name)));

        if (this->m_functions.Find(key, index)) {
            registeredFunc = this->m_functions.m_entries[index].m_value.m_func;
            registeredParam = this->m_functions.m_entries[index].m_value.m_param;
            found = true;
        } else {
            found = false;
        }
    }

    if (found && registeredFunc == func && registeredParam == param) {
        MapKey key(name);
        FunctionEntry removed;
        this->m_functions.Remove(key, removed);
    }
}
