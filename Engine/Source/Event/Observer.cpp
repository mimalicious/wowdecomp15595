#include <cstdint>
#include <typeinfo>
#include "Engine/Source/Base/RefCount.h"
#include "Engine/Source/Event/Observer.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00D957AC
static TRefCnt* s_inputObserver;

// STATIC: %0x00D957B4
static LockedDataAllocator s_eventRegAllocator(sizeof(EventReg), 1024);

// STATIC: %0x00D957F0
static LockedDataAllocator s_callbackRegAllocator(sizeof(EventReg::EVENTCALLBACKREG), 256);

// STATIC: %0x00D9582C
static LockedDataAllocator s_dispatchRegAllocator(sizeof(EventReg::EVENTDISPATCHREG), 1024);

void InputObserverInitialize() {
}

// OFFSET: %0x008086C0
void InputObserverDestroy() {
    if (s_inputObserver && --s_inputObserver->m_refcnt == 0) {
        s_inputObserver->DeleteSelf();
    }

    s_inputObserver = NULL;
}

void ObserverInitialize() {
}

// INLINED
void ObserverDestroy() {
    s_eventRegAllocator.m_allocator.Clear(typeid(EventReg).raw_name(), SLOG_OBJECT, 1);
    s_callbackRegAllocator.m_allocator.Clear(typeid(EventReg::EVENTCALLBACKREG).raw_name(), SLOG_OBJECT, 1);
    s_dispatchRegAllocator.m_allocator.Clear(typeid(EventReg::EVENTDISPATCHREG).raw_name(), SLOG_OBJECT, 1);
}
