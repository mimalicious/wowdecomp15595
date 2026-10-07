#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00CA48EC
static const char* s_asyncQueueNames[2] = { "Disk Queue", "Net Queue" };

// STATIC: 0x00CA48F4
static uint32_t s_handlerTimeout = 100;

// STATIC: 0x00CA48F8
static int32_t s_topStreamingPriority = 0x7FFFFFFF;

// STATIC: 0x00CA48FC
static TSExplicitList<CAsyncQueue, offsetof(CAsyncQueue, m_link)> s_asyncQueueList;

// STATIC: 0x00CA4908
static TSExplicitList<CAsyncThread, offsetof(CAsyncThread, m_link)> s_asyncThreadList;

namespace AsyncFileRead {
    // STATIC: 0x00CA4914
    TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncFileReadFreeList;
}

// STATIC: 0x00CA4920
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncFileReadPostList;

// STATIC: 0x00DD3D00/0x140CBA528
static int32_t s_queueLockCount;

// STATIC: 0x00DD3D04/0x140CBA530
static void (*s_ingameStartCallback)();

// STATIC: 0x00DD3D08/0x140CBA538
static void (*s_ingameProgressCallback)(float, int32_t);

// STATIC: 0x00DD3D0C
static int32_t s_progressCount;

// STATIC: 0x00DD3D10/0x140CBA540
static void* s_asyncProgressCallbackParams;

// STATIC: 0x00DD3D14/0x140CBA548
static void (*s_asyncProgressCallback)(float, void*);

// STATIC: 0x00DD3D18
static CAsyncObject* s_asyncWaitObject;

// STATIC: 0x00DD3D1C
static HPROPCONTEXT s_propContext;

// STATIC: 0x00DD3D20
static CAsyncQueue* s_asyncQueues[2];

// STATIC: 0x00DD3D28
static uint32_t s_threadSleep;

// STATIC: 0x00DD3D2C
static int32_t s_streamingError;

// STATIC: 0x00DD3D30
static bool s_shuttingDown;

namespace AsyncFileRead {
    // STATIC: 0x00DD3D34
    SEvent s_shutdownEvent(1, 0);
}

// STATIC: 0x00DD3D38
static TSGrowableArray<void (*)()> s_asyncPollHandlers;

// STATIC: 0x00DD3D48
static TSGrowableArray<int32_t (*)()> s_asyncStatusHandlers;

// STATIC: 0x00DD3D58/0x140CBA5B8
SCritSect g_queueLock;

// STATIC: 0x00DD3D80
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

