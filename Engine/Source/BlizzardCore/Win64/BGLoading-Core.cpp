#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <winsock2.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/BGLoading-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Net/Net.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: 0x00D05894
static Blizzard::Net::Socket s_launcherSocket = INVALID_SOCKET;

// STATIC: 0x00EB3B94
static bool s_restartPending;

// STATIC: 0x00EB3B98
static int32_t s_connectRetries;

// STATIC: 0x00EB3B9C
static bool s_notifiedPaused;

// STATIC: 0x00EB3BA0
static int32_t s_notifyState;

// STATIC: 0x00EB3BB0
static char s_session[32];

// STATIC: 0x00EB3BD0
static char s_request[128];

// STATIC: 0x00EB3C50
static uint32_t s_requestLength;

// STATIC: 0x00EB3C54
static bool s_requestInitialized;

// OFFSET: 0x0079FF80
static void HexEncode(int32_t count, const uint8_t* src, char* dst) {
    while (count) {
        count--;
        uint8_t high = *src >> 4;
        *dst++ = high + (high > 9 ? 'A' - 10 : '0');
        uint8_t low = *src++ & 0xF;
        *dst++ = low + (low > 9 ? 'A' - 10 : '0');
    }
}

// OFFSET: 0x007A2970/0x1404994F0
static void NotifyLauncher() {
    if (!s_requestInitialized) {
        DWORD processId = GetCurrentProcessId();
        Blizzard::Time::Timestamp now = Blizzard::Time::GetTimestamp();
        HexEncode(sizeof(processId), reinterpret_cast<const uint8_t*>(&processId), s_session);
        HexEncode(sizeof(now), reinterpret_cast<const uint8_t*>(&now), s_session + sizeof(processId));
        s_session[sizeof(processId) + sizeof(now)] = '\0';
        Blizzard::String::Copy(s_request, "GET /prelim/bgdl?session=", sizeof(s_request));
        Blizzard::String::Append(s_request, s_session, sizeof(s_request));
        Blizzard::String::Append(s_request, "&action=", sizeof(s_request));
        s_requestLength = Blizzard::String::Length(s_request);
        s_requestInitialized = true;
    }

    // STATIC: 0x00EB3BA8
    static uint32_t s_lastNotify = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

    // STATIC: 0x00EB3BA4
    static uint32_t s_connectStart = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

    bool paused = BGLoading::BGLoadManager::Get()->m_paused;

    if (!s_notifyState) {
        if (paused == s_notifiedPaused) {
            return;
        }

        if (paused || Blizzard::Time::GetTimeElapsed(s_lastNotify, static_cast<uint32_t>(Blizzard::Time::Milliseconds())) > 1000) {
            s_notifyState = 1;
        }
    }

    if (s_notifyState == 1) {
        Blizzard::Net::ConnectHost("127.0.0.1", 9113, &s_launcherSocket, 0, 0, false);
        s_connectStart = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
        s_notifyState = 2;
    }

    if (s_notifyState == 2) {
        Blizzard::Net::Socket readSockets[2] = { s_launcherSocket, INVALID_SOCKET };
        Blizzard::Net::Socket writeSockets[2] = { s_launcherSocket, INVALID_SOCKET };

        if (Blizzard::Net::Select(readSockets, writeSockets, 0)) {
            int32_t error = 0;
            int length = sizeof(error);

            if (getsockopt(s_launcherSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &length) == SOCKET_ERROR) {
                error = Blizzard::Net::TranslateError(0);
            }

            if (error) {
                Blizzard::Net::CloseSocket(&s_launcherSocket, false);
                s_launcherSocket = INVALID_SOCKET;

                if (++s_connectRetries <= 3) {
                    s_notifyState = 1;
                    return;
                }

                s_notifyState = 4;
            } else {
                s_connectRetries = 0;
                s_notifyState = 3;
            }
        } else if (Blizzard::Time::GetTimeElapsed(s_connectStart, static_cast<uint32_t>(Blizzard::Time::Milliseconds())) > 1000) {
            Blizzard::Net::CloseSocket(&s_launcherSocket, false);
            s_launcherSocket = INVALID_SOCKET;
            s_notifyState = 4;
        }

        if (s_notifyState == 4 && s_restartPending && !BGLoading::BGLoadManager::Get()->m_thread) {
            s_restartPending = false;

            if (Blizzard::Streaming::IsStreamingMode()) {
                BGLoading::BGLoadManager::Get()->StartThread();
            }
        }
    }

    if (s_notifyState == 3) {
        Blizzard::String::MemFill(&s_request[s_requestLength], 0, 8);
        Blizzard::String::Copy(&s_request[s_requestLength], paused ? "pause" : "resume", sizeof(s_request));
        Blizzard::String::Append(s_request, " HTTP/1.0\r\n\r\n", sizeof(s_request));

        if (send(s_launcherSocket, s_request, Blizzard::String::Length(s_request), 0) < 0) {
            Blizzard::Net::TranslateError(7001);
        }

        s_lastNotify = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
        s_notifiedPaused = paused;
        Blizzard::Net::CloseSocket(&s_launcherSocket, false);

        if (BGLoading::BGLoadManager::Get()->m_thread) {
            s_restartPending = true;

            if (Blizzard::Streaming::IsStreamingMode()) {
                BGLoading::BGLoadManager::Get()->Shutdown();
            }
        }

        s_notifyState = 0;
    } else if (s_notifyState == 4 && Blizzard::Time::GetTimeElapsed(s_connectStart, static_cast<uint32_t>(Blizzard::Time::Milliseconds())) > 3600000) {
        s_notifyState = 0;
    }
}

// PROVEN: 0adbddc5 IDENTICAL/IDENTICAL
// OFFSET: 0x007A2D80/0x140499910
void BGLoading::UpdateLauncherNotification() {
    NotifyLauncher();
}

#endif
