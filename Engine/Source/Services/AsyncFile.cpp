#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00BB9C20
static const char* s_asyncQueueNames[3] = { "Disk Queue", "Net Geometry Queue", "Net Texture Queue" };

// STATIC: %0x00BB9C2C
static uint32_t s_handlerTimeout = 100;

// STATIC: %0x00BB9C30
static TSList<CAsyncQueue, TSGetLink<CAsyncQueue> > s_asyncQueueList;

// STATIC: %0x00BB9C3C
static TSList<CAsyncThread, TSGetLink<CAsyncThread> > s_asyncThreadList;

namespace AsyncFileRead {
    // STATIC: %0x00BB9C48
    TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncFileReadFreeList;
}

// STATIC: %0x00BB9C54
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncFileReadPostList;

// STATIC: %0x00CD2860
static int32_t s_queueLockCount;

// STATIC: %0x00CD2864
static void (*s_ingameStartCallback)();

// STATIC: %0x00CD2868
static void (*s_ingameProgressCallback)(float, int32_t);

// STATIC: %0x00CD286C
static int32_t s_progressCount;

// STATIC: %0x00CD2870
static void* s_asyncProgressCallbackParams;

// STATIC: %0x00CD2874
static void (*s_asyncProgressCallback)(float, void*);

// STATIC: %0x00CD2878
static CAsyncObject* s_asyncWaitObject;

// STATIC: %0x00CD287C
static HPROPCONTEXT s_propContext;

// STATIC: %0x00CD2880
static CAsyncQueue* s_asyncQueues[3];

// STATIC: %0x00CD288C
static uint32_t s_threadSleep;

// STATIC: %0x00CD2890
static int32_t s_streamingError;

// STATIC: %0x00CD2894
static bool s_shuttingDown;

namespace AsyncFileRead {
    // STATIC: %0x00CD2898
    SEvent s_shutdownEvent(1, 0);
}

// STATIC: %0x00CD289C
static TSGrowableArray<void (*)()> s_asyncPollHandlers;

// STATIC: %0x00CD28AC
static TSGrowableArray<int32_t (*)()> s_asyncStatusHandlers;

// STATIC: %0x00CD28BC
SCritSect g_queueLock;

// STATIC: %0x00CD28E4
static TSGrowableArray<CStreamingObj> s_streamingObjects;

// PROVEN: e75ee842 IDENTICAL/IDENTICAL
// OFFSET: 0x0053EA10/0x1401994D0
void AsyncFileReadProgressCallback(void (*callback)(float, void*), void* param) {
    s_asyncProgressCallback = callback;
    s_asyncProgressCallbackParams = param;
}

// PROVEN: 416eeef9 IDENTICAL/IDENTICAL
// OFFSET: 0x0053EA30/0x1401994E0
void AsyncFileReadIngameProgressCallback(void (*startCallback)(), void (*progressCallback)(float, int32_t)) {
    s_ingameStartCallback = startCallback;
    s_ingameProgressCallback = progressCallback;
}

// OFFSET: %0x0054F3C0
CAsyncObject* AsyncFileReadGetCurrentObject(int32_t queue) {
    g_queueLock.Enter();

    if (s_asyncQueues[queue]) {
        CAsyncObject* object = s_asyncQueues[queue]->m_currentObject;
        g_queueLock.Leave();
        return object;
    }

    g_queueLock.Leave();
    return NULL;
}

// PROVEN: 84343944 IDENTICAL/IDENTICAL
// OFFSET: 0x0053EAD0/0x140199570
void AsyncFileReadEnterQueueLock() {
    if (!s_queueLockCount) {
        g_queueLock.Enter();
    }

    s_queueLockCount++;
}

// PROVEN: fb0d39ff IDENTICAL/IDENTICAL
// OFFSET: 0x0053EAF0/0x1401995B0
void AsyncFileReadLeaveQueueLock() {
    if (!--s_queueLockCount) {
        g_queueLock.Leave();
    }
}