// OFFSET: 0x0053EA50
void AsyncFileGetNetQueueStatus(AsyncFileNetQueueStatus* status) {
    status->m_active = false;
    g_queueLock.Enter();

    if (s_asyncQueues[1]) {
        CAsyncObject* object = s_asyncQueues[1]->m_currentObject;

        if (object && object->m_file) {
            status->m_active = true;
            SFile::GetActualFileName(object->m_file, status->m_name, sizeof(status->m_name));
            status->m_local = 0;
            status->m_total = 0;
            SFile::FileGetIsLocalAmount(object->m_file, status->m_local, status->m_total);
        }
    }

    g_queueLock.Leave();
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

// PROVEN: bf657c8a IDENTICAL/NONE
// OFFSET: 0x0053EB10/0x1401995D0
float AsyncFileReadGetStreamingBandwidth() {
    if (s_asyncQueues[1]) {
        return s_asyncQueues[1]->m_bandwidth;
    }

    return 0.0f;
}

// PROVEN: c850dd30 IDENTICAL/NONE
// OFFSET: 0x0053EB20/0x1401995F0
static int CompareStreamingObjects(const void* a, const void* b) {
    int32_t priority1 = static_cast<const CStreamingObj*>(a)->m_priority;
    int32_t priority2 = static_cast<const CStreamingObj*>(b)->m_priority;

    if (priority1 < priority2) {
        return -1;
    }

    return priority1 > priority2;
}

// PROVEN: d421f6a9 IDENTICAL/NONE
// OFFSET: 0x0053EB50/0x140199610
int32_t AsyncFileReadHasStreamingErrorOccurred() {
    return s_streamingError;
}

// PROVEN: e8da7b18 IDENTICAL/NONE
// OFFSET: 0x0053EB60/0x140199620
void AsyncFileReadSetStreamingError() {
    s_streamingError = 1;
}

// PROVEN: 2fac8f36 IDENTICAL/UNPROVEN
// OFFSET: 0x0053EB70/0x140199630
void AsyncFileReadResetStreamingError() {
    s_streamingError = 0;
}

// PROVEN: da6d66e9 IDENTICAL/NONE
// OFFSET: 0x0053ED70/0x140199920
int32_t AsyncFileReadGetStreamingQueue() {
    int32_t queue = 3;
    g_queueLock.Enter();

    if (s_asyncQueues[1]) {
        CAsyncObject* object;

        if (s_asyncQueues[1]->m_currentObject) {
            object = s_asyncQueues[1]->m_currentObject;
        } else {
            object = s_asyncQueues[1]->m_readList.Head();
        }

        if (object) {
            if (object->m_priority < 3) {
                queue = 0;
            } else if (object->m_priority == 3) {
                queue = s_topStreamingPriority >= 0;
            } else {
                queue = 2;
            }
        }
    }

    SFile::PauseBackgroundStreaming(queue != 3);
    g_queueLock.Leave();

    return queue;
}

// PROVEN: 337a9368 IDENTICAL/NONE
// OFFSET: 0x0053EDF0/0x1401999A0
int32_t AsyncFileReadIsStreaming() {
    g_queueLock.Enter();

    int32_t streaming = 0;

    if (s_asyncQueues[1] && (s_asyncQueues[1]->m_readList.Head() || s_asyncQueues[1]->m_currentObject)) {
        streaming = 1;
    }

    g_queueLock.Leave();

    return streaming;
}

// PROVEN: 0e5f5e4c IDENTICAL/NONE
// OFFSET: 0x0053EE30
int32_t AsyncFileReadIsDiskReading() {
    g_queueLock.Enter();

    int32_t reading = 0;

    if (s_asyncQueues[0] && (s_asyncQueues[0]->m_readList.Head() || s_asyncQueues[0]->m_currentObject)) {
        reading = 1;
    }

    g_queueLock.Leave();

    return reading;
}

// OFFSET: 0x0053EE70/0x140199A40
int32_t AsyncFileReadComputePriority(int32_t category, int32_t type, int32_t visible, float distanceSq) {
    float distance = sqrt(distanceSq);
    int32_t priority = static_cast<int32_t>((distance < 1000.0f ? distance : 1000.0f) * 0.255f);

    if (!category) {
        return priority - 1000;
    }

    priority += category * 5;

    if (type == 1) {
        priority += 8;
    }

    if (!visible) {
        priority += 25;
    }

    return priority;
}

// OFFSET: 0x0053EED0/0x140199A90
int32_t AsyncFileReadComputeSoundPriority(int32_t urgent, float distanceSq) {
    float distance = sqrt(distanceSq);
    int32_t priority = static_cast<int32_t>((distance < 1000.0f ? distance : 1000.0f) * 0.255f);

    if (urgent) {
        return priority - 1000;
    }

    return priority + 15;
}

// OFFSET: 0x0053EF20/0x140199AC0
static void AsyncFileReadUnlinkObject(CAsyncObject* object) {
    CAsyncQueue* queue = object->m_queue;

    if (queue) {
        if (!object->m_link.IsLinked()) {
            return;
        }

        uint8_t priority = object->m_priority;

        if (queue->m_priorityHead[priority] == object) {
            CAsyncObject* next = object->m_link.Next();
            queue->m_priorityHead[priority] = !queue->m_readList.IsEnd(next) && next->m_priority == priority ? next : NULL;
        }

        priority = object->m_priority;

        if (object->m_queue->m_priorityTail[priority] == object) {
            CAsyncObject* prev = object->m_link.Prev();
            object->m_queue->m_priorityTail[priority] = !queue->m_readList.IsEnd(prev) && prev->m_priority == priority ? prev : NULL;
        }
    }

    object->m_link.Unlink();
}

// OFFSET: 0x0053EFF0
void AsyncFileReadUnloadPostList() {
    CAsyncObject* object;

    while ((object = s_asyncFileReadPostList.Head()) != NULL) {
        object->m_link.Unlink();

        if (object->m_userPostUnloadCallback) {
            object->m_userPostUnloadCallback(object);
        }
    }
}

// OFFSET: 0x0053F060/0x140199BA0
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
        object->m_isProcessed = 1;
        g_queueLock.Leave();

        void (*callback)(CAsyncObject*) = object->m_readSucceeded && !object->m_cancelled ? object->m_userPostloadCallback : object->m_userPostUnloadCallback;

        if (callback) {
            callback(object);
        }

        s_progressCount--;

        if (object == s_asyncWaitObject) {
            s_asyncWaitObject = NULL;
            return 1;
        }

        if (OsGetAsyncTimeMs() - start > s_handlerTimeout) {
            break;
        }
    }

    for (uint32_t i = 0; i < s_asyncPollHandlers.Count(); i++) {
        s_asyncPollHandlers[i]();
    }

    return 1;
}

