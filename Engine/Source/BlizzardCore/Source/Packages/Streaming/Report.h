#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_REPORT_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_REPORT_H

#include <cstdint>
#include <string>
#include <vector>

namespace Report {

class LogData {
    public:
    virtual std::string ToString() const = 0;
    virtual std::string Key() const = 0;
    virtual ~LogData();
};

template <class T>
class TrackerData : public LogData {
    public:
    /* 0004 */ std::string m_key;
    /* 0020 */ T m_value;

    TrackerData(std::string key, const T& value);
    std::string ToString() const;
    std::string Key() const;
};

extern bool s_initialized;
extern char s_app[260];
extern char s_version[260];

void DeleteData(std::string key);
std::vector<LogData*>::iterator FindLogData(std::string key);
std::vector<LogData*>& GetLogData();
void Initialize(const std::string& app, const std::string& version, const std::string& language);
bool IsInitialized();
void Reset();
void SendReport();
void SetData(std::string key, int32_t value);
void SetData(std::string key, int64_t value);
void SetData(std::string key, std::string value);
void Shutdown();

}

namespace ReportOld {

extern char s_language[260];

std::string GetDefaultMessage();
void SetLanguage(const char* language);

}

#endif
