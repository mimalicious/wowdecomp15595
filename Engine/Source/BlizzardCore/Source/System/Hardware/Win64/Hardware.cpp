#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstring>
#include <intrin.h>
#include <windows.h>
#include <rpc.h>
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// STATIC: %0x00D82B7C
Blizzard::Hardware::ProcessorFeatures System_Hardware::s_processorFeatures;

// STATIC: %0x00D871A0
Blizzard::Hardware::ProcessorFeatures System_Hardware::s_extendedFeatures;

// OFFSET: 0x007AD1B0/0x1404A2EB0
bool System_Hardware::ReadCPUID(CPUIDInfo* info) {
    memset(info, 0, sizeof(CPUIDInfo));

    int32_t regs[4];
    __cpuid(regs, 0);

    if (!regs[0]) {
        return false;
    }

    info->m_maxLeaf = regs[0];

    int32_t vendor[4];
    vendor[0] = regs[1];
    vendor[1] = regs[3];
    vendor[2] = regs[2];
    vendor[3] = 0;
    memset(info->m_vendor, 0, sizeof(info->m_vendor));
    strncpy(info->m_vendor, reinterpret_cast<const char*>(vendor), 12);

    __cpuid(regs, 0x80000000);

    if (static_cast<uint32_t>(regs[0]) == 0x80000000) {
        return false;
    }

    info->m_maxExtLeaf = regs[0];

    if (info->m_maxLeaf >= 4) {
        __cpuid(regs, 4);
        info->m_leaf4Eax = regs[0];
    }

    __cpuid(regs, 1);
    info->m_leaf1Edx = regs[3];
    info->m_leaf1Ebx = regs[1];

    uint32_t maxExtLeaf = info->m_maxExtLeaf;

    if (maxExtLeaf >= 0x80000008) {
        __cpuid(regs, 0x80000008);
        info->m_ext8Ecx = regs[2];
    }

    if (maxExtLeaf >= 0x80000004) {
        __cpuid(regs, 0x80000004);
        info->m_ext4Eax = regs[0];
    }

    if (maxExtLeaf >= 0x80000001) {
        __cpuid(regs, 0x80000001);
        info->m_ext1Edx = regs[3];
        info->m_ext1Ecx = regs[2];
    }

    return true;
}

