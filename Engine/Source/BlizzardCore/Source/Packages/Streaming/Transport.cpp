#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack_Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/BlizzardCore/Source/System/URL/URL.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

static Blizzard::Using::Use s_useFile("Blizzard::File");

static double s_NsPerByte = 0.0085;

static int64_t s_ReadAccessTimeNs;

int64_t Blizzard::Streaming::AddLatency::accumulatorNs;

int32_t System_File::Stacked::count_retry;

static Blizzard::Thread::LocalObject<std::string> s_transportBuffer;

// PROVEN: c7bea0fc IDENTICAL/IDENTICAL
// OFFSET: 0x007ABA80/0x1404A1AD0
int32_t Blizzard::Streaming::Transport::TransportHttpCallback::GetTimeout() {
    return this->m_timeout;
}

// PROVEN: 02a25635 IDENTICAL/NONE
// OFFSET: 0x007ABA90
void Blizzard::Streaming::Transport::TransportHttpCallback::SetByteRange(int64_t start, int64_t end) {
    this->m_rangeStart = start;
    this->m_rangeEnd = end;
}

// PROVEN: d4f1202e IDENTICAL/IDENTICAL
// OFFSET: 0x007ABAB0/0x1404A1AE0
void Blizzard::Streaming::Transport::TransportHttpCallback::SetTimeOutValue(uint32_t timeout) {
    this->m_timeout = timeout;
}

// OFFSET: 0x007ABAC0
int32_t Blizzard::Streaming::Transport::AdjustRange(int64_t* start, int64_t* end, int32_t md5Size, int64_t fileSize) {
    int64_t alignedStart = *start - *start % md5Size;
    int64_t alignedEnd = md5Size + *end - (md5Size + *end) % md5Size - 1;
    int64_t firstChunk = alignedStart / md5Size;
    int64_t lastChunk = (alignedEnd + 1) / md5Size - 1;

    if (alignedEnd > fileSize) {
        alignedEnd = fileSize - 1;
    }

    int32_t skip = static_cast<int32_t>(32 * firstChunk);
    int64_t original = *start;
    *start = skip + alignedStart;
    *end = alignedEnd + 32 * (lastChunk + 1);

    return static_cast<int32_t>(skip + original - *start);
}

// OFFSET: 0x007B37B0
void Blizzard::Streaming::Transport::TransportHttpCallback::Completed(Blizzard::URL::DownloadCallback::Status status) {
    this->m_status = status;

    switch (status) {
        case Blizzard::URL::DownloadCallback::DOWNLOAD_SUCCESS:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_STOPPED:
            this->m_success = true;
            break;

        case Blizzard::URL::DownloadCallback::DOWNLOAD_FAILED:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_TIMEOUT:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_NOT_FOUND:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_NOT_MODIFIED:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_ABORTED:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_FORBIDDEN:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_NAME_NOT_RESOLVED:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_CANNOT_CONNECT:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_BAD_REQUEST:
        case Blizzard::URL::DownloadCallback::DOWNLOAD_UNKNOWN_ERROR:
            this->m_success = false;
            break;
    }

    Blizzard::Lock::EventSet(this->m_event);
}

// OFFSET: 0x007BB2C0
bool Blizzard::Streaming::Transport::MD5_Data(std::string* data, int32_t md5Size) {
    bool result = false;
    char* out = const_cast<char*>(data->c_str());
    uint32_t total = data->size();
    uint32_t position = 0;
    uint32_t consumed = 0;

    if (!total) {
        return result;
    }

    for (;;) {
        uint32_t length = md5Size;
        uint32_t next = position + md5Size + 32;

        if (next > total) {
            length = total - position - 32;
        }

        const char* chunk = data->c_str() + position;
        consumed += length + 32;

        char hex[33];
        memmove(hex, data->c_str() + position + length, 32);
        hex[32] = '\0';

        Blizzard::Util::MD5 md5;
        md5.Process(chunk, length);

        Blizzard::Util::Digest<16> actual;
        md5.Finish(actual.m_data);

        if (out != chunk) {
            memmove(out, chunk, length);
        }

        out += length;

        Blizzard::Util::Digest<16> expected;
        expected.FromHexString(hex);

        result = actual == expected;

        if (!result) {
            break;
        }

        total = data->size();
        position = next;

        if (consumed >= total) {
            break;
        }
    }

    return result;
}

