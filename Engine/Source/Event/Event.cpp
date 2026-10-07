#include <cstdint>
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/EvtContext.h"
#include "Engine/Source/Event/EvtInput.h"
#include "Engine/Source/Event/EvtQueue.h"
#include "Engine/Source/Event/EvtSched.h"
#include "Engine/Source/Event/EvtTimer.h"
#include "Engine/Source/Event/Observer.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: bcb8d168 IDENTICAL/UNPROVEN
// OFFSET: 0x008A0000/0x1405E11B0
void EventInitialize(uint32_t threadCount, int32_t netServer) {
    ObserverInitialize();
    InputObserverInitialize();
    IEvtQueueInitialize();
    IEvtInputInitialize();

    if (threadCount < 1) {
        threadCount = 1;
    }

    IEvtSchedulerInitialize(threadCount, netServer);
    OsInputSetEventPollProc(EventForceIdleProcessing);
}

// OFFSET: 0x008A0040
void EventDestroy() {
    OsInputSetEventPollProc(NULL);
    IEvtSchedulerDestroy();
    IEvtInputDestroy();
    IEvtQueueDestroy();
    InputObserverDestroy();
    ObserverDestroy();
}

// OFFSET: %0x00804880
void EventDoMessageLoop() {
    IEvtSchedulerProcess();
}

// PROVEN: 20ab3ec1 IDENTICAL/IDENTICAL
// OFFSET: 0x008A0080/0x1405E1240
void EventInitiateShutdown() {
    IEvtSchedulerShutdown();
}

// PROVEN: 047bcbf3 IDENTICAL/NONE
// OFFSET: 0x008A0090/0x1405E1250
HEVENTCONTEXT EventCreateContextEx(int32_t interactive, EVENTHANDLERFUNC initializeHandler, EVENTHANDLERFUNC destroyHandler, uint32_t idleTime, uint32_t debugFlags) {
    return IEvtSchedulerCreateContext(interactive, initializeHandler, destroyHandler, idleTime, debugFlags);
}

// PROVEN: 5c26ca08 IDENTICAL/IDENTICAL
// OFFSET: 0x008A00A0/0x1405E1270
void EventSetConfirmCloseCallback(int32_t (*callback)(void*), void* param) {
    IEvtInputSetConfirmCloseCallback(callback, param);
}

// PROVEN: c8016dbf IDENTICAL/IDENTICAL
// OFFSET: 0x008A00E0/0x1405E12C0
HEVENTCONTEXT EventGetCurrentContext() {
    return static_cast<HEVENTCONTEXT>(PropGet(PROP_EVENTCONTEXT));
}

// OFFSET: 0x008A01E0
int32_t EventIsButtonDown(MOUSEBUTTON button) {
    int32_t result = 0;
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        result = IEvtQueueCheckSyncMouseState(context, button);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }

    return result;
}

// OFFSET: 0x008A0240/0x1405E13C0
int32_t EventIsKeyDown(KEY key) {
    int32_t result = 0;
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        result = IEvtQueueCheckSyncKeyState(context, key);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }

    return result;
}

// OFFSET: 0x008A02A0/0x1405E14B0
void EventPostCloseEx(HEVENTCONTEXT contextHandle) {
    if (!contextHandle) {
        contextHandle = static_cast<HEVENTCONTEXT>(PropGet(PROP_EVENTCONTEXT));
    }

    uint32_t findMask;
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(contextHandle)), 0, &findMask, 0, 1);

    if (context) {
        CloseContext(context);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }
}

// OFFSET: 0x008A0320/0x1405E15A0
int32_t EventQueuePost(HEVENTCONTEXT contextHandle, EVENTID id, const void* data, uint32_t bytes) {
    if (!contextHandle) {
        contextHandle = static_cast<HEVENTCONTEXT>(PropGet(PROP_EVENTCONTEXT));
    }

    uint32_t findMask;
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(contextHandle)), 0, &findMask, 0, 1);

    if (!context) {
        return 0;
    }

    context->m_critsect.Enter();
    int32_t destroyed = context->m_schedState == SCHEDSTATE_DESTROYED;
    context->m_critsect.Leave();

    int32_t result;

    if (destroyed) {
        result = 0;
    } else {
        IEvtQueuePost(context, id, data, bytes);
        result = 1;
    }

    if (findMask != -1) {
        TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
    }

    return result;
}

