#ifndef ENGINE_SOURCE_SERVICES_ASYNCFILE_H
#define ENGINE_SOURCE_SERVICES_ASYNCFILE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SThread.h"

class CAsyncQueue : public TSLinkedNode<CAsyncQueue> {
    public:
    /* 0008 */ TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> m_readList;
    /* 0014 */ CAsyncObject* m_currentObject;

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
    /* 0008 */ float m_distance;
    /* 000C */ int16_t m_priority;
    /* 000E */ int16_t m_subPriority;
};

void AsyncFileReadAddPollHandler(void (*handler)());
void AsyncFileReadAddStatusHandler(int32_t (*handler)());
void AsyncFileReadAddStreamingObject(void (*callback)(void*), void* param, float distance, int16_t priority, int16_t subPriority);
CAsyncObject* AsyncFileReadAllocObject();
int32_t AsyncFileReadCancel(CAsyncObject* object, void (*callback)(void* param, CAsyncObject* object));
void AsyncFileReadClearStreamingObjectList();
CAsyncQueue* AsyncFileReadCreateQueue();
void AsyncFileReadCreateThread(CAsyncQueue* queue, const char* name);
void AsyncFileReadDestroy();
void AsyncFileReadDestroyObject(CAsyncObject* object);
void AsyncFileReadEnterQueueLock();
CAsyncObject* AsyncFileReadGetCurrentObject(int32_t queue);
int32_t AsyncFileReadGetStreamingQueue();
int32_t AsyncFileReadHasStreamingErrorOccurred();
void AsyncFileReadIngameProgressCallback(void (*startCallback)(), void (*progressCallback)(float, int32_t));
void AsyncFileReadInitialize(uint32_t threadSleep, uint32_t handlerTimeout);
int32_t AsyncFileReadIsReading();
int32_t AsyncFileReadIsStreaming();
void AsyncFileReadLeaveQueueLock();
void AsyncFileReadLinkObject(CAsyncObject* object, int32_t urgent);
void AsyncFileReadLinkToPriority(CAsyncObject* object, int32_t urgent);
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent);
int32_t AsyncFileReadPollHandler(const void* data, void* param);
void AsyncFileReadProgressCallback(void (*callback)(float, void*), void* param);
void AsyncFileReadResetStreamingError();
uint32_t STORMAPI AsyncFileReadThread(void* param);
void AsyncFileReadUnloadPostList();
void AsyncFileReadUpdateStreamingObjectList();
void AsyncFileReadWait(CAsyncObject* object);
void AsyncFileReadWaitAll();
void DefaultAsyncObjectCleanupCallback(void* param, CAsyncObject* object);
int32_t NetQueuePromoteLocalFiles(const void* data, void* param);

#endif