// OFFSET: 0x0053F2B0/0x140199EF0
static void AsyncFileReadLinkToFront(CAsyncObject* object) {
    CAsyncQueue* queue = object->m_queue;
    CAsyncObject* head = queue->m_readList.Head();

    if (head != object) {
        queue->m_priorityHead[0] = object;

        if (!head || head->m_priority) {
            object->m_queue->m_priorityTail[0] = object;
        }

        object->m_priority = 0;
        queue->m_readList.LinkToHead(object);
    }
}

// OFFSET: 0x0053F2F0/0x140199F40
void AsyncFileReadLinkObject(CAsyncObject* object, int32_t urgent) {
    CAsyncQueue* queue = object->m_queue;

    if (!queue) {
        return;
    }

    uint8_t priority = object->m_priority;

    if (urgent) {
        if (queue->m_priorityHead[priority]) {
            if (queue->m_priorityHead[priority] != object) {
                AsyncFileReadUnlinkObject(object);
                object->m_queue->m_readList.LinkNode(object, 2, queue->m_priorityHead[priority]);
                queue->m_priorityHead[priority] = object;
            }

            return;
        }
    } else if (queue->m_priorityTail[priority]) {
        if (queue->m_priorityTail[priority] != object) {
            AsyncFileReadUnlinkObject(object);
            object->m_queue->m_readList.LinkNode(object, 1, queue->m_priorityTail[priority]);
            queue->m_priorityTail[priority] = object;
        }

        return;
    }

    for (CAsyncObject* other = queue->m_readList.Head(); !queue->m_readList.IsEnd(other); other = queue->m_readList.RawNext(other)) {
        if (priority <= other->m_priority && (urgent || priority != other->m_priority)) {
            queue->m_readList.LinkNode(object, 2, other);
            queue->m_priorityTail[priority] = object;
            queue->m_priorityHead[priority] = object;
            return;
        }
    }

    queue->m_readList.LinkToTail(object);
    queue->m_priorityTail[priority] = object;
    queue->m_priorityHead[priority] = object;
}

// OFFSET: 0x0053F460/0x14019A0D0
void AsyncFileReadAddPollHandler(void (*handler)()) {
    for (uint32_t i = 0; i < s_asyncPollHandlers.Count(); i++) {
        if (s_asyncPollHandlers[i] == handler) {
            return;
        }
    }

    s_asyncPollHandlers.New(handler);
}