// OFFSET: 0x007BB420
bool Blizzard::Streaming::Transport::ReadFile(const Manifests::TransportInfo& info, void* data, int64_t fileSize, int64_t offset, int32_t size) {
    Blizzard::File::StreamRecord* file;

    if (!Blizzard::File::Open(info.m_uncPath.c_str() + 7, 4101, file)) {
        return false;
    }

    int32_t count = size;

    if (Blizzard::File::Read(file, data, offset, &count, 0) && count == size) {
        Blizzard::File::Close(file);
        return true;
    }

    Blizzard::File::Close(file);
    return false;
}

// OFFSET: [0x007C3610, 0x007C6000]
Blizzard::Streaming::Transport::TransportHttpCallback::~TransportHttpCallback() {
    Blizzard::Lock::EventDestroy(this->m_event);
}

// PROVEN: 9e670bcf IDENTICAL/IDENTICAL
// OFFSET: 0x007C3680/0x1404BDD00
Blizzard::URL::DownloadCallback::ByteRange Blizzard::Streaming::Transport::TransportHttpCallback::GetByteRange() {
    ByteRange range;
    range.m_start = this->m_rangeStart;
    range.m_end = this->m_rangeEnd;
    return range;
}

// PROVEN: a221b672 IDENTICAL/IDENTICAL
// OFFSET: 0x007C36B0/0x1404BDD30
bool Blizzard::Streaming::Transport::TransportHttpCallback::Stop() {
    return this->m_stop;
}

// OFFSET: 0x007C36C0
void Blizzard::Streaming::Transport::TransportHttpCallback::WaitForCompletion() {
    Blizzard::Lock::EventWaitForever(this->m_event);
}

// OFFSET: 0x007C36D0
Blizzard::URL::DownloadCallback::Cookie Blizzard::Streaming::Transport::TransportHttpCallback::GetCookie() {
    Cookie cookie;
    cookie.m_name = this->m_cookieName.c_str();
    cookie.m_value = this->m_cookieValue.c_str();
    return cookie;
}

// INLINED
Blizzard::Streaming::AddLatency::~AddLatency() {
    int64_t delta = s_ReadAccessTimeNs + this->m_start + static_cast<int64_t>(this->m_bytes * s_NsPerByte) - Blizzard::Time::GetTimestamp();
    accumulatorNs += delta;

    if (accumulatorNs < 0 || accumulatorNs <= 1000000) {
        return;
    }

    do {
        Blizzard::Time::Timestamp start = Blizzard::Time::GetTimestamp();
        int32_t milliseconds = static_cast<int32_t>(accumulatorNs / 1000000);
        int32_t sleep = std::min(milliseconds, 100);

        if (sleep > 0) {
            Blizzard::Process::Sleep(sleep);
        }

        accumulatorNs += start - Blizzard::Time::GetTimestamp();
    } while (accumulatorNs > 1000000);
}

// OFFSET: 0x007C7A60/0x1404C2CE0
Blizzard::Streaming::Transport::Config* Blizzard::Streaming::Transport::GetConfig() {
    static Config s_config = { 10000, true };
    return &s_config;
}

// PROVEN: f9861b54 IDENTICAL/NONE
// OFFSET: 0x007C7AC0
bool Blizzard::Streaming::Transport::TransportHttpCallback::ShouldBlock() {
    return GetConfig()->m_shouldBlock;
}

// OFFSET: 0x007C7AD0/0x1404C2D80
void Blizzard::Streaming::Transport::TransportHttpCallback::SetCookie(const char* name, const char* value) {
    this->m_cookieName.assign(name, strlen(name));
    this->m_cookieValue.assign(value, strlen(value));
}

// OFFSET: 0x007C7B20
Blizzard::Streaming::Transport::TransportHttpCallback::TransportHttpCallback(int32_t size, std::string& data) : m_data(&data) {
    this->m_cookieName.erase(this->m_cookieName.begin(), this->m_cookieName.end());
    this->m_cookieValue.erase(this->m_cookieValue.begin(), this->m_cookieValue.end());
    this->m_data->reserve(size);
    this->m_data->erase(this->m_data->begin(), this->m_data->end());
    this->m_received = 0;
    this->m_rangeStart = 0;
    this->m_rangeEnd = 0;
    this->m_timeout = GetConfig()->m_timeout;
    this->m_stop = false;
    this->m_success = false;
    Blizzard::Lock::EventCreate(this->m_event);
}