// OFFSET: %0x0054F440
static int CompareStreamingObjects(const void* a, const void* b) {
    const CStreamingObj* obj1 = static_cast<const CStreamingObj*>(a);
    const CStreamingObj* obj2 = static_cast<const CStreamingObj*>(b);

    if (obj1->m_priority < obj2->m_priority) {
        return -1;
    }

    if (obj1->m_priority > obj2->m_priority) {
        return 1;
    }

    if (obj1->m_subPriority < obj2->m_subPriority) {
        return -1;
    }

    if (obj1->m_subPriority > obj2->m_subPriority) {
        return 1;
    }

    if (obj2->m_distance > obj1->m_distance) {
        return -1;
    }

    return obj1->m_distance > obj2->m_distance;
}

// OFFSET: %0x0054F490
int32_t AsyncFileReadHasStreamingErrorOccurred() {
    return s_streamingError;
}

// OFFSET: %0x0054F4A0
void AsyncFileReadResetStreamingError() {
    s_streamingError = 0;
}

// OFFSET: %0x0054F6A0
void AsyncFileReadUnloadPostList() {
    CAsyncObject* object;

    while ((object = s_asyncFileReadPostList.Head()) != NULL) {
        object->m_link.Unlink();

        if (object->m_userPostUnloadCallback) {
            reinterpret_cast<void (*)(void*, CAsyncObject*)>(object->m_userPostUnloadCallback)(object->m_userArg, object);
        }
    }
}

// OFFSET: %0x0054F710
int32_t AsyncFileReadGetStreamingQueue() {
    int32_t queue = 2;
    g_queueLock.Enter();

    if (s_asyncQueues[1] && s_asyncQueues[1]->m_readList.Head()) {
        queue = 0;
    } else if (s_asyncQueues[2] && s_asyncQueues[2]->m_readList.Head()) {
        queue = 1;
    }

    SFile::PauseBackgroundStreaming(queue != 2);
    g_queueLock.Leave();

    return queue;
}

// OFFSET: %0x0054F770
int32_t AsyncFileReadIsStreaming() {
    g_queueLock.Enter();

    int32_t streaming = 0;

    if (s_asyncQueues[1] && (s_asyncQueues[1]->m_readList.Head() || s_asyncQueues[1]->m_currentObject)) {
        streaming = 1;
    }

    if (s_asyncQueues[2] && (s_asyncQueues[2]->m_readList.Head() || s_asyncQueues[2]->m_currentObject)) {
        streaming = 1;
    }

    g_queueLock.Leave();

    return streaming;
}

// OFFSET: %0x0054F7D0
int32_t AsyncFileReadPollHandler(const void* data, void* param) {
    uint32_t start = OsGetAsyncTimeMs();

    while (true) {
        g_queueLock.Enter();

        CAsyncObject* object = s_asyncFileReadPostList.Head();

        if (!object) {
            g_queueLock.Leave();
            break;
        }

        object->m_link.Unlink();

        if (object == s_asyncWaitObject) {
            s_asyncWaitObject = NULL;
        }

        int32_t succeeded = object->m_readSucceeded;
        object->m_isProcessed = 1;
        g_queueLock.Leave();

        if (succeeded) {
            if (object->m_userPostloadCallback) {
                reinterpret_cast<void (*)(void*, CAsyncObject*)>(object->m_userPostloadCallback)(object->m_userArg, object);
            }
        } else {
            if (object->m_userPostUnloadCallback) {
                reinterpret_cast<void (*)(void*, CAsyncObject*)>(object->m_userPostUnloadCallback)(object->m_userArg, object);
            }

            s_streamingError = 1;
        }

        s_progressCount--;

        if (OsGetAsyncTimeMs() - start > s_handlerTimeout) {
            break;
        }
    }

    for (uint32_t i = 0; i < s_asyncPollHandlers.Count(); i++) {
        s_asyncPollHandlers[i]();
    }

    return 1;
}

// OFFSET: %0x0054FB30
void AsyncFileReadAddPollHandler(void (*handler)()) {
    for (uint32_t i = 0; i < s_asyncPollHandlers.Count(); i++) {
        if (s_asyncPollHandlers[i] == handler) {
            return;
        }
    }

    s_asyncPollHandlers.New(handler);
}

