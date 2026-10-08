#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_URL_URL_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_URL_URL_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

namespace Blizzard {
namespace URL {

class DownloadCallback {
    public:
    enum Status {
        DOWNLOAD_SUCCESS = 0,
        DOWNLOAD_FAILED = 1,
        DOWNLOAD_TIMEOUT = 2,
        DOWNLOAD_NOT_FOUND = 3,
        DOWNLOAD_NOT_MODIFIED = 4,
        DOWNLOAD_ABORTED = 5,
        DOWNLOAD_FORBIDDEN = 6,
        DOWNLOAD_NAME_NOT_RESOLVED = 7,
        DOWNLOAD_CANNOT_CONNECT = 8,
        DOWNLOAD_STOPPED = 9,
        DOWNLOAD_BAD_REQUEST = 10,
        DOWNLOAD_UNKNOWN_ERROR = 11,
    };

    struct ByteRange {
        /* 0000 */ int64_t m_start;
        /* 0008 */ int64_t m_end;
    };

    struct Cookie {
        /* 0000 */ const char* m_name;
        /* 0004 */ const char* m_value;
    };

    virtual ~DownloadCallback();
    virtual const char* WantIfModifiedSince();
    virtual bool WantData();
    virtual void GetLastModified(const char* lastModified);
    virtual void GetContentLength(int64_t length);
    virtual void ConnectedOK();
    virtual void GetHTTPStatusCode(int32_t code);
    virtual int32_t GetTimeout();
    virtual ByteRange GetByteRange();
    virtual bool Stop();
    virtual bool ShouldBlock();
    virtual void WaitForCompletion();
    virtual const char* GetPostData();
    virtual const char* GetPostContentType();
    virtual Cookie GetCookie();
    virtual bool ReceivedData(const void* data, int32_t size) = 0;
    virtual void Completed(Status status) = 0;
};

class HttpRequest {
    public:
    /* 0000 */ void* m_session;
    /* 0004 */ void* m_connect;
    /* 0008 */ void* m_request;
    /* 000C */ Blizzard::Lock::EventObject m_connectEvent;
    /* 0010 */ Blizzard::Lock::EventObject m_requestEvent;
    /* 0014 */ Blizzard::Lock::EventObject m_completeEvent;
    /* 0018 */ int32_t m_state;
    /* 001C */ bool m_done;
    /* 001D */ bool m_connected;
    /* 0020 */ uint32_t m_error;
    /* 0024 */ DownloadCallback::Status m_status;
    /* 0028 */ DownloadCallback* m_callback;

    HttpRequest();
};

bool DownloadURL(const char* url, DownloadCallback* callback);

}
}

#endif
