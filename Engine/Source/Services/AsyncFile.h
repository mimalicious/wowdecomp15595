#ifndef ENGINE_SOURCE_SERVICES_ASYNCFILE_H
#define ENGINE_SOURCE_SERVICES_ASYNCFILE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SThread.h"

class CAsyncQueue {
    public:
    /* 0000 */ float m_bandwidth;
    /* 0004 */ TSLink<CAsyncQueue> m_link;
    /* 000C */ TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> m_readList;
    /* 0018 */ CAsyncObject* m_currentObject;
    /* 001C */ CAsyncObject* m_priorityHead[8];
    /* 003C */ CAsyncObject* m_priorityTail[8];

    CAsyncQueue();
    ~CAsyncQueue();
};

class CAsyncThread : public TSLinkedNode<CAsyncThread> {
    public:
    /* 0008 */ SThread m_thread;
    /* 000C */ CAsyncQueue* m_queue;
};

struct CStreamingObj {
    /* 0000 */ void (*m_callback)(void*);
    /* 0004 */ void* m_param;
    /* 0008 */ int32_t m_priority;
};

struct AsyncFileNetQueueStatus {
    /* 0000 */ bool m_active;
    /* 0001 */ char m_name[260];
    /* 0108 */ int64_t m_local;
    /* 0110 */ int64_t m_total;
};

void AsyncFileGetNetQueueStatus(AsyncFileNetQueueStatus* status);
void AsyncFileReadAddPollHandler(void (*handler)());
void AsyncFileReadAddStatusHandler(int32_t (*handler)());
void AsyncFileReadAddStreamingObject(void (*callback)(void*), void* param, int32_t priority);
CAsyncObject* AsyncFileReadAllocObject();
int32_t AsyncFileReadCancel(CAsyncObject* object, void (*callback)(CAsyncObject* object));
void AsyncFileReadClearStreamingObjectList();
int32_t AsyncFileReadComputePriority(int32_t category, int32_t type, int32_t visible, float distanceSq);
int32_t AsyncFileReadComputeSoundPriority(int32_t urgent, float distanceSq);
void AsyncFileReadCreateObject(SFileImpl* file, uint8_t priority, int32_t urgent, CAsyncObject** result, void* buffer, void* userArg, void (*postload)(CAsyncObject*), void (*failed)(CAsyncObject*), uint16_t userFlags, uint32_t offset, uint32_t size);
int32_t AsyncFileReadCreateObject(const char* name, uint8_t priority, int32_t urgent, int32_t noHash, CAsyncObject** result, void* buffer, void* userArg, void (*postload)(CAsyncObject*), void (*failed)(CAsyncObject*), uint16_t userFlags, uint32_t offset, uint32_t size);
CAsyncQueue* AsyncFileReadCreateQueue();
void AsyncFileReadCreateThread(CAsyncQueue* queue, const char* name);
void AsyncFileReadDestroy();
void AsyncFileReadDestroyObject(CAsyncObject* object);
void AsyncFileReadEnterQueueLock();
float AsyncFileReadGetStreamingBandwidth();
int32_t AsyncFileReadGetStreamingQueue();
int32_t AsyncFileReadHasStreamingErrorOccurred();
void AsyncFileReadIngameProgressCallback(void (*startCallback)(), void (*progressCallback)(float, int32_t));
void AsyncFileReadInitialize(uint32_t threadSleep, uint32_t handlerTimeout);
int32_t AsyncFileReadIsDiskReading();
int32_t AsyncFileReadIsReading();
int32_t AsyncFileReadIsStreaming();
void AsyncFileReadLeaveQueueLock();
void AsyncFileReadLinkObject(CAsyncObject* object, int32_t urgent);
void AsyncFileReadLinkToPriority(CAsyncObject* object, int32_t urgent);
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent);
int32_t AsyncFileReadPollHandler(const void* data, void* param);
void AsyncFileReadProgressCallback(void (*callback)(float, void*), void* param);
void AsyncFileReadResetStreamingError();
void AsyncFileReadSetStreamingError();
uint32_t STORMAPI AsyncFileReadThread(void* param);
void AsyncFileReadUnloadPostList();
void AsyncFileReadUpdateStreamingObjectList();
void AsyncFileReadWait(CAsyncObject* object);
void AsyncFileReadWaitAll();
void DefaultAsyncObjectCleanupCallback(CAsyncObject* object);
int32_t NetQueuePromoteLocalFiles(const void* data, void* param);

#endif