// OFFSET: %0x0054FBF0
void AsyncFileReadAddStatusHandler(int32_t (*handler)()) {
    for (uint32_t i = 0; i < s_asyncStatusHandlers.Count(); i++) {
        if (s_asyncStatusHandlers[i] == handler) {
            return;
        }
    }

    s_asyncStatusHandlers.New(handler);
}

// OFFSET: %0x0054FCB0
void AsyncFileReadDestroyObject(CAsyncObject* object) {
    g_queueLock.Enter();

    while (object->m_isCurrent) {
        DebugLog("AsyncFileReadDestroyObject()");
        g_queueLock.Leave();

        while (object->m_isCurrent) {
            OsSleep(1);
        }

        g_queueLock.Enter();
    }

    object->m_link.Unlink();

    if (object->m_file) {
        SFileCloseFile(object->m_file);
        object->m_file = NULL;
    }

    object->m_queue = NULL;
    AsyncFileRead::s_asyncFileReadFreeList.LinkToHead(object);
    g_queueLock.Leave();
}

// OFFSET: %0x0054FD80
int32_t NetQueuePromoteLocalFiles(const void* data, void* param) {
    // STATIC: %0x00CD28F4
    static uint32_t s_lastPromoteTime;

    uint32_t time = OsGetAsyncTimeMs();

    if (time - s_lastPromoteTime >= 250) {
        s_lastPromoteTime = time;
        g_queueLock.Enter();

        for (int32_t i = 1; i < 3; i++) {
            CAsyncObject* object = s_asyncQueues[i]->m_readList.Head();

            while (!s_asyncQueues[i]->m_readList.IsEnd(object)) {
                CAsyncObject* next = s_asyncQueues[i]->m_readList.RawNext(object);

                if (!object->m_isCurrent && SFile::FileIsLocal(object->m_file, 0, 0) == 1) {
                    s_asyncQueues[0]->m_readList.LinkToTail(object);
                    object->m_queue = s_asyncQueues[0];
                }

                object = next;
            }
        }

        g_queueLock.Leave();
    }

    return 1;
}

// OFFSET: %0x0054FEA0
void AsyncFileReadWait(CAsyncObject* object) {
    // STATIC: %0x00CD28F8
    static int32_t s_waiting;

    if (s_waiting) {
        SErrDisplayAppFatal("AsyncFileReadWait(): s_waiting != FALSE");
    }

    s_waiting++;
    g_queueLock.Enter();

    if (object->m_isProcessed) {
        g_queueLock.Leave();
        return;
    }

    s_asyncWaitObject = object;

    if (!object->m_isCurrent && !object->m_isRead) {
        object->m_link.Unlink();
        object->m_queue->m_readList.LinkToHead(object);
    }

    g_queueLock.Leave();

    if (SFile::IsStreamingMode()) {
        SFile::LogFileAccess(object->m_file, 1, false);
    }

    if (s_ingameStartCallback) {
        s_ingameStartCallback();
    }

    while (true) {
        if (s_ingameProgressCallback) {
            int64_t local = 0;
            int64_t total = 0;
            SFile::FileGetIsLocalAmount(object->m_file, local, total);
            s_ingameProgressCallback(static_cast<float>(static_cast<double>(local) / static_cast<double>(total)), 0);
        }

        AsyncFileReadPollHandler(NULL, NULL);

        if (!s_asyncWaitObject) {
            break;
        }

        OsSleep(1);
    }

    s_waiting--;
}

// OFFSET: %0x0054FFA0
void AsyncFileReadAddStreamingObject(void (*callback)(void*), void* param, float distance, int16_t priority, int16_t subPriority) {
    CStreamingObj* object = s_streamingObjects.New();
    object->m_callback = callback;
    object->m_param = param;
    object->m_distance = distance;
    object->m_priority = priority;
    object->m_subPriority = subPriority;
}

// OFFSET: %0x0054FFE0
void AsyncFileReadClearStreamingObjectList() {
    s_streamingObjects.SetCount(0);
}

