#include "URL.h"
#include <cstddef>
#include <cstdint>

// OFFSET: %0x00A6F2C0
Blizzard::URL::DownloadCallback::~DownloadCallback() {
}

// OFFSET: %0x00408910
const char* Blizzard::URL::DownloadCallback::WantIfModifiedSince() {
    return NULL;
}

// OFFSET: %0x004E0850
bool Blizzard::URL::DownloadCallback::WantData() {
    return true;
}

// OFFSET: %0x00A42EC0
void Blizzard::URL::DownloadCallback::GetLastModified(const char* lastModified) {
}

// OFFSET: %0x007C8240
void Blizzard::URL::DownloadCallback::GetContentLength(int64_t length) {
}

// OFFSET: %0x00401000
void Blizzard::URL::DownloadCallback::ConnectedOK() {
}

// OFFSET: %0x00A42EC0
void Blizzard::URL::DownloadCallback::GetHTTPStatusCode(int32_t code) {
}

// OFFSET: %0x0070BAB0
int32_t Blizzard::URL::DownloadCallback::GetTimeout() {
    return 60000;
}

// OFFSET: %0x0070BAC0
Blizzard::URL::DownloadCallback::ByteRange Blizzard::URL::DownloadCallback::GetByteRange() {
    ByteRange range;
    range.m_start = -1;
    range.m_end = -1;
    return range;
}

// OFFSET: %0x005A4510
bool Blizzard::URL::DownloadCallback::Stop() {
    return false;
}

// OFFSET: %0x005A4510
bool Blizzard::URL::DownloadCallback::ShouldBlock() {
    return false;
}

// OFFSET: %0x00408910
const char* Blizzard::URL::DownloadCallback::GetPostData() {
    return NULL;
}

// OFFSET: %0x00408910
const char* Blizzard::URL::DownloadCallback::GetPostContentType() {
    return NULL;
}

// OFFSET: %0x0070BAE0
Blizzard::URL::DownloadCallback::Cookie Blizzard::URL::DownloadCallback::GetCookie() {
    Cookie cookie;
    cookie.m_name = NULL;
    cookie.m_value = NULL;
    return cookie;
}