// OFFSET: 0x0053F520/0x14019A1A0
void AsyncFileReadAddStatusHandler(int32_t (*handler)()) {
    for (uint32_t i = 0; i < s_asyncStatusHandlers.Count(); i++) {
        if (s_asyncStatusHandlers[i] == handler) {
            return;
        }
    }

    s_asyncStatusHandlers.New(handler);
}

// OFFSET: 0x0053F5E0/0x14019A270
static void AsyncFileReadFreeObject(CAsyncObject* object) {
    while (object->m_isCurrent) {
        DebugLog("AsyncFileReadDestroyObject()");
        g_queueLock.Leave();

        while (object->m_isCurrent) {
            OsSleep(1);
        }

        g_queueLock.Enter();
    }

    AsyncFileReadUnlinkObject(object);

    if (object->m_file && object->m_ownsFile) {
        SFileCloseFile(object->m_file);
        object->m_file = NULL;
    }

    object->m_inUse = 0;
    object->m_queue = NULL;
    object->m_buffer = NULL;
    AsyncFileRead::s_asyncFileReadFreeList.LinkToHead(object);
}

// OFFSET: 0x0053F670/0x14019A3F0
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent) {
    CAsyncQueue* queue = s_asyncQueues[0];

    if (SFile::IsStreamingMode()) {
        int32_t local = SFile::FileIsLocal(object->m_file, 6, 0);

        if (!local || local == 2) {
            SFile::PauseBackgroundStreaming(true);
            object->m_isStreamed = 1;
            queue = s_asyncQueues[1];
        }
    }

    g_queueLock.Enter();
    object->m_queue = queue;

    if (object != s_asyncWaitObject) {
        AsyncFileReadLinkObject(object, urgent);
    } else {
        AsyncFileReadLinkToFront(object);
    }

    g_queueLock.Leave();
}

// OFFSET: 0x0053F720/0x14019A4C0
void AsyncFileReadLinkToPriority(CAsyncObject* object, int32_t urgent) {
    if (!object->m_isProcessed && !object->m_isRead && object->m_queue) {
        AsyncFileReadLinkObject(object, urgent);
    }
}

// OFFSET: 0x0053F750
int32_t NetQueuePromoteLocalFiles(const void* data, void* param) {
    // STATIC: 0x00DD3D90
    static uint32_t s_lastPromoteTime;

    uint32_t time = OsGetAsyncTimeMs();

    if (time - s_lastPromoteTime >= 250) {
        s_lastPromoteTime = time;
        g_queueLock.Enter();

        for (CAsyncObject* object = s_asyncQueues[1]->m_readList.Head(), * next; !s_asyncQueues[1]->m_readList.IsEnd(object); object = next) {
            next = s_asyncQueues[1]->m_readList.RawNext(object);

            if (!object->m_isCurrent && SFile::FileIsLocal(object->m_file, 0, 0) == 1) {
                AsyncFileReadUnlinkObject(object);
                object->m_isStreamed = 0;
                object->m_queue = s_asyncQueues[0];
                AsyncFileReadLinkObject(object, 0);
            }
        }

        g_queueLock.Leave();
    }

    return 1;
}

// OFFSET: 0x0053F800/0x14019A720
void AsyncFileReadWait(CAsyncObject* object) {
    // STATIC: 0x00DD3D94
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

    if (!object->m_isRead && !object->m_isCurrent) {
        AsyncFileReadUnlinkObject(object);
        AsyncFileReadLinkToFront(object);
    }

    g_queueLock.Leave();

    if (s_ingameStartCallback) {
        s_ingameStartCallback();
    }

    while (true) {
        g_queueLock.Enter();

        if (s_ingameProgressCallback && object->m_file) {
            int64_t local = 0;
            int64_t total = 0;
            SFile::FileGetIsLocalAmount(object->m_file, local, total);
            s_ingameProgressCallback(static_cast<float>(static_cast<double>(local) / static_cast<double>(total)), 0);
        }

        g_queueLock.Leave();
        AsyncFileReadPollHandler(NULL, NULL);

        if (!s_asyncWaitObject) {
            break;
        }

        OsSleep(1);
    }

    s_waiting--;
}