// OFFSET: %0x00550000
void AsyncFileReadUpdateStreamingObjectList() {
    if (!s_streamingObjects.Count()) {
        return;
    }

    qsort(s_streamingObjects.Ptr(), s_streamingObjects.Count(), sizeof(CStreamingObj), CompareStreamingObjects);
    AsyncFileReadEnterQueueLock();

    for (int32_t i = s_streamingObjects.Count() - 1; i >= 0; i--) {
        s_streamingObjects[i].m_callback(s_streamingObjects[i].m_param);
    }

    AsyncFileReadLeaveQueueLock();
    s_streamingObjects.SetCount(0);
}

// OFFSET: %0x00550090
uint32_t STORMAPI AsyncFileReadThread(void* param) {
    CAsyncThread* thread = static_cast<CAsyncThread*>(param);
    CAsyncQueue* queue = thread->m_queue;
    PropSelectContext(s_propContext);

    while (AsyncFileRead::s_shutdownEvent.Wait(0)) {
        uint32_t count = 0;

        while (true) {
            g_queueLock.Enter();

            CAsyncObject* object = queue->m_readList.Head();

            if (!object) {
                break;
            }

            if (s_shuttingDown) {
                AsyncFileReadDestroyObject(object);
                g_queueLock.Leave();
                continue;
            }

            object->m_isCurrent = 1;
            queue->m_currentObject = object;
            g_queueLock.Leave();

            int32_t status = 0;

            if (SFile::IsStreamingMode() && !SFile::MakeFileLocal(object->m_file, (object->m_priority > 127) + 1, true, 0x20000, &status)) {
                status = 2;
            }

            if (object->m_cancelled) {
                g_queueLock.Enter();
                queue->m_currentObject = NULL;
                object->m_isCurrent = 0;
                reinterpret_cast<void (*)(void*, CAsyncObject*)>(object->m_userPostUnloadCallback)(object->m_userArg, object);
            } else if (status == 1) {
                g_queueLock.Enter();
                queue->m_currentObject = NULL;
                object->m_isCurrent = 0;
            } else {
                int8_t succeeded = 0;

                if (!status) {
                    for (int32_t retries = 10; ; ) {
                        if (!object->m_buffer || SFile::Read(object->m_file, object->m_buffer, object->m_size, NULL)) {
                            succeeded = 1;
                            break;
                        }

                        if (!--retries) {
                            break;
                        }
                    }
                }

                g_queueLock.Enter();
                s_asyncFileReadPostList.LinkToTail(object);
                queue->m_currentObject = NULL;
                object->m_readSucceeded = succeeded;
                object->m_queue = NULL;
                object->m_isCurrent = 0;
                object->m_isRead = 1;
            }

            g_queueLock.Leave();

            if (s_threadSleep && ++count == s_threadSleep) {
                OsSleep(1);
                count = 0;
            }
        }

        g_queueLock.Leave();
        OsSleep(1);
    }

    return 0;
}

// OFFSET: %0x005502C0
void AsyncFileReadCreateThread(CAsyncQueue* queue, const char* name) {
    CAsyncThread* thread = s_asyncThreadList.NewNode(2, 0, 0);
    thread->m_queue = queue;
    queue->m_currentObject = NULL;
    SThread::Create(AsyncFileReadThread, thread, thread->m_thread, const_cast<char*>(name), 0);
}

// OFFSET: %0x00550380
CAsyncObject* AsyncFileReadAllocObject() {
    g_queueLock.Enter();

    CAsyncObject* object = AsyncFileRead::s_asyncFileReadFreeList.Head();

    if (!object) {
        object = AsyncFileRead::s_asyncFileReadFreeList.NewNode(1, 0, 0);
    }

    object->m_link.Unlink();
    g_queueLock.Leave();

    object->m_file = NULL;
    object->m_buffer = NULL;
    object->m_size = 0;
    object->m_userArg = NULL;
    object->m_userPostloadCallback = NULL;
    object->m_userPostUnloadCallback = NULL;
    object->m_queue = NULL;
    object->m_isProcessed = 0;
    object->m_isRead = 0;
    object->m_isCurrent = 0;
    object->m_isStreamed = 0;
    object->m_readSucceeded = 0;
    object->m_cancelled = 0;
    object->m_priority = 126;

    return object;
}

