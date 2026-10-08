#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_MANIFESTS_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_MANIFESTS_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/URL/URL.h"
#include "Engine/Source/Storm/H/Types.h"

namespace Blizzard {
namespace Streaming {
namespace Manifests {

class ManifestItem {
    public:
    /* 0004 */ std::string m_name;

    virtual bool HandleTag(const char* tag, const char* value);
    virtual ~ManifestItem();
};

class TransportItem : public ManifestItem {
    public:
    /* 0020 */ int32_t m_md5Size;
    /* 0024 */ int32_t m_splitSize;

    bool HandleTag(const char* tag, const char* value) override;
};

struct TransportInfo {
    /* 0000 */ TransportItem* m_item;
    /* 0004 */ std::string m_uncPath;
    /* 0020 */ int32_t m_maxRetry;
};

class TimeslotItem : public ManifestItem {
    public:
    static Blizzard::Time::Timestamp ParseTimestamp(const char* time);

    /* 0020 */ Blizzard::Time::Timestamp m_startTime;
    /* 0028 */ Blizzard::Time::Timestamp m_endTime;
    /* 0030 */ int32_t m_usage;

    bool HandleTag(const char* tag, const char* value) override;
};

class ServerPathItem : public ManifestItem {
    public:
    /* 0020 */ std::string m_path;

    bool HandleTag(const char* tag, const char* value) override;
};

class ParameterItem : public ManifestItem {
    public:
    /* 0020 */ std::string m_value;

    bool HandleTag(const char* tag, const char* value) override;
    ~ParameterItem();

    ParameterItem();
};

class ServerItem : public ManifestItem {
    public:
    struct UsageSlot {
        /* 0000 */ Blizzard::Time::Timestamp m_time;
        /* 0008 */ int32_t m_usage;
    };

    /* 0020 */ std::string m_location;
    /* 003C */ std::vector<std::string> m_timeslotIds;
    /* 004C */ std::vector<UsageSlot> m_usageSlots;
    /* 005C */ int32_t m_uses;
    /* 0060 */ int32_t m_canDeactivate;
    /* 0064 */ int32_t m_maxRetry;
    /* 0068 */ bool m_active;

    bool HandleTag(const char* tag, const char* value) override;
    ~ServerItem();

    ServerItem();
    int32_t GetUsage(Blizzard::Time::Timestamp time);
    void InsertTimeslot(TimeslotItem* timeslot);
    void InsertUsageSlot(UsageSlot& slot, Blizzard::Time::Timestamp end);
};

class FileItem : public ManifestItem {
    public:
    /* 0020 */ std::string m_fileName;
    /* 003C */ std::string m_path;
    /* 0058 */ std::string m_localPath;
    /* 0074 */ std::string unk_0074;
    /* 0090 */ std::string m_transportItem;
    /* 00B0 */ int64_t m_size;
    /* 00B8 */ TransportItem* m_transport;
    /* 00BC */ int32_t m_flags;
    /* 00C0 */ std::string m_fileVersion;
    /* 00DC */ std::vector<std::string> m_validServers;

    bool HandleTag(const char* tag, const char* value) override;
    ~FileItem();

    FileItem();
    int32_t GetContentId();
    std::string GetRemotePath();
};

}

class ManifestRandomNumberGenerator {
    public:
    virtual ~ManifestRandomNumberGenerator();
    virtual int32_t RandomGenerator(int32_t range);
};

class ManifestInfo {
    public:
    static ManifestInfo* s_instance;

