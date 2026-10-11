#include "GameTime.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00DC0CB8
CGameTime g_clientGameTime;

// OFFSET: 0x00A313C0/0x1407DB280
float CGameTime::GameTimeGetDayProgression() {
    uint32_t elapsed;

    if (this->m_running) {
        OsGetAsyncTimeMs();
        elapsed = OsGetAsyncTimeMs() - this->m_lastTickMs;
    } else {
        elapsed = 0;
    }

    float minutes = elapsed * 0.001f * this->m_minutesPerSecond + this->m_baseMinuteOfDay;

    while (minutes > 1440.0f) {
        minutes -= 1440.0f;
    }

    return minutes * (1.0f / 1440.0f);
}

// OFFSET: 0x00A31360
float CGameTime::GameTimeSetMinutesPerSecond(float minutesPerSecond) {
    float old = this->m_minutesPerSecond;

    if (minutesPerSecond < 1.0f / 60.0f) {
        this->m_minutesPerSecond = 1.0f / 60.0f;
    } else if (minutesPerSecond > 60.0f) {
        this->m_minutesPerSecond = 60.0f;
    } else {
        this->m_minutesPerSecond = minutesPerSecond;
    }

    return old;
}

// OFFSET: 0x00A32060
CGameTime::CGameTime() {
    this->m_minutesPerSecond = 1.0f / 60.0f;
    this->m_fracMinutes = 0.0f;
    this->unk_0020 = 0;
    this->m_baseMinuteOfDay = 0.0f;
    this->m_minuteOffset = 0;
    this->unk_0048 = 0.0f;
    this->m_dayOffset = 0;
    this->unk_004C = 0.0f;
    this->m_minuteTicks = 0;
    this->m_elapsedMinutes = 0;
    this->m_lastTickMs = 0;
    this->m_running = 0;
}

// OFFSET: 0x005D78E0/0x140252BB0
bool CGameTime::IsDaytime() {
    CGameTime time;
    time.SetHourAndMinutes(this->GetHourAndMinutes());
    time.SetDate(0, 0, 0);

    uint32_t value;
    WowTime end;
    WowTime::WowEncodeTime(value, 0, 21, -1, -1, -1, -1, 0);
    WowTime::WowDecodeTime(value, &end);
    end.m_tzHours = 0;

    WowTime start;
    WowTime::WowEncodeTime(value, 30, 5, -1, -1, -1, -1, 0);
    WowTime::WowDecodeTime(value, &start);
    start.m_tzHours = 0;

    return time.InRange(start, end);
}

// OFFSET: 0x00A31440
void CGameTime::UpdateTimeStamps(int32_t minutes) {
    HASHKEY_NONE key;
    TIMESTAMPSTRUCT* stamp = this->m_timeStamps.Ptr(minutes, key);

    if (!stamp) {
        return;
    }

    TIMESTAMPCALLBACK* callback = stamp->m_callbacks.Head();

    while (callback) {
        TIMESTAMPCALLBACK* next = stamp->m_callbacks.Next(callback);
        callback->m_callback(this, callback->m_param);
        callback = next;
    }
}

// OFFSET: 0x00A31610
void CGameTime::TickMinute() {
    int32_t minutes = (this->GetHourAndMinutes() + 1) % 1440;
    this->SetHourAndMinutes(minutes);

    if (!minutes) {
        this->AddDays(1, false);
    }

    this->m_minuteTicks++;
    this->UpdateTimeStamps(minutes);
    this->m_lastTickMs = OsGetAsyncTimeMs();
    this->m_running = 1;
    this->m_baseMinuteOfDay = minutes + this->m_fracMinutes;
}

// OFFSET: 0x00A31680
void CGameTime::GameTimeSetTime(const WowTime& time, bool tick) {
    WowTime newTime = time;

    if (this->m_minuteOffset) {
        int32_t minutes = newTime.GetHourAndMinutes() + this->m_minuteOffset;
        newTime.SetHourAndMinutes(minutes < 0 ? minutes + 1440 : minutes % 1440);
    }

    if (this->m_dayOffset) {
        newTime.AddDays(this->m_dayOffset, false);
    }

    newTime.m_tzHours = time.m_tzHours;
    *static_cast<WowTime*>(this) = newTime;

    if (!tick) {
        return;
    }

    if (this->m_minute) {
        this->m_minute--;
    } else {
        this->m_minute = 59;

        if (this->m_hour) {
            this->m_hour--;
        } else {
            this->m_hour = 23;
            this->AddDays(-1, false);
        }
    }

    this->TickMinute();
}

