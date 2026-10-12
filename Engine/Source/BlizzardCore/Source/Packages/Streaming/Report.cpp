#include "Report.h"
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Tracker.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// STATIC: 0x00D074F4/0x140BB80D8
static const char* s_trackerAddressUS = "http://us.tracker.worldofwarcraft.com:3724/announce";

// STATIC: 0x00D074F8/0x140BB80E0
static const char* s_trackerAddressEU = "http://eu.tracker.worldofwarcraft.com:3724/announce";

// STATIC: 0x00D074FC/0x140BB80E8
static const char* s_trackerAddressKR = "http://kr.tracker.worldofwarcraft.com:3724/announce";

// STATIC: 0x00D07500/0x140BB80F0
static const char* s_trackerAddressCN = "http://cn.tracker.worldofwarcraft.com:3724/announce";

// STATIC: 0x00D07504/0x140BB80F8
static const char* s_trackerAddressTW = "http://tw.tracker.worldofwarcraft.com:3724/announce";

// STATIC: 0x00EBAD89/0x140DABAB1
bool Report::s_initialized;

// STATIC: 0x00EBAD90
char Report::s_app[260];

// STATIC: 0x00EBAE98
char Report::s_version[260];

// STATIC: 0x00EBAFA0/0x140DABCE0
char ReportOld::s_language[260];

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

// OFFSET: 0x007FCF60/0x14050FBB0
Report::LogData::~LogData() {
}

// OFFSET: 0x007FCF90/0x14050FBE0
bool Report::IsInitialized() {
    return s_initialized;
}

// OFFSET: 0x007FCFA0/0x14050FBF0
void Report::Initialize(const std::string& app, const std::string& version, const std::string& language) {
    Blizzard::String::Copy(s_app, app.c_str(), sizeof(s_app));
    Blizzard::String::Copy(s_version, version.c_str(), sizeof(s_version));
    ReportOld::SetLanguage(language.c_str());
    s_initialized = true;
}

// OFFSET: [0x007FD180/0x14050FDB0<int32_t>, 0x007FD290/0x14050FE90<int64_t>, 0x007FD310/0x14050FF90<std::string>]
template <class T>
Report::TrackerData<T>::TrackerData(std::string key, const T& value) {
    this->m_key = key;
    this->m_value = value;
}

// OFFSET: [0x007FD3B0<std::string>, 0x007FD400<std::string>, 0x007FD200/0x14050FF30<int32_t>]
template <class T>
Report::TrackerData<T>::~TrackerData() {
}

// OFFSET: 0x007FD250/0x14050FE50
template <class T>
std::string Report::TrackerData<T>::Key() const {
    return this->m_key;
}

// OFFSET: 0x007FDB80/0x1405109F0
std::string ReportOld::GetDefaultMessage() {
    std::string message;
    message.append("&stats=", 7);
    message += "app=" + std::string(Report::s_app);
    message += ",ver=" + std::string(Report::s_version);
    message += ",lang=" + std::string(s_language);

    return message;
}

// OFFSET: 0x007FDD90/0x140510C40
template <>
std::string Report::TrackerData<int32_t>::ToString() const {
    char buffer[256];
    Blizzard::String::Format(buffer, sizeof(buffer), "%s=%d", this->m_key.c_str(), this->m_value);

    return std::string(buffer);
}

// OFFSET: 0x007FDE20/0x140510CD0
template <>
std::string Report::TrackerData<int64_t>::ToString() const {
    char buffer[256];
    Blizzard::String::Format(buffer, sizeof(buffer), "%s=%lld", this->m_key.c_str(), this->m_value);

    return std::string(buffer);
}

// OFFSET: 0x007FDEB0/0x140510D60
template <>
std::string Report::TrackerData<std::string>::ToString() const {
    char buffer[256];
    Blizzard::String::Format(buffer, sizeof(buffer), "%s=%s", this->m_key.c_str(), this->m_value.c_str());

    return std::string(buffer);
}

// OFFSET: 0x007FDF50/0x140510E00
std::vector<Report::LogData*>& Report::GetLogData() {
    static std::vector<LogData*> s_logData;
    return s_logData;
}

// OFFSET: 0x007FDFB0/0x140510E80
std::vector<Report::LogData*>::iterator Report::FindLogData(std::string key) {
    for (std::vector<LogData*>::iterator it = GetLogData().begin(); it != GetLogData().end(); ++it) {
        if ((*it)->Key() == key) {
            return it;
        }
    }

    return GetLogData().end();
}

// OFFSET: 0x007FE200/0x140511130
void Report::Reset() {
    for (std::vector<LogData*>::iterator it = GetLogData().begin(); it != GetLogData().end(); ++it) {
        delete *it;
    }

    GetLogData().erase(GetLogData().begin(), GetLogData().end());
}

// OFFSET: 0x007FE3C0/0x1405112E0
void Report::Shutdown() {
    Reset();
    s_initialized = false;
}

// OFFSET: 0x007FE3D0/0x140511300
void Report::DeleteData(std::string key) {
    std::vector<LogData*>::iterator it = FindLogData(key);

    if (it != GetLogData().end()) {
        delete *it;
        GetLogData().erase(it);
    }
}

// OFFSET: 0x007FE4B0/0x140511440
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

// OFFSET: 0x007FE6D0/0x1405116C0
void Report::SetData(std::string key, int32_t value) {
    DeleteData(key);
    GetLogData().push_back(new TrackerData<int32_t>(key, value));
}

// OFFSET: 0x007FE7A0/0x1405118B0
void Report::SetData(std::string key, int64_t value) {
    DeleteData(key);
    GetLogData().push_back(new TrackerData<int64_t>(key, value));
}

// OFFSET: 0x007FE870/0x140511AA0
void Report::SetData(std::string key, std::string value) {
    DeleteData(key);
    GetLogData().push_back(new TrackerData<std::string>(key, value));
}