// OFFSET: 0x007C7C10/0x1404C2EF0
bool Blizzard::Streaming::Transport::TransportHttpCallback::ReceivedData(const void* data, int32_t size) {
    if (this->m_received >= this->m_rangeEnd - this->m_rangeStart + 1) {
        this->m_stop = true;
    }

    if (size <= 0) {
        return true;
    }

    int32_t left = static_cast<int32_t>(this->m_rangeEnd - this->m_rangeStart) - this->m_received + 1;
    int32_t take = std::min(size, left);

    this->m_data->append(static_cast<const char*>(data), take);
    this->m_received += take;

    if (take != size) {
        this->m_stop = true;
    }

    return true;
}

// OFFSET: 0x007C74B0
Blizzard::Streaming::Transport::URL_INFO::URL_INFO(const char* url, int64_t start, int64_t end, int32_t size) : m_url(url) {
    this->m_start = start;
    this->m_end = end;
    this->m_size = size;
}

// OFFSET: 0x007E29A0/0x1404E94D0
void Blizzard::Streaming::Transport::GetSplitFileURL(const std::string& url, int64_t start, int64_t end, int32_t skip, int32_t splitSize, int32_t md5Size, int32_t size, std::vector<URL_INFO>* urls) {
    if (md5Size) {
        splitSize += 32 * (splitSize / md5Size);
    }

    int32_t first = static_cast<int32_t>(start / splitSize);
    int64_t firstOffset = start % splitSize;
    int32_t last = static_cast<int32_t>((end - 1) / splitSize);
    int32_t previous = 0;

    if (first > last) {
        return;
    }

    int64_t lastOffset = end % splitSize;

    for (int32_t chunk = first; chunk <= last; chunk++) {
        int64_t chunkStart = chunk == first ? firstOffset : 0;
        int64_t chunkEnd = lastOffset;

        if (lastOffset < chunkStart) {
            chunkEnd = splitSize - 1;
        }

        int64_t length = chunkEnd - chunkStart + 1;
        int32_t blocks = static_cast<int32_t>(length / (md5Size + 32));

        if (length % (md5Size + 32)) {
            blocks++;
        }

        int32_t chunkSize;

        if (chunk == first) {
            chunkSize = static_cast<int32_t>(chunkEnd) - 32 * blocks - static_cast<int32_t>(chunkStart) - skip + 1;
        } else {
            chunkSize = size - previous;
        }

        previous = chunkSize;

        char buffer[260];
        sprintf(buffer, "%s.%d", url.c_str(), chunk);

        urls->push_back(URL_INFO(buffer, chunkStart, chunkEnd, chunkSize));
    }
}

