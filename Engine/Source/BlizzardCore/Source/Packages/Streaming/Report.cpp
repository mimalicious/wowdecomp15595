#include "Report.h"
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Tracker.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// STATIC: %0x00C0C844
static const char* s_trackerAddressUS = "http://us.tracker.worldofwarcraft.com:3724/announce";

// STATIC: %0x00C0C848
static const char* s_trackerAddressEU = "http://eu.tracker.worldofwarcraft.com:3724/announce";

// STATIC: %0x00C0C84C
static const char* s_trackerAddressKR = "http://kr.tracker.worldofwarcraft.com:3724/announce";

// STATIC: %0x00C0C850
static const char* s_trackerAddressCN = "http://cn.tracker.worldofwarcraft.com:3724/announce";

// STATIC: %0x00C0C854
static const char* s_trackerAddressTW = "http://tw.tracker.worldofwarcraft.com:3724/announce";

// STATIC: %0x00D87629
bool Report::s_initialized;

// STATIC: %0x00D87630
char Report::s_app[260];

// STATIC: %0x00D87738
char Report::s_version[260];

// STATIC: %0x00D87840
char ReportOld::s_language[260];

// PROVEN: f8b41de0 IDENTICAL/IDENTICAL
// OFFSET: 0x007FCE10/0x14050FA30
void ReportOld::SetLanguage(const char* language) {
    Blizzard::String::Copy(s_language, language, sizeof(s_language));

    if (Tracker::trackerAddress) {
        return;
    }

    if (Blizzard::String::EqualI(language, "enUS") || Blizzard::String::EqualI(language, "esMX")) {
        Tracker::trackerAddress = s_trackerAddressUS;
    } else if (Blizzard::String::EqualI(language, "deDE") || Blizzard::String::EqualI(language, "enGB") || Blizzard::String::EqualI(language, "esES") || Blizzard::String::EqualI(language, "frFR") || Blizzard::String::EqualI(language, "ruRU")) {
        Tracker::trackerAddress = s_trackerAddressEU;
    } else if (Blizzard::String::EqualI(language, "koKR")) {
        Tracker::trackerAddress = s_trackerAddressKR;
    } else if (Blizzard::String::EqualI(language, "zhCN")) {
        Tracker::trackerAddress = s_trackerAddressCN;
    } else if (Blizzard::String::EqualI(language, "zhTW")) {
        Tracker::trackerAddress = s_trackerAddressTW;
    }
}

// OFFSET: %0x00762CB0
Report::LogData::~LogData() {
}

// PROVEN: a6bbf819 IDENTICAL/IDENTICAL
// OFFSET: 0x007FCF90/0x14050FBE0
bool Report::IsInitialized() {
    return s_initialized;
}

// PROVEN: d5967bce IDENTICAL/NONE
// OFFSET: 0x007FCFA0/0x14050FBF0
void Report::Initialize(const std::string& app, const std::string& version, const std::string& language) {
    Blizzard::String::Copy(s_app, app.c_str(), sizeof(s_app));
    Blizzard::String::Copy(s_version, version.c_str(), sizeof(s_version));
    ReportOld::SetLanguage(language.c_str());
    s_initialized = true;
}

// OFFSET: [%0x00762EC0<int32_t>, %0x00762F80<int64_t>, %0x00763050<std::string>]
template <class T>
Report::TrackerData<T>::TrackerData(std::string key, const T& value) {
    this->m_key = key;
    this->m_value = value;
}

// OFFSET: %0x00762F40
template <class T>
std::string Report::TrackerData<T>::Key() const {
    return this->m_key;
}

// OFFSET: %0x007638D0
std::string ReportOld::GetDefaultMessage() {
    std::string message;
    message.append("&stats=", 7);
    message += "app=" + std::string(Report::s_app);
    message += ",ver=" + std::string(Report::s_version);
    message += ",lang=" + std::string(s_language);

    return message;
}

// OFFSET: %0x00763AE0
template <>
std::string Report::TrackerData<int32_t>::ToString() const {
    char buffer[256];
    Blizzard::String::Format(buffer, sizeof(buffer), "%s=%d", this->m_key.c_str(), this->m_value);

    return std::string(buffer);
}

// OFFSET: %0x00763B70
template <>
std::string Report::TrackerData<int64_t>::ToString() const {
    char buffer[256];
    Blizzard::String::Format(buffer, sizeof(buffer), "%s=%lld", this->m_key.c_str(), this->m_value);

    return std::string(buffer);
}

// OFFSET: %0x00763C00
template <>
std::string Report::TrackerData<std::string>::ToString() const {
    char buffer[256];
    Blizzard::String::Format(buffer, sizeof(buffer), "%s=%s", this->m_key.c_str(), this->m_value.c_str());

    return std::string(buffer);
}

// OFFSET: %0x00763CA0
std::vector<Report::LogData*>& Report::GetLogData() {
    static std::vector<LogData*> s_logData;
    return s_logData;
}

// OFFSET: %0x00763D00
std::vector<Report::LogData*>::iterator Report::FindLogData(std::string key) {
    for (std::vector<LogData*>::iterator it = GetLogData().begin(); it != GetLogData().end(); ++it) {
        if ((*it)->Key() == key) {
            return it;
        }
    }

    return GetLogData().end();
}

// OFFSET: %0x00763F70
void Report::Reset() {
    for (std::vector<LogData*>::iterator it = GetLogData().begin(); it != GetLogData().end(); ++it) {
        delete *it;
    }

    GetLogData().erase(GetLogData().begin(), GetLogData().end());
}

// PROVEN: 7c7b2d1b IDENTICAL/NONE
// OFFSET: 0x007FE3C0
void Report::Shutdown() {
    Reset();
    s_initialized = false;
}

// OFFSET: %0x00764140
void Report::DeleteData(std::string key) {
    std::vector<LogData*>::iterator it = FindLogData(key);

    if (it != GetLogData().end()) {
        delete *it;
        GetLogData().erase(it);
    }
}

// OFFSET: %0x00764220
void Report::SendReport() {
    if (!s_initialized) {
        return;
    }

    std::string message = ReportOld::GetDefaultMessage();

    for (std::vector<LogData*>::iterator it = GetLogData().begin(); it != GetLogData().end(); ++it) {
        if (it != GetLogData().end()) {
            message.append(",", 1);
        }

        message += (*it)->ToString();
    }

    Tracker::SendToTracker(message.c_str());
}

// OFFSET: %0x00764440
void Report::SetData(std::string key, int32_t value) {
    DeleteData(key);
    GetLogData().push_back(new TrackerData<int32_t>(key, value));
}

// OFFSET: %0x00764510
void Report::SetData(std::string key, int64_t value) {
    DeleteData(key);
    GetLogData().push_back(new TrackerData<int64_t>(key, value));
}

// OFFSET: %0x007645E0
void Report::SetData(std::string key, std::string value) {
    DeleteData(key);
    GetLogData().push_back(new TrackerData<std::string>(key, value));
}
