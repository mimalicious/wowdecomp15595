#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// OFFSET: 0x007C0770/0x1404BA1F0
void Blizzard::Lock::EventCreate(Event& event) {
    event = CreateEventW(NULL, FALSE, FALSE, NULL);
}

// OFFSET: 0x007AD490/0x1404A3190
void Blizzard::Lock::EventDestroy(Event& event) {
    CloseHandle(event);
}

// OFFSET: 0x007AD4B0
void Blizzard::Lock::EventSet(Event& event) {
    SetEvent(event);
}

// OFFSET: 0x007AD4D0/0x1404A31B0
void Blizzard::Lock::EventWaitForever(Event& event) {
    WaitForSingleObject(event, INFINITE);
}

// OFFSET: 0x007AD4F0/0x1404A31C0
Blizzard::Lock::Event* Blizzard::Lock::EventWait(uint32_t timeout, Event& event) {
    return WaitForSingleObject(event, timeout) == WAIT_OBJECT_0 ? &event : NULL;
}

#endif