    static ManifestInfo* Get();
    static bool STORMAPI GetItemTag(char* line, char** value, char** tag, int32_t* length);
    static std::string GetAbsolutePath(const std::string& path);
    static std::string GetBandwidthDataString();
    static bool ValidateMD5(const char* url, const void* data, uint32_t size);
    static void Destroy();
    static bool Initialize(const char* path);
    static void DeactivateServer(const std::string& url, const std::string& name);
    static std::string GetParameter(const char* name);
    static std::string GetRemotePath(const std::string& name);
    static int32_t GetServerItemMaxRetry(const std::string& url, const std::string& name);
    static Manifests::TransportItem* GetTransportItem(const std::string& name);
    static std::vector<std::string> GetFileList(const char* path);
    static void SetFileItem(std::string name, const Manifests::FileItem& item);
    static void SetParameter(const char* name, int32_t value);
    static void SetParameter(const char* name, const char* value);
    static void SetServerItem(std::string name, const Manifests::ServerItem& item);
    static void SetServerPathItem(std::string name, std::string path);
    static void SetTransportItem(std::string name, const Manifests::TransportItem& item);

    /* 0000 */ int32_t m_version;
    /* 0004 */ std::map<std::string, Manifests::ServerItem> m_servers;
    /* 0024 */ std::map<std::string, Manifests::TimeslotItem> m_timeslots;
    /* 0044 */ std::map<std::string, Manifests::ServerPathItem> m_serverPaths;
    /* 0064 */ std::map<std::string, Manifests::ParameterItem> m_parameters;
    /* 0084 */ std::map<std::string, Manifests::TransportItem> m_transports;
    /* 00A4 */ std::map<std::string, Manifests::FileItem> m_files;
    /* 00C4 */ ManifestRandomNumberGenerator* m_randomGenerator;

    ManifestInfo();
    ~ManifestInfo();
    bool FinalizeManifestItems();
    Manifests::FileItem* GetFileItem(const std::string& name);
    std::string GetRandomServerLocation(const std::vector<std::string>& validServers);
    Manifests::ManifestItem* HandleBaseTag(const char* tag, const char* value);
    bool ReadFile(const char* path);
    bool ReadDirectFile(const char* path);
    bool ReadDirectFileFromBuffer(char* buffer, int32_t size);
    bool ReadIndirectFile(const char* url);
};

class FileManifest {
    public:
    struct MANIFEST_ITEM {
        /* 0000 */ std::string m_path;
        /* 001C */ std::string m_absolutePath;
        /* 0038 */ std::string m_fileVersion;
        /* 0058 */ int64_t m_size;
        /* 0060 */ int32_t m_flags;
    };

    static int32_t GetBGPreloadSleep();
    static std::vector<std::string> GetFileList(const char* path);
    static bool GetIsStreamingTrial();
    static bool ReadDirectFile(const char* path);
    static bool ReadDirectFileFromBuffer(char* buffer, int32_t size);
    static bool ReadIndirectFile(const char* url);
    static void SetDefaultBgPreloadSleep(int32_t sleep);
    static bool SetSpeedTest(int32_t speedTest);
    static bool SplitItem(char* line, MANIFEST_ITEM* item);
    static bool SplitServerItem(char* line, std::string& base, std::string& path);
};

class SourceManifest {
    public:
    struct MANIFEST_ITEM {
        /* 0000 */ std::string m_name;
        /* 001C */ std::string m_path;
    };

    static bool Initialize(const char* path);
    static bool ReadDirectFile(const char* path);
    static bool ReadDirectFileFromBuffer(char* buffer, int32_t size);
    static bool ReadIndirectFile(const char* url);
    static bool SplitItem(char* line, MANIFEST_ITEM* item);
};

class AddLatency {
    public:
    static int64_t accumulatorNs;

    /* 0000 */ Blizzard::Time::Timestamp m_start;
    /* 0008 */ int64_t m_bytes;

    ~AddLatency();
};

class Transport {
    public:
    struct URL_INFO {
        /* 0000 */ std::string m_url;
        /* 0020 */ int64_t m_start;
        /* 0028 */ int64_t m_end;
        /* 0030 */ int32_t m_size;

        URL_INFO(const char* url, int64_t start, int64_t end, int32_t size);
    };

