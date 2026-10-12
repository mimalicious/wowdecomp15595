#include <winsock2.h>
#include <windows.h>
#include "WoW/Source/WowServices/WowConnectionNet.h"

// OFFSET: 0x00550D00/0x1401ABC10
void WowConnectionNet::PlatformChangeState(WowConnection* connection, WOW_CONN_STATE state) {
    long networkEvents = 0;

    switch (connection->m_connState) {
        case WOWC_CONNECTING:
            networkEvents = FD_CONNECT | FD_CLOSE | FD_WRITE;
            break;

        case WOWC_LISTENING:
            networkEvents = FD_ACCEPT;
            break;

        case WOWC_CONNECTED:
            networkEvents = connection->m_sendList.Head() || connection->m_notifyWriteReady ? FD_READ | FD_WRITE | FD_CLOSE : FD_READ | FD_CLOSE;
            break;
    }

    if (connection->m_event && connection->m_sock >= 0) {
        WSAEventSelect(connection->m_sock, connection->m_event, networkEvents);
    }

    SetEvent(this->m_event);
}

// OFFSET: 0x00550AA0/0x1401AB930
void WowConnectionNet::PlatformRun() {
    this->m_networkStarted = true;

    DWORD timeout = 500;

    if (this->m_stop) {
        return;
    }

    do {
        WowConnection* connections[MAXIMUM_WAIT_OBJECTS];
        HANDLE handles[MAXIMUM_WAIT_OBJECTS];

        this->m_connectionsLock.Enter();

        connections[0] = NULL;
        handles[0] = this->m_event;
        DWORD count = 1;

        for (WowConnection* connection = this->m_connections.Head(); connection; connection = this->m_connections.Next(connection)) {
            if (connection->m_serviceCount) {
                continue;
            }

            HANDLE event = connection->m_event;

            switch (connection->m_connState) {
                case WOWC_CONNECTING:
                    WSAEventSelect(connection->m_sock, event, FD_CONNECT | FD_CLOSE);
                    break;

                case WOWC_LISTENING:
                    WSAEventSelect(connection->m_sock, event, FD_ACCEPT);
                    break;

                case WOWC_CONNECTED:
                    WSAEventSelect(connection->m_sock, event, (connection->m_sendList.Head() || connection->m_notifyWriteReady ? FD_WRITE : 0) | FD_READ | FD_CLOSE);
                    break;

                case WOWC_DISCONNECTING:
                    timeout = 0;
                    break;

                default:
                    continue;
            }

            if (count < MAXIMUM_WAIT_OBJECTS) {
                connection->m_refCount.Increment();
                handles[count] = event;
                connections[count] = connection;
                count++;

                if (connection->m_lagTestRecvList.Head()) {
                    timeout = 0;
                }
            }
        }

        this->m_connectionsLock.Leave();

        DWORD result = WaitForMultipleObjects(count, handles, FALSE, timeout);

        if (result < count) {
            if (result == WAIT_OBJECT_0) {
                ResetEvent(this->m_event);
            }

            for (DWORD i = 1; i < count; i++) {
                WowConnection* connection = connections[i];

                WSANETWORKEVENTS networkEvents;
                WSAEnumNetworkEvents(connection->m_sock, handles[i], &networkEvents);

                uint32_t flags = 0;

                if (networkEvents.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE)) {
                    flags = 2;
                }

                if (networkEvents.lNetworkEvents & (FD_WRITE | FD_CONNECT | FD_CLOSE)) {
                    flags |= 1;
                }

                if (connection->m_connState == WOWC_DISCONNECTING) {
                    flags |= 8;
                }

                if (WowConnection::s_lagTestDelayMin || WowConnection::s_lagTestDelayMax) {
                    flags |= 3;
                    timeout = 10;
                } else {
                    timeout = 500;
                }

                if (flags) {
                    this->SignalWorker(connection, flags);
                }
            }
        }

        for (DWORD i = 1; i < count; i++) {
            connections[i]->Release();
        }
    } while (!this->m_stop);
}

// PROVEN: 34a5c2ac IDENTICAL/IDENTICAL
// OFFSET: 0x005509A0/0x1401AB810
void WowConnectionNet::PlatformInit(bool useEngine) {
    WSADATA data;

    if (!WSAStartup(MAKEWORD(2, 2), &data) && data.wVersion == MAKEWORD(2, 2)) {
        this->m_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    }
}

// PROVEN: 6f9d5ebc IDENTICAL/IDENTICAL
// OFFSET: 0x00550A00/0x1401AB880
void WowConnectionNet::PlatformDestruct(WowConnection* connection) {
    if (connection->m_event) {
        WSACloseEvent(connection->m_event);
        connection->m_event = NULL;
    }
}

// PROVEN: 707e3f8c IDENTICAL/IDENTICAL
// OFFSET: 0x00550A30/0x1401AB8B0
void WowConnectionNet::PlatformWorkerReady() {
    SetEvent(this->m_event);
}

// PROVEN: 43a32745 IDENTICAL/DIFFERENT
// OFFSET: 0x00550A40/0x1401AB8C0
void WowConnectionNet::PlatformAdd(WowConnection* connection) {
    if (!connection->m_event) {
        connection->m_event = WSACreateEvent();
    }

    BOOL noDelay = TRUE;
    setsockopt(connection->m_sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&noDelay), sizeof(noDelay));

    SetEvent(this->m_event);
}

// PROVEN: 63b0ea27 IDENTICAL/IDENTICAL
// OFFSET: 0x00550A90/0x1401AB8B0
void WowConnectionNet::PlatformRemove(WowConnection* connection) {
    SetEvent(this->m_event);
}

// PROVEN: 9306efd9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WowConnectionNet::PlatformStop() {
}

// INLINED
void WowConnectionNet::PlatformDestroy() {
    WSACleanup();
}
