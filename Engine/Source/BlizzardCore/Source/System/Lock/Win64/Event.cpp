#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// PROVEN: a3632f70 IDENTICAL/IDENTICAL
// OFFSET: 0x007C0770/0x1404BA1F0
void Blizzard::Lock::EventCreate(Event& event) {
    event = CreateEventW(NULL, FALSE, FALSE, NULL);
}

// PROVEN: 3756e437 IDENTICAL/NONE
// OFFSET: 0x007AD490/0x1404A3190
void Blizzard::Lock::EventDestroy(Event& event) {
    CloseHandle(event);
}

// PROVEN: c7ce5a2d IDENTICAL/NONE
// OFFSET: 0x007AD4B0
void Blizzard::Lock::EventSet(Event& event) {
    SetEvent(event);
}

// PROVEN: 3df34236 IDENTICAL/IDENTICAL
// OFFSET: 0x007AD4D0/0x1404A31B0
void Blizzard::Lock::EventWaitForever(Event& event) {
    WaitForSingleObject(event, INFINITE);
}

// PROVEN: fc9d1b3a IDENTICAL/IDENTICAL
// OFFSET: 0x007AD4F0/0x1404A31C0
Blizzard::Lock::Event* Blizzard::Lock::EventWait(uint32_t timeout, Event& event) {
    return WaitForSingleObject(event, timeout) == WAIT_OBJECT_0 ? &event : NULL;
}

// INLINED
Blizzard::Lock::EventObject::EventObject() {
    this->m_event = CreateEventW(NULL, FALSE, FALSE, NULL);
}

// OFFSET: #0x0042B390
Blizzard::Lock::EventObject::~EventObject() {
    EventDestroy(this->m_event);
}

#endif