    struct TRANSPORT_ITEM {
        /* 0000 */ std::string m_name;
        /* 001C */ int32_t m_md5Size;
        /* 0020 */ int32_t m_splitSize;
        /* 0024 */ std::vector<std::string> m_servers;
    };

    struct Config {
        /* 0000 */ uint32_t m_timeout;
        /* 0004 */ bool m_shouldBlock;
        /* 0008 */ std::string m_cookieName;
        /* 0024 */ std::string m_cookieValue;
    };

    class TransportHttpCallback : public Blizzard::URL::DownloadCallback {
        public:
        /* 0004 */ std::string* m_data;
        /* 0008 */ std::string m_cookieName;
        /* 0024 */ std::string m_cookieValue;
        /* 0040 */ int32_t m_received;
        /* 0048 */ int64_t m_rangeStart;
        /* 0050 */ int64_t m_rangeEnd;
        /* 0058 */ uint32_t m_timeout;
        /* 005C */ Blizzard::Lock::Event m_event;
        /* 0060 */ bool m_stop;
        /* 0064 */ Blizzard::URL::DownloadCallback::Status m_status;
        /* 0068 */ bool m_success;

        TransportHttpCallback(int32_t size, std::string& data);
        ~TransportHttpCallback();
        int32_t GetTimeout() override;
        Blizzard::URL::DownloadCallback::ByteRange GetByteRange() override;
        bool Stop() override;
        bool ShouldBlock() override;
        void WaitForCompletion() override;
        Blizzard::URL::DownloadCallback::Cookie GetCookie() override;
        bool ReceivedData(const void* data, int32_t size) override;
        void Completed(Blizzard::URL::DownloadCallback::Status status) override;
        virtual void SetByteRange(int64_t start, int64_t end);
        virtual void SetCookie(const char* name, const char* value);
        virtual void SetTimeOutValue(uint32_t timeout);
    };

    static Config* GetConfig();
    static int32_t AdjustRange(int64_t* start, int64_t* end, int32_t md5Size, int64_t fileSize);
    static void GetSplitFileURL(const std::string& url, int64_t start, int64_t end, int32_t skip, int32_t splitSize, int32_t md5Size, int32_t size, std::vector<URL_INFO>* urls);
    static bool MD5_Data(std::string* data, int32_t md5Size);
    static bool Read(const Manifests::TransportInfo& info, void* data, int64_t fileSize, int64_t offset, int32_t size);
    static bool ReadDirectFile(const char* path);
    static bool ReadDirectFileFromBuffer(char* buffer, int32_t size);
    static bool ReadFile(const Manifests::TransportInfo& info, void* data, int64_t fileSize, int64_t offset, int32_t size);
    static bool ReadHttp(const Manifests::TransportInfo& info, void* data, int64_t fileSize, int64_t offset, int32_t size);
    static bool ReadIndirectFile(const char* url);
    static bool SplitItem(char* line, TRANSPORT_ITEM* item);
};

typedef void (*PROCESSURLFUNC)(const char* url);

extern const char* s_httpPrefix;
extern PROCESSURLFUNC s_processUrlFunc;
extern const char* s_filePrefix;

bool SplitLine(char* line, char** value, char** tag, int32_t* length);
char* STORMAPI TrimLine(char* line);

}
}

namespace System_Streaming {

class HttpCallback : public Blizzard::URL::DownloadCallback {
    public:
    /* 0004 */ std::string m_data;
    /* 0020 */ uint32_t m_received;
    /* 0024 */ Blizzard::URL::DownloadCallback::Status m_status;
    /* 0028 */ uint32_t m_timeout;
    /* 002C */ void* m_event;

    ~HttpCallback();
    int32_t GetTimeout() override;
    bool ShouldBlock() override;
    void WaitForCompletion() override;
    bool ReceivedData(const void* data, int32_t size) override;
    void Completed(Blizzard::URL::DownloadCallback::Status status) override;
    virtual void SetTimeOutValue(uint32_t timeout);

    HttpCallback();
};

}

#endif