// OFFSET: 0x00A31770/0x1407DB740
void CGameTime::GameTimeUpdate(float elapsed) {
    this->m_fracMinutes = this->m_minutesPerSecond * elapsed + this->m_fracMinutes;

    if (this->m_elapsedMinutes && this->m_fracMinutes >= 1.0f) {
        uint32_t minutes = static_cast<uint32_t>(this->m_fracMinutes);

        if (this->m_elapsedMinutes < minutes) {
            minutes = this->m_elapsedMinutes;
        }

        this->m_elapsedMinutes -= minutes;
        this->m_fracMinutes -= minutes;
    }

    while (this->m_fracMinutes >= 1.0f) {
        this->m_fracMinutes -= 1.0f;
        this->TickMinute();
    }
}

// OFFSET: 0x00A318F0/0x1407DB840
void CGameTime::GameTimeSync(const WowTime& time, bool force) {
    WowTime newTime = time;

    if (this->m_minuteOffset) {
        int32_t minutes = newTime.GetHourAndMinutes() + this->m_minuteOffset;
        newTime.SetHourAndMinutes(minutes < 0 ? minutes + 1440 : minutes % 1440);
    }

    if (this->m_dayOffset) {
        newTime.AddDays(this->m_dayOffset, false);
    }

    int32_t days = newTime.GetDaysSinceEpoch() - this->GetDaysSinceEpoch();
    int32_t delta = (1440 * days - this->GetHourAndMinutes() + newTime.GetHourAndMinutes()) % 1440;
    int32_t behind;

    if (force || delta > 0) {
        if (delta > 1) {
            char newString[1024];
            char currentString[1024];
            const char* newText = WowTime::WowGetTimeString(&newTime, newString, sizeof(newString));
            const char* currentText = WowTime::WowGetTimeString(this, currentString, sizeof(currentString));
            DebugLog(0x10, "GameTimeSync: skipping forwards %d game minutes, (current = %s, newtime = %s)", delta, currentText, newText);
        }

        for (; delta > 0; delta--) {
            this->TickMinute();
        }

        behind = 0;
    } else {
        behind = -delta;
    }

    newTime.m_tzHours = time.m_tzHours;
    *static_cast<WowTime*>(this) = newTime;

    if (!behind) {
        return;
    }

    this->m_elapsedMinutes += behind;

    for (int32_t i = behind; i > 0; i--) {
        this->TickMinute();
    }

    DebugLog(0x10, "GameTimeSync: delta=%d, differential=%d, HoursAndMinutes=%d", 0, this->m_elapsedMinutes, this->GetHourAndMinutes());
}

// OFFSET: 0x00A31C20/0x1407DBB30
void __stdcall CGameTime::GameTimeUnregisterCallback(TIMESTAMPCALLBACK* callback) {
    callback->m_link.Unlink();
    delete callback;
}

// OFFSET: 0x00A321A0
TIMESTAMPCALLBACK* CGameTime::GameTimeRegisterCallback(const WowTime* time, void (__stdcall* callback)(CGameTime* time, void* param), void* param) {
    if (!callback) {
        DebugValidate("callback");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (time->m_hour < 0 || time->m_minute < 0) {
        return NULL;
    }

    int32_t minutes = time->GetHourAndMinutes();
    HASHKEY_NONE key;
    TIMESTAMPSTRUCT* stamp = this->m_timeStamps.Ptr(minutes, key);

    if (!stamp) {
        stamp = this->m_timeStamps.New(minutes, key, 0, 0);
    }

    TIMESTAMPCALLBACK* node = stamp->m_callbacks.NewNode(2, 0, 0);
    node->m_param = param;
    node->m_callback = callback;
    return node;
}

// OFFSET: #0x004C9730
TIMESTAMPSTRUCT::~TIMESTAMPSTRUCT() {
    this->m_callbacks.Clear();
}
