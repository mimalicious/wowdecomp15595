#include "WoW/Source/Battlenet/Battlenet.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00E4D3F0/0x140D361D8
Battlenet::Allocator* Battlenet::g_allocator;

// OFFSET: 0x006616D0/0x140307140
bool Battlenet::Initialize(Allocator& allocator) {
    Battlenet::g_allocator = &allocator;
    return true;
}

// OFFSET: 0x006616E0/0x140307150
void Battlenet::Teardown() {
    Battlenet::g_allocator = NULL;
}

// OFFSET: 0x00660760
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

// OFFSET: 0x00627330/0x1402B9370
void Battlenet::SystemServer::Log(const char* message) {
    this->LogLevel(message, 5);
}

// OFFSET: 0x006632F0/0x1403093D0
Battlenet::Library::~Library() {
}

// OFFSET: 0x00663440/0x140309540
Battlenet::Library* Battlenet::Library::LoadApplicationBundle(const char* name, Allocator& allocator) {
    uint32_t length = SStrLen(name);

    if (length + 5 < length) {
        return NULL;
    }

    char* path = static_cast<char*>(allocator.Alloc(length + 5, ".\\LibraryWin32.cpp(176)"));

    if (!path) {
        return NULL;
    }

    memcpy(path, name, length);
    strcpy(&path[length], ".dll");

    Library* library = Battlenet::Library::Load(path, allocator);
    allocator.Free(path);
    return library;
}

// OFFSET: 0x00663510/0x140309650
void Battlenet::WindowsLibrary::Destroy() {
    Allocator* allocator = this->m_allocator;
    this->~WindowsLibrary();
    allocator->Free(this);
}

// OFFSET: 0x00662A60
bool Battlenet::GetProc(Library* library, void*& proc, uint32_t ordinal) {
    proc = library->GetProc(ordinal);
    return proc != NULL;
}