// OFFSET: 0x008A03D0/0x1405E16E0
void EventRegisterEx(EVENTID id, EVENTHANDLERFUNC handler, void* param, float priority) {
    if (id < 0 || id >= EVENTIDS || !handler) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        IEvtQueueRegister(context, id, handler, param, priority);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }
}

// OFFSET: 0x008A0460/0x1405E1810
void EventUnregisterEx(EVENTID id, EVENTHANDLERFUNC handler, void* param, uint32_t flags) {
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        IEvtQueueUnregister(context, id, handler, param, flags);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }
}

// OFFSET: 0x008A04D0/0x1405E1910
int32_t EventInputProcess(HEVENTCONTEXT contextHandle) {
    int32_t result = 0;

    if (!contextHandle) {
        contextHandle = static_cast<HEVENTCONTEXT>(PropGet(PROP_EVENTCONTEXT));
    }

    uint32_t findMask;
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(contextHandle)), 0, &findMask, 0, 1);

    if (context) {
        int32_t shutdown;
        result = IEvtInputProcess(context, &shutdown);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }

    return result;
}

// OFFSET: 0x008A0540/0x1405E1A00
uint32_t EventSetTimer(uint32_t delay, EVENTHANDLERFUNC handler, void* param) {
    uint32_t result = 0;
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        result = IEvtTimerSet(context, delay, handler, param, NULL, 0, NULL);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }

    return result;
}

// OFFSET: 0x008A05B0
uint32_t EventSetTimer(uint32_t delay, EVENTHANDLERGUIDFUNC handler, uint64_t guid, void* param) {
    uint32_t result = 0;
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        result = IEvtTimerSet(context, delay, NULL, NULL, handler, guid, param);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }

    return result;
}

// OFFSET: 0x008A0620/0x1405E1C50
void EventKillTimer(uint32_t id, EVENTHANDLERFUNC handler, const char* name) {
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        IEvtTimerKill(context, id, handler, name);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }
}

// OFFSET: 0x008A0690/0x1405E1D30
void EventSetMouseMode(MOUSEMODE mode, uint32_t holdButton) {
    if (mode >= MOUSE_MODES) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        IEvtInputSetMouseMode(context, mode, holdButton);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }
}

// OFFSET: 0x008A0700/0x1405E1E20
void EventInputGetMousePosition(float* x, float* y) {
    uint32_t findMask;
    uint32_t contextId = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(PropGet(PROP_EVENTCONTEXT)));
    EvtContext* context = TSingletonInstanceId<EvtContext, 8>::s_idTable.Ptr(contextId, 0, &findMask, 0, 1);

    if (context) {
        IEvtInputGetMousePosition(x, y);

        if (findMask != -1) {
            TSingletonInstanceId<EvtContext, 8>::s_idTable.Unlock(findMask & 7, findMask >= 8);
        }
    }
}

// OFFSET: 0x008A0760
void EventPostClose() {
    EventPostCloseEx(NULL);
}

// PROVEN: 7591e3ba IDENTICAL/IDENTICAL
// OFFSET: 0x008A07D0/0x1405E1F10
void EventRegister(EVENTID id, EVENTHANDLERFUNC handler) {
    EventRegisterEx(id, handler, NULL, 0.0f);
}

// PROVEN: e018d798 IDENTICAL/IDENTICAL
// OFFSET: 0x008A07F0/0x1405E1F20
void EventUnregister(EVENTID id, EVENTHANDLERFUNC handler) {
    EventUnregisterEx(id, handler, NULL, 0xFFFFFFFF);
}