// PROVEN: fa22b59b IDENTICAL/NONE
// OFFSET: 0x0053F900
int32_t AsyncFileReadIsReading() {
    g_queueLock.Enter();

    int32_t reading = 0;

    for (CAsyncQueue* queue = s_asyncQueueList.Head(), * next; !s_asyncQueueList.IsEnd(queue); queue = next) {
        next = s_asyncQueueList.RawNext(queue);

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

// OFFSET: 0x0053F970/0x14019A860
void AsyncFileReadAddStreamingObject(void (*callback)(void*), void* param, int32_t priority) {
    CStreamingObj* object = s_streamingObjects.New();
    object->m_callback = callback;
    object->m_param = param;
    object->m_priority = priority;
}

// OFFSET: 0x0053F990/0x14019A8A0
void AsyncFileReadClearStreamingObjectList() {
    s_streamingObjects.SetCount(0);
}

// OFFSET: 0x0053F9A0/0x14019A8B0
void AsyncFileReadUpdateStreamingObjectList() {
    if (!s_streamingObjects.Count()) {
        return;
    }

    qsort(s_streamingObjects.Ptr(), s_streamingObjects.Count(), sizeof(CStreamingObj), CompareStreamingObjects);
    AsyncFileReadEnterQueueLock();

    for (int32_t i = s_streamingObjects.Count() - 1; i >= 0; i--) {
        s_streamingObjects[i].m_callback(s_streamingObjects[i].m_param);
    }

    s_topStreamingPriority = s_streamingObjects[0].m_priority;
    AsyncFileReadLeaveQueueLock();
    s_streamingObjects.SetCount(0);
}

// OFFSET: 0x0053FA40/0x14019A980
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

            if (SFile::IsStreamingMode()) {
                uint64_t start = OsGetAsyncTimeClocks();

                if (!SFile::MakeFileLocal(object->m_file, true, 0x20000, &status, object->m_offset, object->m_size)) {
                    status = 2;
                } else {
                    int64_t elapsed = OsGetAsyncTimeClocks() - start;

                    if (elapsed > 0 && !SFile::IsPatchFile(object->m_file) && object->m_ownsFile) {
                        float seconds = static_cast<float>(static_cast<double>(elapsed) / static_cast<double>(OsGetAsyncClocksPerSecond()));
                        uint32_t size = SFileGetFileSize(object->m_file, NULL) >= 0x20000 ? 0x20000 : SFileGetFileSize(object->m_file, NULL);
                        float bandwidth = static_cast<float>(static_cast<double>(static_cast<int64_t>(size) * 8) / seconds * 0.000001f);

                        if (bandwidth < 1000.0f) {
                            if (queue->m_bandwidth != 0.0f) {
                                queue->m_bandwidth = queue->m_bandwidth * 0.8f + bandwidth * 0.2f;
                            } else {
                                queue->m_bandwidth = bandwidth;
                            }
                        }
                    }
                }
            }

            if (object->m_cancelled) {
                g_queueLock.Enter();
                AsyncFileReadUnlinkObject(object);
                object->m_queue = NULL;
                queue->m_currentObject = NULL;
                object->m_isCurrent = 0;
                s_asyncFileReadPostList.LinkToTail(object);
            } else if (status == 1) {
                g_queueLock.Enter();
                queue->m_currentObject = NULL;
                object->m_isCurrent = 0;
            } else {
                int32_t succeeded = 0;

                if (!status) {
                    void* buffer = NULL;

                    if (!object->m_buffer) {
                        if (SFile::IsPatchFile(object->m_file)) {
                            buffer = SMemAlloc(object->m_size, __FILE__, __LINE__, 0);
                        } else {
                            succeeded = 1;
                        }
                    }

                    int32_t retries = 10;

                    do {
                        retries--;

                        if (succeeded) {
                            break;
                        }

                        succeeded = SFile::ReadAtPosition(object->m_file, object->m_buffer ? object->m_buffer : buffer, object->m_offset, object->m_size, NULL);
                    } while (retries);

                    if (buffer) {
                        SMemFree(buffer, __FILE__, __LINE__, 0);
                    }
                }

                g_queueLock.Enter();

                if (object->m_ownsFile) {
                    SFileCloseFile(object->m_file);
                    object->m_file = NULL;
                }

                AsyncFileReadUnlinkObject(object);
                object->m_queue = NULL;
                queue->m_currentObject = NULL;
                object->m_isCurrent = 0;
                object->m_readSucceeded = succeeded;
                object->m_isRead = 1;
                s_asyncFileReadPostList.LinkToTail(object);
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

// OFFSET: 0x0053FE20/0x14019AEA0
void AsyncFileReadCreateThread(CAsyncQueue* queue, const char* name) {
    CAsyncThread* thread = s_asyncThreadList.NewNode(2, 0, 0);
    thread->m_queue = queue;
    queue->m_currentObject = NULL;
    SThread::Create(AsyncFileReadThread, thread, thread->m_thread, const_cast<char*>(name), 0);
}

// OFFSET: 0x0053FEE0/0x14019AFB0
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
    object->m_offset = 0;
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
    object->m_ownsFile = 1;
    object->m_inUse = 1;
    object->m_priority = 2;

    return object;
}

// PROVEN: c408728d IDENTICAL/NONE
// OFFSET: 0x0053FFA0/0x14019A3B0
void AsyncFileReadDestroyObject(CAsyncObject* object) {
    g_queueLock.Enter();
    AsyncFileReadFreeObject(object);
    g_queueLock.Leave();
}

// PROVEN: c5f4f3a2 IDENTICAL/NONE
// OFFSET: 0x0053FFA0/0x14019A3B0
void DefaultAsyncObjectCleanupCallback(CAsyncObject* object) {
    g_queueLock.Enter();
    AsyncFileReadFreeObject(object);
    g_queueLock.Leave();
}

// PROVEN: ec1efc1b IDENTICAL/NONE
// OFFSET: 0x0053FFD0/0x14019B180
int32_t AsyncFileReadCancel(CAsyncObject* object, void (*callback)(CAsyncObject* object)) {
    int32_t result = 0;
    g_queueLock.Enter();

    if (object->m_isProcessed) {
    } else if (!object->m_isRead && !object->m_isCurrent) {
        AsyncFileReadFreeObject(object);
        result = 1;
    } else {
        if (!callback) {
            callback = object->m_userPostUnloadCallback;

            if (!callback) {
                callback = DefaultAsyncObjectCleanupCallback;
                object->m_userPostUnloadCallback = callback;
            }
        } else {
            object->m_userPostUnloadCallback = callback;
        }

        object->m_userPostloadCallback = callback;
        object->m_cancelled = 1;
    }

    g_queueLock.Leave();

    return result;
}

// OFFSET: 0x00540040/0x14019B220
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

// OFFSET: 0x00540120/0x14019B390
void AsyncFileReadCreateObject(SFileImpl* file, uint8_t priority, int32_t urgent, CAsyncObject** result, void* buffer, void* userArg, void (*postload)(CAsyncObject*), void (*failed)(CAsyncObject*), uint16_t userFlags, uint32_t offset, uint32_t size) {
    if (result) {
        *result = NULL;
    }

    int32_t wholeFile = 0;

    if (!size) {
        size = SFileGetFileSize(file, NULL);
        wholeFile = 1;
    } else if (size == SFileGetFileSize(file, NULL)) {
        wholeFile = 1;
    }

    if (buffer || !SFile::FileIsLocal(file, offset, size)) {
        CAsyncObject* object = AsyncFileReadAllocObject();
        object->m_buffer = buffer;
        object->m_file = file;
        object->m_offset = offset;
        object->m_ownsFile = wholeFile;
        object->m_userPostloadCallback = postload;
        object->m_userArg = userArg;
        object->m_userFlags = userFlags;
        object->m_size = size;
        object->m_userPostUnloadCallback = failed;
        object->m_priority = priority;
        AsyncFileReadObject(object, urgent);

        if (result) {
            *result = object;
        }
    } else if (wholeFile) {
        SFileCloseFile(file);
    }
}

// OFFSET: 0x005401F0/0x14019B4C0
int32_t AsyncFileReadCreateObject(const char* name, uint8_t priority, int32_t urgent, int32_t noHash, CAsyncObject** result, void* buffer, void* userArg, void (*postload)(CAsyncObject*), void (*failed)(CAsyncObject*), uint16_t userFlags, uint32_t offset, uint32_t size) {
    if (noHash) {
        SFile::EnableHash(false);
    }

    SFileImpl* file;

    if (!SFileOpenFile(name, &file)) {
        SFile::EnableHash(true);

        if (result) {
            *result = NULL;
        }

        return 0;
    }

    if (noHash) {
        SFile::EnableHash(true);
    }

    AsyncFileReadCreateObject(file, priority, urgent, result, buffer, userArg, postload, failed, userFlags, offset, size);

    return 1;
}

// OFFSET: 0x00540370
CAsyncQueue::~CAsyncQueue() {
}

// PROVEN: 16c5e7ea IDENTICAL/NONE
// OFFSET: 0x00540400
CAsyncQueue::CAsyncQueue() {
    for (int32_t i = 0; i < 8; i++) {
        this->m_priorityTail[i] = NULL;
        this->m_priorityHead[i] = NULL;
    }

    this->m_bandwidth = 0.0f;
}

// OFFSET: 0x00540460/0x14019B670
CAsyncQueue* AsyncFileReadCreateQueue() {
    return s_asyncQueueList.NewNode(2, 0, 0);
}

// OFFSET: 0x005404F0/0x14019B7A0
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

    int32_t queues = SFile::IsStreamingMode() ? 2 : 1;

    for (int32_t i = 0; i < queues; i++) {
        s_asyncQueues[i] = AsyncFileReadCreateQueue();
        AsyncFileReadCreateThread(s_asyncQueues[i], s_asyncQueueNames[i]);
    }

    s_queueLockCount = 0;
}

// OFFSET: 0x005405E0
void AsyncFileReadDestroy() {
    s_shuttingDown = true;

    for (CAsyncThread* thread = s_asyncThreadList.Head(), * next; !s_asyncThreadList.IsEnd(thread); thread = next) {
        next = s_asyncThreadList.RawNext(thread);
        AsyncFileRead::s_shutdownEvent.Set();
        thread->m_thread.Wait(0xFFFFFFFF);
    }

    s_shuttingDown = false;
    AsyncFileReadUnloadPostList();
    AsyncFileRead::s_asyncFileReadFreeList.Clear();

    for (CAsyncQueue* queue = s_asyncQueueList.Head(); !s_asyncQueueList.IsEnd(queue); queue = s_asyncQueueList.RawNext(queue)) {
    }

    s_asyncQueueList.Clear();
    s_asyncQueues[0] = NULL;
    s_asyncQueues[1] = NULL;
    s_asyncThreadList.Clear();
    s_asyncPollHandlers.SetCount(0);
    s_asyncStatusHandlers.SetCount(0);
    s_asyncProgressCallback = NULL;
    s_asyncProgressCallbackParams = NULL;
    s_ingameProgressCallback = NULL;
    s_ingameStartCallback = NULL;
    s_topStreamingPriority = 0x7FFFFFFF;

    if (SFile::IsStreamingMode()) {
        EventUnregisterEx(EVENT_ID_IDLE, NetQueuePromoteLocalFiles, NULL, 2);
    }

    EventUnregisterEx(EVENT_ID_POLL, AsyncFileReadPollHandler, NULL, 0xFFFFFFFF);
}
