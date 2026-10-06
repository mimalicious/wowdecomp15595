#include "WoW/Source/Battlenet/Battlenet.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: #0x00DCED2C
Battlenet::Allocator* Battlenet::g_allocator;

// INLINED
Battlenet::Allocator::~Allocator() {
}

// OFFSET: #0x009C0D10
bool Battlenet::Initialize(Allocator& allocator) {
    Battlenet::g_allocator = &allocator;
    return true;
}

// OFFSET: #0x009C0D20
void Battlenet::Teardown() {
    Battlenet::g_allocator = NULL;
}

// OFFSET: #0x009BFEA0
uint32_t Battlenet::StringCopy(void* dest, const char* source, uint32_t size) {
    if (!size) {
        return 0;
    }

    size--;
    uint32_t length = static_cast<uint32_t>(strlen(source));

    if (size > length) {
        size = length;
    }

    memcpy(dest, source, size);
    static_cast<char*>(dest)[size] = '\0';
    return size;
}

// INLINED
Battlenet::Event::Header::Header() {
    this->m_source.m_exists = false;
    this->m_source.m_adapter = 0;
    this->m_source.m_event = static_cast<uint32_t>(-1);
    this->m_target.m_type = 0;
}

// OFFSET: #0x009BFD80
Battlenet::SystemClient::~SystemClient() {
}

// OFFSET: #0x008C50C0
void Battlenet::SystemServer::Log(const char* message) {
    this->LogLevel(message, 5);
}

// OFFSET: #0x008C50E0
Battlenet::SystemServer::~SystemServer() {
}

// INLINED
Battlenet::EventSubscriber::EventSubscriber() {
    this->m_eventId.m_value = 0;
}

// OFFSET: #0x008C5110
Battlenet::EventSubscriber::~EventSubscriber() {
}

// OFFSET: #0x009C0C40
Battlenet::EventInterface::~EventInterface() {
}

// OFFSET: #0x009C2880
Battlenet::Library::~Library() {
}

// OFFSET: #0x009C29C0
Battlenet::Library* Battlenet::Library::LoadApplicationBundle(const char* name, Allocator& allocator) {
    uint32_t length = SStrLen(name);

    if (length + 5 < length) {
        return NULL;
    }

    char* path = static_cast<char*>(allocator.Alloc(length + 5));

    if (!path) {
        return NULL;
    }

    memcpy(path, name, length);
    strcpy(&path[length], ".dll");

    Library* library = Battlenet::Library::Load(path, allocator);
    allocator.Free(path);
    return library;
}

// OFFSET: #0x009C2A70
void Battlenet::WindowsLibrary::Destroy() {
    Allocator* allocator = this->m_allocator;
    this->~WindowsLibrary();
    allocator->Free(this);
}

// OFFSET: #0x009C2010
bool Battlenet::GetProc(Library* library, void*& proc, uint32_t ordinal) {
    proc = library->GetProc(ordinal);
    return proc != NULL;
}
