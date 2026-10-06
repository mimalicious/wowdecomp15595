#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/TExtraInstanceRecycler.h"
#include "Engine/Source/Event/EvtContext.h"
#include "Engine/Source/Event/EvtQueue.h"
#include "Engine/Source/Event/EvtSched.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00D95790
static TExtraInstanceRecycler<EvtMessage> s_messageRecycler(64, 64, 256);

void IEvtQueueInitialize() {
}

// OFFSET: 0x008A3690
void IEvtQueueDestroy() {
    s_messageRecycler.Clear();
}

// PROVEN: a8dcebd2 IDENTICAL/DIFFERENT
// OFFSET: 0x008A36A0/0x1405E5B80
int32_t IEvtQueueCheckSyncMouseState(EvtContext* context, MOUSEBUTTON button) {
    if (!context) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return (context->m_queueSyncButtonState & button) != 0;
}

// OFFSET: 0x008A37E0/0x1405E5C60
void IEvtQueueRegister(EvtContext* context, EVENTID id, EVENTHANDLERFUNC handler, void* param, float priority) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    TSExplicitList<EvtHandler, offsetof(EvtHandler, m_link)>& list = context->m_queueHandlerList[id];

    EvtHandler* evtHandler = list.NewNode(0, 0, 0);
    evtHandler->m_priority = priority;
    evtHandler->m_param = param;
    evtHandler->m_func = handler;
    evtHandler->m_marker = 0;

    EvtHandler* existing = list.Head();

    while (existing && (priority < existing->m_priority || existing->m_marker)) {
        existing = list.Next(existing);
    }

    list.LinkNode(evtHandler, 2, existing);
}

// OFFSET: 0x008A39A0/0x1405E5EE0
void IEvtQueuePost(EvtContext* context, EVENTID id, const void* data, uint32_t bytes) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    EvtMessage* message = s_messageRecycler.New((bytes > 4 ? bytes : 4) - 4);
    message->m_id = id;

    if (data) {
        memcpy(message->m_data, data, bytes);
    }

    context->m_critsect.Enter();
    context->m_queueMessageList.LinkToTail(message);
    context->m_critsect.Leave();
}

// OFFSET: 0x008A3BB0
static void UpdateSyncKeyState(EvtContext* context, KEY key, EVENTID& id) {
    int32_t found = 0;

    context->m_critsect.Enter();

    TSExplicitList<EvtKeyDown, offsetof(EvtKeyDown, m_link)>& list = context->m_queueSyncKeyDownList;

    for (EvtKeyDown* keyDown = list.Head(), * next; !list.IsEnd(keyDown); keyDown = next) {
        next = list.RawNext(keyDown);

        if (keyDown->m_key == key) {
            found = 1;
            list.DeleteNode(keyDown);
        }
    }

    if (id == EVENT_ID_KEYDOWN) {
        if (found) {
            id = EVENT_ID_KEYDOWN_REPEATING;
        }

        EvtKeyDown* newKeyDown = list.NewNode(2, 0, 0);
        newKeyDown->m_key = key;
    }

    context->m_critsect.Leave();
}

// OFFSET: 0x008A3CB0
static void UpdateSyncState(EvtContext* context, EVENTID& id, const void* data) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    switch (id) {
        case EVENT_ID_FOCUS:
            context->m_queueSyncButtonState = 0;
            context->m_critsect.Enter();
            context->m_queueSyncKeyDownList.Clear();
            context->m_critsect.Leave();
            break;

        case EVENT_ID_KEYDOWN:
        case EVENT_ID_KEYUP:
            UpdateSyncKeyState(context, static_cast<const EVENT_DATA_KEY*>(data)->m_key, id);
            break;

        case EVENT_ID_MOUSEDOWN:
        case EVENT_ID_MOUSEUP: {
            uint32_t button = static_cast<const EVENT_DATA_MOUSE*>(data)->m_button;

            if (id == EVENT_ID_MOUSEDOWN) {
                context->m_queueSyncButtonState |= button;
            } else {
                context->m_queueSyncButtonState &= ~button;
            }

            break;
        }

        default:
            break;
    }
}

// OFFSET: 0x008A3760
int32_t IEvtQueueCheckSyncKeyState(EvtContext* context, KEY key) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    int32_t found = 0;

    context->m_critsect.Enter();

    for (EvtKeyDown* keyDown = context->m_queueSyncKeyDownList.Head(), * next; !context->m_queueSyncKeyDownList.IsEnd(keyDown); keyDown = next) {
        next = context->m_queueSyncKeyDownList.RawNext(keyDown);

        if (keyDown->m_key == key) {
            found = 1;
            break;
        }
    }

    context->m_critsect.Leave();

    return found;
}

// OFFSET: 0x008A3D60/0x1405E6300
void IEvtQueueDispatch(EvtContext* context, EVENTID id, const void* data) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    UpdateSyncState(context, id, data);

    if (SErrIsDisplayingError()) {
        return;
    }

    IEvtSchedulerIsContextInteractive(context);

    TSExplicitList<EvtHandler, offsetof(EvtHandler, m_link)>& list = context->m_queueHandlerList[id];

    EvtHandler marker;
    marker.m_marker = 1;
    list.LinkToHead(&marker);

    EvtHandler* handler;

    while ((handler = list.Next(&marker)) != NULL) {
        list.LinkNode(&marker, 1, handler);

        if (handler->m_marker) {
            continue;
        }

        if (!handler->m_func(data, handler->m_param)) {
            break;
        }
    }

    list.UnlinkNode(&marker);
}

// OFFSET: 0x008A3E60/0x1405E6500
void IEvtQueueDispatchAll(EvtContext* context) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    TSExplicitList<EvtMessage, offsetof(EvtMessage, m_link)> messages;

    context->m_critsect.Enter();
    messages.Combine(&context->m_queueMessageList, 2, NULL);
    context->m_critsect.Leave();

    EvtMessage* message;

    while ((message = messages.Head()) != NULL) {
        IEvtQueueDispatch(context, message->m_id, message->m_data);
        s_messageRecycler.Delete(message);
    }
}

// OFFSET: 0x008A3FC0/0x1405E67D0
void IEvtQueueUnregister(EvtContext* context, EVENTID id, EVENTHANDLERFUNC handler, void* param, uint32_t flags) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    for (int32_t i = 0; i < EVENTIDS; i++) {
        if (!(flags & 1) || i == id) {
            TSExplicitList<EvtHandler, offsetof(EvtHandler, m_link)>& list = context->m_queueHandlerList[i];

            for (EvtHandler* evtHandler = list.Head(), * next; !list.IsEnd(evtHandler); evtHandler = next) {
                next = list.RawNext(evtHandler);

                if ((evtHandler->m_func == handler || !(flags & 2)) && (evtHandler->m_param == param || !(flags & 4)) && !evtHandler->m_marker) {
                    list.DeleteNode(evtHandler);
                }
            }
        }
    }
}