// OFFSET: %0x00550440
int32_t AsyncFileReadCancel(CAsyncObject* object, void (*callback)(void* param, CAsyncObject* object)) {
    g_queueLock.Enter();

    if (object->m_isCurrent) {
        if (!callback) {
            callback = reinterpret_cast<void (*)(void*, CAsyncObject*)>(object->m_userPostUnloadCallback);

            if (callback) {
                goto cancel;
            }

            callback = DefaultAsyncObjectCleanupCallback;
        }

        object->m_userPostUnloadCallback = reinterpret_cast<void*>(callback);
        object->m_userArg = NULL;

    cancel:
        object->m_userPostloadCallback = reinterpret_cast<void*>(callback);
        object->m_cancelled = 1;
        g_queueLock.Leave();

        return 0;
    }

    if (object->m_file) {
        SFileCloseFile(object->m_file);
        object->m_file = NULL;
    }

    object->m_queue = NULL;
    object->m_link.Unlink();
    AsyncFileRead::s_asyncFileReadFreeList.LinkToHead(object);
    g_queueLock.Leave();

    return 1;
}

// OFFSET: %0x005505E0
CAsyncQueue::~CAsyncQueue() {
}

// OFFSET: %0x00550660
void AsyncFileReadLinkObject(CAsyncObject* object, int32_t urgent) {
    if (!object->m_queue) {
        return;
    }

    object->m_link.Unlink();

    for (CAsyncObject* other = object->m_queue->m_readList.Head(); !object->m_queue->m_readList.IsEnd(other); other = object->m_queue->m_readList.RawNext(other)) {
        if (object->m_priority <= other->m_priority && (urgent || object->m_priority != other->m_priority)) {
            object->m_queue->m_readList.LinkNode(object, 2, other);
            return;
        }
    }

    object->m_queue->m_readList.LinkToTail(object);
}

// OFFSET: %0x00550760
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent) {
    CAsyncQueue* queue = s_asyncQueues[0];

    if (SFile::IsStreamingMode()) {
        int32_t local = SFile::FileIsLocal(object->m_file, 0, 0);

        if (!local || local == 2) {
            SFile::PauseBackgroundStreaming(true);
            object->m_isStreamed = 1;
            queue = object->m_priority > 127 ? s_asyncQueues[2] : s_asyncQueues[1];
        }
    }

    g_queueLock.Enter();
    object->m_queue = queue;

    if (object == s_asyncWaitObject) {
        object->m_priority = object->m_priority > 127 ? 128 : 0;
        queue->m_readList.LinkToHead(object);
    } else {
        AsyncFileReadLinkObject(object, urgent);
    }

    g_queueLock.Leave();

    if (SFile::IsStreamingMode()) {
        SFile::LogFileAccess(object->m_file, 0, false);
    }
}

// OFFSET: %0x00550810
void AsyncFileReadLinkToPriority(CAsyncObject* object, int32_t urgent) {
    if (!object->m_isRead && !object->m_isProcessed && object->m_queue) {
        AsyncFileReadLinkObject(object, urgent);
    }
}

// OFFSET: %0x00550840
CAsyncQueue* AsyncFileReadCreateQueue() {
    return s_asyncQueueList.NewNode(2, 0, 0);
}