// OFFSET: 0x007AD350/0x1404A3000
uint32_t System_Hardware::CountSetBits(uint32_t mask) {
    uint32_t count = 0;

    for (uint32_t bit = 0x80000000; bit; bit >>= 1) {
        if (mask & bit) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x007AD3D0/0x1404A3090
bool System_Hardware::ReadLogicalProcessorInformation() {
    typedef BOOL (WINAPI *GETLOGICALPROCESSORINFORMATION)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

    GETLOGICALPROCESSORINFORMATION getInformation = reinterpret_cast<GETLOGICALPROCESSORINFORMATION>(GetProcAddress(GetModuleHandleW(L"kernel32"), "GetLogicalProcessorInformation"));
    uint32_t cores = 0;

    if (!getInformation) {
        return false;
    }

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION information[64];
    DWORD length = sizeof(information);

    if (!getInformation(information, &length)) {
        return false;
    }

    uint32_t processors = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* end = &information[length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)];

    for (SYSTEM_LOGICAL_PROCESSOR_INFORMATION* entry = information; entry < end; entry++) {
        if (entry->Relationship == RelationProcessorCore) {
            cores++;
            processors += CountSetBits(static_cast<uint32_t>(entry->ProcessorMask));
        }
    }

    s_processorFeatures.m_coreCount = cores;
    s_processorFeatures.m_processorCount = processors;

    return true;
}

// OFFSET: 0x007B5430/0x1404AB2C0
uint32_t Blizzard::Hardware::GetProcessorFeatures() {
    SYSTEM_INFO systemInfo;
    memset(&systemInfo, 0, sizeof(systemInfo));
    uint32_t features = 0;

    GetSystemInfo(&systemInfo);
    System_Hardware::s_processorFeatures.m_processorCount = systemInfo.dwNumberOfProcessors;

    if (!systemInfo.dwNumberOfProcessors) {
        System_Hardware::s_processorFeatures.m_processorCount = 1;
    }

    System_Hardware::s_processorFeatures.m_coreCount = 1;

    System_Hardware::CPUIDInfo info;

    if (System_Hardware::ReadCPUID(&info)) {
        if (info.m_leaf1Edx & 0x10) {
            features = 0x1;
        }

        if (info.m_leaf1Edx & 0x800000) {
            features |= 0x2;
        }

        if (info.m_leaf1Edx & 0x2000000) {
            features |= 0x4;
        }

        if (info.m_ext1Edx & 0x80000000) {
            features |= 0x8;
        }

        const char* vendors = "GenuineIntel" "AuthenticAMD" "CyrixInstead" "CentaurHalls";

        if (!strncmp(info.m_vendor, vendors, 12)) {
            System_Hardware::s_processorFeatures.m_vendor = 1;

            if (info.m_leaf1Edx & 0x4000000) {
                features |= 0x10;
            }

            if (info.m_maxLeaf >= 4 && info.m_leaf4Eax >> 26) {
                System_Hardware::s_processorFeatures.m_coreCount = (info.m_leaf4Eax >> 26) + 1;
                System_Hardware::s_processorFeatures.m_features = features | 0x40;
                return features | 0x40;
            }
        } else if (!strncmp(info.m_vendor, vendors + 12, 12)) {
            System_Hardware::s_processorFeatures.m_vendor = 2;

            if (info.m_leaf1Edx & 0x4000000) {
                features |= 0x10;
            }

            uint8_t cores = static_cast<uint8_t>(info.m_ext8Ecx);

            if (cores) {
                System_Hardware::s_processorFeatures.m_features = features | 0x40;
                System_Hardware::s_processorFeatures.m_coreCount = cores + 1;
                return features | 0x40;
            }
        } else if (!strncmp(info.m_vendor, vendors + 24, 12)) {
        } else if (!strncmp(info.m_vendor, vendors + 36, 12)) {
        }
    }

    System_Hardware::s_processorFeatures.m_features = features;

    return features;
}

// OFFSET: 0x007C0710
bool Blizzard::Hardware::ExtendedProcessorFeatures(ProcessorFeatures& features) {
    System_Hardware::s_extendedFeatures.m_features = GetProcessorFeatures();
    System_Hardware::s_extendedFeatures.m_coreCount = GetCoreCount();
    System_Hardware::s_extendedFeatures.m_vendor = GetVendor();
    System_Hardware::s_extendedFeatures.m_processorCount = System_Thread::GetProcessorCount();
    features = System_Hardware::s_extendedFeatures;

    return true;
}

// OFFSET: 0x007FE960
void System_Hardware::GetMACAddress(uint8_t* address) {
    typedef RPC_STATUS (RPC_ENTRY *UUIDCREATE)(UUID*);

    HMODULE library = LoadLibraryA("rpcrt4.dll");

    if (!library) {
        memset(address, 0, 6);
        return;
    }

    UUID uuid;
    UUIDCREATE create = reinterpret_cast<UUIDCREATE>(GetProcAddress(library, "UuidCreateSequential"));

    if (!create || create(&uuid)) {
        create = reinterpret_cast<UUIDCREATE>(GetProcAddress(library, "UuidCreate"));

        if (!create || create(&uuid)) {
            memset(&uuid, 0, sizeof(uuid));
        }
    }

    memcpy(address, &uuid.Data4[2], 6);
}

// INLINED
uint64_t System_Hardware::GetDiskCapacity() {
    ULARGE_INTEGER total;

    if (GetDiskFreeSpaceExW(NULL, NULL, &total, NULL)) {
        return total.QuadPart;
    }

    return 0;
}

// OFFSET: 0x007FE9E0
void System_Hardware::CalcFingerprint(uint8_t* fingerprint) {
    char buffer[296];
    memset(buffer, 0, sizeof(buffer));

    DWORD size = 256;

    if (!GetComputerNameA(buffer, &size)) {
        strcpy(buffer, "<unknown>");
    }

    GetMACAddress(reinterpret_cast<uint8_t*>(&buffer[256]));
    Blizzard::Hardware::ExtendedProcessorFeatures(*reinterpret_cast<Blizzard::Hardware::ProcessorFeatures*>(&buffer[264]));
    *reinterpret_cast<uint64_t*>(&buffer[280]) = Blizzard::Memory::GetPhysicalMemory();
    *reinterpret_cast<uint64_t*>(&buffer[288]) = GetDiskCapacity();

    SHA1::Digest digest;
    SHA1::Hash(digest, buffer, sizeof(buffer));
    memcpy(fingerprint, digest.m_data, 16);
}

// PROVEN: b30a7375 IDENTICAL/UNPROVEN
// OFFSET: 0x007FEAC0/0x140511E40
bool Blizzard::Hardware::GetFingerprint(Fingerprint* fingerprint) {
    System_Hardware::CalcFingerprint(fingerprint->m_data);
    return true;
}

// OFFSET: 0x007AD5C0/0x1404A3220
uint64_t Blizzard::Memory::GetPhysicalMemory() {
    typedef BOOL (WINAPI *GLOBALMEMORYSTATUSEX)(LPMEMORYSTATUSEX);

    MEMORYSTATUS status;
    memset(&status.dwMemoryLoad, 0, sizeof(status) - sizeof(status.dwLength));
    status.dwLength = sizeof(status);
    GlobalMemoryStatus(&status);

    if (status.dwTotalPhys < 0x80000000) {
        return status.dwTotalPhys;
    }

    GLOBALMEMORYSTATUSEX getStatus = reinterpret_cast<GLOBALMEMORYSTATUSEX>(GetProcAddress(GetModuleHandleA("kernel32.dll"), "GlobalMemoryStatusEx"));

    MEMORYSTATUSEX statusEx;
    statusEx.dwLength = sizeof(statusEx);
    memset(&statusEx.dwMemoryLoad, 0, sizeof(statusEx) - sizeof(statusEx.dwLength));

    if (getStatus && getStatus(&statusEx)) {
        return statusEx.ullTotalPhys;
    }

    if (status.dwTotalPhys == 0x80000000) {
        return 0x80000000;
    }

    return 0;
}

#endif
