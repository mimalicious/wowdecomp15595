#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00DC0CA8
static void (*s_timeCallbacks[4])(int32_t hour, int32_t minute);

// OFFSET: 0x004D90C0/0x1401172E0
static void UpdateTime() {
    uint32_t time = g_clientGameTime.GetHourAndMinutes();
    float dayProgression = g_clientGameTime.GameTimeGetDayProgression();
    int32_t days = g_clientGameTime.GetDaysSinceEpoch();
    DayNight::SetTime(time, static_cast<float>(days), dayProgression);
    DayNight::ForceFullUpdate();

    for (uint32_t i = 0; i < sizeof(s_timeCallbacks) / sizeof(s_timeCallbacks[0]); i++) {
        if (s_timeCallbacks[i]) {
            s_timeCallbacks[i](g_clientGameTime.m_hour, g_clientGameTime.m_minute);
        }
    }

    char timeString[128];
    char text[256];
    SStrPrintf(text, sizeof(text), "Time set to %s", WowTime::WowGetTimeString(&g_clientGameTime, timeString, sizeof(timeString)));
    ConsoleWrite(text, DEFAULT_COLOR);
}

// OFFSET: 0x004D9180
static int32_t Packet_SMSG_GAMESPEED_SET(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    float minutesPerSecond;
    msg->Get(minutesPerSecond);

    if (!msg->IsRead()) {
        ConsoleWriteA("Malformed message recieved: Id = %d, Len = %d, Read = %d\n", DEFAULT_COLOR, msgId, msg->m_size, msg->m_read);
        return 0;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Gamespeed set from %.03f to %.03f", g_clientGameTime.GameTimeSetMinutesPerSecond(minutesPerSecond), minutesPerSecond);
    ConsoleWrite(text, DEFAULT_COLOR);
    return 1;
}

// OFFSET: 0x004D9220
static int32_t Packet_SMSG_LOGIN_SETTIMESPEED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t encodedTime;
    float minutesPerSecond;
    uint32_t tzHours;
    msg->Get(encodedTime);
    msg->Get(minutesPerSecond);
    msg->Get(tzHours);

    if (!msg->IsRead()) {
        ConsoleWriteA("Malformed message recieved: Id = %d, Len = %d, Read = %d\n", DEFAULT_COLOR, msgId, msg->m_size, msg->m_read);
        return 0;
    }

    WowTime gameTime;
    WowTime::WowDecodeTime(encodedTime, &gameTime);
    gameTime.m_tzHours = tzHours;
    g_clientGameTime.GameTimeSetTime(gameTime, true);
    UpdateTime();

    char text[256];
    SStrPrintf(text, sizeof(text), "Gamespeed set from %.03f to %.03f", g_clientGameTime.GameTimeSetMinutesPerSecond(minutesPerSecond), minutesPerSecond);
    ConsoleWrite(text, DEFAULT_COLOR);
    return 1;
}

// OFFSET: 0x004D9300
static int32_t Packet_SMSG_GAMETIME_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t encodedTime;
    uint32_t tzHours = 0;
    msg->Get(encodedTime);
    msg->Get(tzHours);

    if (!msg->IsRead()) {
        ConsoleWriteA("Malformed message recieved: Id = %d, Len = %d, Read = %d\n", DEFAULT_COLOR, msgId, msg->m_size, msg->m_read);
        return 0;
    }

    WowTime gameTime;
    WowTime::WowDecodeTime(encodedTime, &gameTime);
    gameTime.m_tzHours = tzHours;
    g_clientGameTime.GameTimeSync(gameTime, false);
    return 1;
}

// OFFSET: 0x004D9390
static int32_t Packet_SMSG_SERVERTIME(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t encodedTime;
    uint32_t lastTick;
    msg->Get(encodedTime);
    msg->Get(lastTick);

    if (!msg->IsRead()) {
        ConsoleWriteA("Malformed message recieved: Id = %d, Len = %d, Read = %d\n", DEFAULT_COLOR, msgId, msg->m_size, msg->m_read);
        return 0;
    }

    WowTime serverTime;
    WowTime::WowDecodeTime(encodedTime, &serverTime);
    serverTime.m_tzHours = 0;

    char timeString[128];
    char text[256];
    SStrPrintf(text, sizeof(text), "Server game time is %s", WowTime::WowGetTimeString(&serverTime, timeString, sizeof(timeString)));
    ConsoleWrite(text, DEFAULT_COLOR);
    SStrPrintf(text, sizeof(text), "Server last tick is 0x%08x %s", lastTick, static_cast<int32_t>(lastTick) < 0 ? "(High bit set)" : "");
    ConsoleWrite(text, DEFAULT_COLOR);
    return 1;
}

// OFFSET: 0x004D9480
static int32_t Packet_SMSG_GAMETIME_SET(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t encodedTime;
    uint32_t tzHours;
    msg->Get(encodedTime);
    msg->Get(tzHours);

    if (!msg->IsRead()) {
        ConsoleWriteA("Malformed message recieved: Id = %d, Len = %d, Read = %d\n", DEFAULT_COLOR, msgId, msg->m_size, msg->m_read);
        return 0;
    }

    WowTime gameTime;
    WowTime::WowDecodeTime(encodedTime, &gameTime);
    gameTime.m_tzHours = tzHours;
    g_clientGameTime.GameTimeSetTime(gameTime, true);
    UpdateTime();
    return 1;
}

// OFFSET: 0x004D9540/0x140117860
void ClientInitializeGameTime() {
    ClientServices::SetMessageHandler(SMSG_GAME_SPEED_SET, Packet_SMSG_GAMESPEED_SET, NULL);
    ClientServices::SetMessageHandler(SMSG_LOGIN_SET_TIME_SPEED, Packet_SMSG_LOGIN_SETTIMESPEED, NULL);
    ClientServices::SetMessageHandler(SMSG_GAME_TIME_UPDATE, Packet_SMSG_GAMETIME_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_SERVERTIME, Packet_SMSG_SERVERTIME, NULL);
    ClientServices::SetMessageHandler(SMSG_GAME_TIME_SET, Packet_SMSG_GAMETIME_SET, NULL);

    for (uint32_t i = 0; i < sizeof(s_timeCallbacks) / sizeof(s_timeCallbacks[0]); i++) {
        s_timeCallbacks[i] = NULL;
    }
}

// OFFSET: 0x004D9510/0x140117830
int32_t ClientGameTimeTickHandler(const void* data, void* param) {
    g_clientGameTime.GameTimeUpdate(*static_cast<const float*>(data));
    return 1;
}

// OFFSET: 0x004D95D0
void ClientDestroyGameTime() {
    ClientServices::ClearMessageHandler(SMSG_GAME_SPEED_SET);
    ClientServices::ClearMessageHandler(SMSG_LOGIN_SET_TIME_SPEED);
    ClientServices::ClearMessageHandler(SMSG_GAME_TIME_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_SERVERTIME);
    ClientServices::ClearMessageHandler(SMSG_GAME_TIME_SET);
    g_clientGameTime.m_timeStamps.Clear();
}
