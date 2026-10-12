#include "WowConnectionNet.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: dcd6c1c3 IDENTICAL/IDENTICAL
// OFFSET: 0x00550330/0x1401AAF40
static uint32_t STORMAPI NetworkThreadProc(void* param) {
    WowConnectionNet* network = static_cast<WowConnectionNet*>(param);
    network->PlatformRun();
    network->m_stopEvent.Set();

    return 0;
}

// PROVEN: d6518a0d IDENTICAL/IDENTICAL
// OFFSET: 0x005505D0/0x1401AB2A0
static uint32_t STORMAPI WorkerThreadProc(void* param) {
    WowConnectionNet::WORKER* worker = static_cast<WowConnectionNet::WORKER*>(param);
    worker->m_owner->RunWorker(worker->m_index);

    return 0;
}

// INLINED
WowConnectionNet::WORKER::WORKER() : m_event(0, 0) {
}

// OFFSET: 0x005505F0/0x1401AB2C0
void WowConnectionNet::Start() {
    char name[32];
    uint32_t i = 0;

    for (; static_cast<int32_t>(i) < this->m_numWorkers; i++) {
        WowConnectionNet::WORKER& worker = this->m_workers[i];
        worker.m_index = i;
        worker.m_connection = NULL;
        worker.m_quit = false;
        worker.m_owner = this;
        worker.m_lastTime = OsGetAsyncTimeMs();
        SStrPrintf(name, sizeof(name), "NetThread %d", i);
        SThread::Create(WorkerThreadProc, &worker, worker.m_thread, name, 0);
    }

    for (; i < sizeof(this->m_workers) / sizeof(this->m_workers[0]); i++) {
        WowConnectionNet::WORKER& worker = this->m_workers[i];
        worker.m_index = 0;
        worker.m_connection = NULL;
        worker.m_quit = true;
        worker.m_owner = this;
    }

    SStrPrintf(name, sizeof(name), "Network");
    this->m_networkStarted = false;
    SThread::Create(NetworkThreadProc, this, this->m_thread, name, 0);

    while (!this->m_networkStarted) {
        OsSleep(100);
    }
}

// PROVEN: a83d2922 IDENTICAL/IDENTICAL
// OFFSET: 0x005502C0/0x1401AAEB0
void WowConnectionNet::Service(WowConnection* connection, uint32_t flags) {
    do {
        if (flags & 0x1) {
            connection->DoWrites();
        }

        if (flags & 0x2) {
            connection->DoReads();
        }

        if (flags & 0x4) {
            connection->DoExceptions();
        }

        if (flags & 0x8) {
            connection->DoDisconnect();
        }

        this->m_connectionsLock.Enter();
        flags = connection->m_serviceFlags;
        connection->m_serviceFlags = 0;
        this->m_connectionsLock.Leave();
    } while (flags);
}

// OFFSET: 0x00550430/0x1401AB070
void WowConnectionNet::RunWorker(int32_t index) {
    if (this->m_threadInit) {
        this->m_threadInit();
    }

    this->m_workerSemaphore.Signal(1);

    WowConnectionNet::WORKER& worker = this->m_workers[index];

    while (true) {
        do {
            worker.m_lastTime = OsGetAsyncTimeMs();
        } while (worker.m_event.Wait(1000));

        if (worker.m_quit) {
            break;
        }

        uint32_t serviceFlags = worker.m_connection->m_serviceFlags;
        worker.m_connection->m_serviceFlags = 0;
        this->Service(worker.m_connection, serviceFlags);

        worker.m_lock.Enter();
        InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&worker.m_connection->m_serviceCount), -1);
        worker.m_connection->Release();
        worker.m_connection = NULL;
        worker.m_lock.Leave();

        this->m_workerSemaphore.Signal(1);
        this->PlatformWorkerReady();
    }
}

// OFFSET: 0x005504F0/0x1401AB170
void WowConnectionNet::SignalWorker(WowConnection* connection, uint32_t flags) {
    if (this->m_workerSemaphore.Wait(500)) {
        return;
    }

    connection->m_refCount.Increment();
    connection->m_serviceFlags = flags;
    InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&connection->m_serviceCount), 1);

    for (int32_t i = 0; i < this->m_numWorkers; i++) {
        WowConnectionNet::WORKER& worker = this->m_workers[i];
        worker.m_lock.Enter();

        if (!worker.m_connection) {
            worker.m_connection = connection;
            worker.m_lock.Leave();
            worker.m_event.Set();
            return;
        }

        worker.m_lock.Leave();
    }
}

// OFFSET: 0x00550590/0x1401AB240
void WowConnectionNet::Delete(WowConnection* connection) {
    this->m_connectionsLock.Enter();

    if (!connection->m_refCount.m_value) {
        delete connection;
    }

    this->m_connectionsLock.Leave();
}

// PROVEN: a5a8e9af IDENTICAL/IDENTICAL
// OFFSET: 0x005508B0/0x1401AB6E0
WowConnectionNet::WowConnectionNet(int32_t numThreads, void (*threadInit)()) : m_stopEvent(1, 0), m_workerSemaphore(0, numThreads), m_serviceSemaphore(0, 0x7FFFFFFF) {
    this->m_numWorkers = numThreads;
    this->m_threadInit = threadInit;
    this->m_stop = false;
}

// PROVEN: 9285fc03 IDENTICAL/UNPROVEN
// OFFSET: 0x005506E0/0x1401AB3F0
void WowConnectionNet::Add(WowConnection* connection) {
    this->m_connectionsLock.Enter();

    if (!this->m_connections.IsLinked(connection)) {
        this->m_connections.LinkToTail(connection);
        this->PlatformAdd(connection);
    }

    this->m_connectionsLock.Leave();
}

// PROVEN: b9fc6366 IDENTICAL/DIFFERENT
// OFFSET: 0x00550730/0x1401AB460
void WowConnectionNet::Remove(WowConnection* connection) {
    this->m_connectionsLock.Enter();

    if (this->m_connections.IsLinked(connection)) {
        this->m_connections.UnlinkNode(connection);
    }

    this->PlatformRemove(connection);
    this->m_connectionsLock.Leave();
}

// OFFSET: 0x00550350/0x1401AAF60
void WowConnectionNet::Stop() {
    this->m_stop = true;
    this->PlatformStop();
    this->PlatformWorkerReady();

    if (this->m_stopEvent.Wait(10000)) {
        ConsolePrintf("Main network thread did not send stop event normally");
    }

    if (this->m_thread.Wait(5000)) {
        ConsolePrintf("Main network thread did not exit normally");
    }

    for (int32_t i = 0; i < this->m_numWorkers; i++) {
        WowConnectionNet::WORKER& worker = this->m_workers[i];
        worker.m_quit = true;
        worker.m_event.Set();
        this->m_serviceSemaphore.Signal(1);

        if (worker.m_thread.Wait(10000)) {
            ConsolePrintf("Network thread %d did not exit normally", i);
        } else if (worker.m_connection) {
            InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&worker.m_connection->m_serviceCount), -1);
            worker.m_connection->Release();
            worker.m_connection = NULL;
        }
    }
}

// PROVEN: 91633125 IDENTICAL/DIFFERENT
// OFFSET: 0x00550780/0x1401AB520
WowConnectionNet::~WowConnectionNet() {
}