// OFFSET: %0x005508F0
void AsyncFileReadInitialize(uint32_t threadSleep, uint32_t handlerTimeout) {
    s_threadSleep = threadSleep;
    s_handlerTimeout = handlerTimeout;

    if (threadSleep > 100) {
        s_threadSleep = 100;
    }

    if (handlerTimeout < 20) {
        s_handlerTimeout = 20;
    }

    EventRegisterEx(EVENT_ID_POLL, AsyncFileReadPollHandler, NULL, 0.0f);

    if (SFile::IsStreamingMode()) {
        EventRegisterEx(EVENT_ID_IDLE, NetQueuePromoteLocalFiles, NULL, -1.0f);
    }

    s_asyncWaitObject = NULL;
    s_asyncProgressCallback = NULL;
    s_asyncProgressCallbackParams = NULL;
    s_ingameProgressCallback = NULL;
    s_ingameStartCallback = NULL;
    s_propContext = PropGetSelectedContext();
    AsyncFileRead::s_shutdownEvent.Reset();

    int32_t queues = SFile::IsStreamingMode() ? 3 : 1;

    for (int32_t i = 0; i < queues; i++) {
        s_asyncQueues[i] = AsyncFileReadCreateQueue();
        AsyncFileReadCreateThread(s_asyncQueues[i], s_asyncQueueNames[i]);
    }

    s_queueLockCount = 0;
}

// OFFSET: %0x005509E0
void AsyncFileReadDestroy() {
    s_shuttingDown = true;

    for (CAsyncThread* thread = s_asyncThreadList.Head(); !s_asyncThreadList.IsEnd(thread); thread = s_asyncThreadList.RawNext(thread)) {
        AsyncFileRead::s_shutdownEvent.Set();
        thread->m_thread.Wait(0xFFFFFFFF);
    }

    AsyncFileReadUnloadPostList();
    AsyncFileRead::s_asyncFileReadFreeList.Clear();

    for (CAsyncQueue* queue = s_asyncQueueList.Head(); !s_asyncQueueList.IsEnd(queue); queue = s_asyncQueueList.RawNext(queue)) {
    }

    s_asyncQueueList.Clear();
    s_asyncQueues[0] = NULL;
    s_asyncQueues[1] = NULL;
    s_asyncQueues[2] = NULL;
    s_asyncThreadList.Clear();
    s_asyncPollHandlers.SetCount(0);
    s_asyncStatusHandlers.SetCount(0);
    s_asyncProgressCallback = NULL;
    s_asyncProgressCallbackParams = NULL;
    s_ingameProgressCallback = NULL;
    s_ingameStartCallback = NULL;

    if (SFile::IsStreamingMode()) {
        EventUnregisterEx(EVENT_ID_IDLE, NetQueuePromoteLocalFiles, NULL, 2);
    }

    EventUnregisterEx(EVENT_ID_POLL, AsyncFileReadPollHandler, NULL, 0xFFFFFFFF);
}

// OFFSET: %0x00550B00
int32_t AsyncFileReadIsReading() {
    g_queueLock.Enter();

    int32_t reading = 0;

    for (CAsyncQueue* queue = s_asyncQueueList.Head(); !s_asyncQueueList.IsEnd(queue); queue = s_asyncQueueList.RawNext(queue)) {
        if (queue->m_currentObject) {
            reading = 1;
        }

        if (queue->m_readList.Head()) {
            reading = 1;
        }
    }

    if (s_asyncFileReadPostList.Head()) {
        reading = 1;
    }

    g_queueLock.Leave();

    return reading;
}

// OFFSET: %0x00550B70
void AsyncFileReadWaitAll() {
    DebugLog("AsyncFileReadWaitAll() Enter\n");

    int32_t total = 0;

    for (uint32_t i = 0; i < s_asyncStatusHandlers.Count(); i++) {
        total += s_asyncStatusHandlers[i]();
    }

    s_progressCount = total;

    while (AsyncFileReadIsReading()) {
        AsyncFileReadPollHandler(NULL, NULL);

        if (s_asyncProgressCallback) {
            float progress = 1.0f;

            if (total) {
                progress = static_cast<float>(total - s_progressCount) / total;

                if (progress < 0.0f) {
                    progress = 0.0f;
                } else if (progress > 1.0f) {
                    progress = 1.0f;
                }
            }

            s_asyncProgressCallback(progress, s_asyncProgressCallbackParams);
        }

        OsSleep(1);
    }

    s_asyncProgressCallback = NULL;
    DebugLog("AsyncFileReadWaitAll() Leave\n");
}

// OFFSET: %0x005B38B0
void DefaultAsyncObjectCleanupCallback(void* param, CAsyncObject* object) {
    AsyncFileReadDestroyObject(object);
}
