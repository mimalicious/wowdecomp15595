#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00EC6B14
CStringManager* CStringManager::s_stringManager;

// INLINED
RCString::RCString() {
    this->Copy(NULL);
}

// INLINED
RCString::~RCString() {
}

// PROVEN: bca1a0cc IDENTICAL/IDENTICAL
// OFFSET: 0x00872000/0x1405A8300
void RCString::Copy(const RCString& source) {
    this->m_str = source.m_str;
}

// INLINED
CStringMemory::CStringMemory() {
    this->AllocateBlock(0);
}

// PROVEN: 04fc1c81 IDENTICAL/NONE
// OFFSET: 0x00872010/0x1405A8310
CStringMemory::~CStringMemory() {
    for (uint32_t i = 0; i < this->Count(); i++) {
        SMemFree(this->m_data[i], __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x00872060/0x1405A8390
void CStringMemory::AllocateBlock(uint32_t size) {
    if (size < 0x10000) {
        size = 0x10000;
    }

    uint32_t index = this->Count();
    this->SetCount(index + 1);
    (*this)[index] = SMemAlloc(size, __FILE__, __LINE__, 0);
    this->unk_0010 = static_cast<char*>((*this)[index]);
    this->unk_0014 = this->unk_0010 + size;
}

// PROVEN: a0e51e37 IDENTICAL/NONE
// OFFSET: 0x00872100/0x1405A8410
char* CStringMemory::New(const char* str) {
    uint32_t bytes = (SStrLen(str) + sizeof(char*) + 4) & ~3;

    if (this->unk_0010 + bytes > this->unk_0014) {
        this->AllocateBlock(bytes);
    }

    char* entry = this->unk_0010;
    SStrCopy(entry + sizeof(char*), str, STORM_MAX_STR);
    this->unk_0010 += bytes;

    return entry;
}

// OFFSET: #0x0047CE90
const char* CStringManager::Add(const char* str) {
    uint32_t index = SStrHashHT(str) % 521;

    for (char* entry = this->unk_0000[index]; entry; entry = *reinterpret_cast<char**>(entry)) {
        if (!SStrCmp(str, entry + sizeof(char*), STORM_MAX_STR)) {
            return entry + sizeof(char*);
        }
    }

    char* entry = this->unk_0824.New(str);
    *reinterpret_cast<char**>(entry) = this->unk_0000[index];
    this->unk_0000[index] = entry;

    return entry + sizeof(char*);
}

// PROVEN: bb1b8802 IDENTICAL/NONE
// OFFSET: 0x008721F0
CStringManager::CStringManager() {
    memset(this->unk_0000, 0, sizeof(this->unk_0000));
}

// PROVEN: 2e1f7de4 IDENTICAL/NONE
// OFFSET: 0x00871FF0/0x1405A82F0
int32_t CStringManager::IsDestroyed() {
    return CStringManager::s_stringManager == NULL;
}

// OFFSET: 0x00872230/0x1405A8650
void CStringManager::DestroyManager() {
    delete CStringManager::s_stringManager;
    CStringManager::s_stringManager = NULL;
}

// INLINED
CStringManager* CStringManager::Get() {
    if (!CStringManager::s_stringManager) {
        void* mem = SMemAlloc(sizeof(CStringManager), __FILE__, __LINE__, 0);
        CStringManager::s_stringManager = mem ? new (mem) CStringManager() : NULL;
    }

    return CStringManager::s_stringManager;
}

// OFFSET: 0x00872270/0x1405A86A0
void RCString::Copy(const char* str) {
    if (!str) {
        this->m_str = NULL;
        return;
    }

    CStringManager* pManager = CStringManager::Get();

    if (!pManager) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    this->m_str = pManager->Add(str);
}
