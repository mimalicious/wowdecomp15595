#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_SHAREDMEMORY_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_SHAREDMEMORY_H

#include <cstdint>
#include <string>

namespace Blizzard {
namespace Memory {

class SharedMemory {
    public:
    /* 0000 */ std::string m_name;
    /* 001C */ uint32_t m_size;
    /* 0020 */ void* m_mapping;
    /* 0024 */ void* m_data;
};

void CloseSharedMemory(void* memory);
bool MakeSharedMemoryName(const char* path, uint32_t size, char* name);
int32_t OpenSharedMemory(const char* path, uint32_t size, SharedMemory** memory);

}
}

#endif