// OFFSET: 0x007EC5A0/0x1404F3DD0
bool Blizzard::Streaming::Transport::ReadHttp(const Manifests::TransportInfo& info, void* data, int64_t fileSize, int64_t offset, int32_t size) {
    int32_t error = 0;
    int32_t overhead = info.m_item ? 32 * (size / info.m_item->m_md5Size) : 0;

    TransportHttpCallback callback(size + overhead, s_transportBuffer);

    int64_t start = offset;
    int64_t end = offset + size - 1;
    int32_t skip = 0;
    std::vector<URL_INFO> urls;

    if (info.m_item) {
        int32_t md5Size = info.m_item->m_md5Size;
        skip = AdjustRange(&start, &end, md5Size, fileSize);

        if (info.m_item->m_splitSize > 0) {
            GetSplitFileURL(info.m_uncPath, start, end, skip, info.m_item->m_splitSize, md5Size, size, &urls);
        }
    } else {
        urls.push_back(URL_INFO(info.m_uncPath.c_str(), offset, end, size));
    }

    bool result = false;
    int32_t copied = 0;

    for (uint32_t i = 0; i < urls.size(); i++) {
        result = false;

        URL_INFO& url = urls[i];
        int64_t rangeStart = url.m_start;
        int64_t rangeEnd = url.m_end;
        int32_t rangeSize = url.m_size;

        callback.m_data->erase(callback.m_data->begin(), callback.m_data->end());
        callback.m_received = 0;
        callback.m_stop = false;
        callback.m_success = false;
        callback.m_rangeStart = rangeStart;
        callback.m_rangeEnd = rangeEnd;

        if (GetConfig()->m_cookieName.size() && GetConfig()->m_cookieValue.size()) {
            callback.m_cookieName = GetConfig()->m_cookieName.c_str();
            callback.m_cookieValue = GetConfig()->m_cookieValue.c_str();
        }

        for (int32_t attempt = 0; attempt < info.m_maxRetry; attempt++) {
            if (attempt) {
                System_File::Stacked::count_retry++;
            }

            result = Blizzard::URL::DownloadURL(url.m_url.c_str(), &callback);

            if (result) {
                if (!GetConfig()->m_shouldBlock) {
                    Blizzard::Lock::EventWaitForever(callback.m_event);
                }

                result = callback.m_success;

                if (!result) {
                    error = 3;
                    Blizzard::Process::Sleep(10);
                } else if (rangeEnd - rangeStart + 1 != callback.m_data->size()) {
                    result = false;
                    error = 2;
                } else if (info.m_item && !(result = MD5_Data(callback.m_data, info.m_item->m_md5Size))) {
                    error = 4;
                }
            } else {
                error = 1;
                Blizzard::Process::Sleep(10);
            }

            if (result) {
                int32_t amount = std::min(size, rangeSize);

                if (info.m_item) {
                    memmove(static_cast<uint8_t*>(data) + copied, callback.m_data->c_str() + skip, amount);
                } else {
                    memmove(data, callback.m_data->c_str(), amount);
                }

                copied += std::min(size, rangeSize);
                break;
            }

            callback.m_data->erase(callback.m_data->begin(), callback.m_data->end());
            callback.m_received = 0;
            callback.m_stop = false;
            callback.m_success = false;

            if (callback.m_status == Blizzard::URL::DownloadCallback::DOWNLOAD_TIMEOUT) {
                callback.m_timeout *= 2;
            }
        }
    }

    if (result && copied != size) {
        error = 5;
        result = false;
    }

    if (!result) {
        const char* message;

        switch (error) {
            case 1:
                message = "DownloadURL failed with no error";
                break;

            case 2:
                message = "Downloaded smaller than expected";
                break;

            case 3:
                if (callback.m_status == Blizzard::URL::DownloadCallback::DOWNLOAD_TIMEOUT) {
                    message = "DownloadURL failed - Timed out";
                } else if (callback.m_status == Blizzard::URL::DownloadCallback::DOWNLOAD_NOT_FOUND) {
                    message = "DownloadURL failed - File not found";
                } else {
                    message = "DownloadURL failed";
                }

                break;

            case 4:
                message = "MD5 check failed";
                break;

            case 5:
                message = "Copied smaller than expected";
                break;

            default:
                message = "Unknown error";
                break;
        }

        Blizzard::File::AddToLastErrorStack(callback.m_status, Blizzard::String::QuickFormat<1024>("%s - %s", message, info.m_uncPath.c_str()).m_buffer, 21);
    }

    return result;
}

// OFFSET: 0x007EFC10/0x1404F7E20
bool Blizzard::Streaming::Transport::Read(const Manifests::TransportInfo& info, void* data, int64_t fileSize, int64_t offset, int32_t size) {
    struct Handler {
        const char* m_prefix;
        bool (*m_read)(const Manifests::TransportInfo&, void*, int64_t, int64_t, int32_t);
    };

    Handler handlers[3] = {
        { "file://", ReadFile },
        { "http://", ReadHttp },
        { NULL, NULL }
    };

    for (Handler* handler = handlers; handler->m_prefix; handler++) {
        const char* url = info.m_uncPath.c_str();

        if (url && !_strnicmp(url, handler->m_prefix, strlen(handler->m_prefix))) {
            AddLatency latency;
            latency.m_bytes = 0;
            latency.m_start = Blizzard::Time::GetTimestamp();

            bool result = handler->m_read(info, data, fileSize, offset, size);

            if (result) {
                latency.m_bytes = size;
            }

            return result;
        }
    }

    return false;
}

// OFFSET: 0x007F0390/0x1404F9E80
bool Blizzard::Storage::HttpSource::Read(void* buffer, int64_t offset, int64_t size) {
    Blizzard::Streaming::Manifests::TransportInfo info;
    info.m_uncPath = Blizzard::Streaming::ManifestInfo::Get()->GetRemotePath(this->m_path);
    info.m_item = Blizzard::Streaming::ManifestInfo::GetTransportItem(this->m_path);
    info.m_maxRetry = Blizzard::Streaming::ManifestInfo::GetServerItemMaxRetry(info.m_uncPath, this->m_path);

    bool result = Blizzard::Streaming::Transport::Read(info, buffer, this->m_size, offset, static_cast<int32_t>(size));

    if (!result) {
        Blizzard::Streaming::ManifestInfo::DeactivateServer(info.m_uncPath, this->m_path);
    }

    return result;
}
