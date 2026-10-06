#include "URL.h"
#include <cstddef>
#include <cstdint>

// OFFSET: %0x00A6F2C0
Blizzard::URL::DownloadCallback::~DownloadCallback() {
}

// PROVEN: b74c9f97 IDENTICAL
// OFFSET: 0x00842700
const char* Blizzard::URL::DownloadCallback::WantIfModifiedSince() {
    return NULL;
}

// PROVEN: a6c56ab6 IDENTICAL
// OFFSET: 0x00A3AA30
bool Blizzard::URL::DownloadCallback::WantData() {
    return true;
}

// PROVEN: deafea3c IDENTICAL
// OFFSET: 0x00847950
void Blizzard::URL::DownloadCallback::GetLastModified(const char* lastModified) {
}

// PROVEN: 21209c1c IDENTICAL
// OFFSET: 0x00841AE0
void Blizzard::URL::DownloadCallback::GetContentLength(int64_t length) {
}

// PROVEN: 1d4642d1 IDENTICAL
// OFFSET: 0x00401000
void Blizzard::URL::DownloadCallback::ConnectedOK() {
}

// PROVEN: abe175a0 IDENTICAL
// OFFSET: 0x00847950
void Blizzard::URL::DownloadCallback::GetHTTPStatusCode(int32_t code) {
}

// PROVEN: 217020cf IDENTICAL
// OFFSET: 0x007A4D00
int32_t Blizzard::URL::DownloadCallback::GetTimeout() {
    return 60000;
}

// PROVEN: f438aba5 IDENTICAL
// OFFSET: 0x007A4D10
Blizzard::URL::DownloadCallback::ByteRange Blizzard::URL::DownloadCallback::GetByteRange() {
    ByteRange range;
    range.m_start = -1;
    range.m_end = -1;
    return range;
}

// PROVEN: de849c14 IDENTICAL
// OFFSET: 0x00B06D20
bool Blizzard::URL::DownloadCallback::Stop() {
    return false;
}

// PROVEN: 34678b26 IDENTICAL
// OFFSET: 0x00B06D20
bool Blizzard::URL::DownloadCallback::ShouldBlock() {
    return false;
}

// PROVEN: ba9c6c50 IDENTICAL
// OFFSET: 0x00842700
const char* Blizzard::URL::DownloadCallback::GetPostData() {
    return NULL;
}

// PROVEN: 58d31dfe IDENTICAL
// OFFSET: 0x00842700
const char* Blizzard::URL::DownloadCallback::GetPostContentType() {
    return NULL;
}

// PROVEN: 18329adf IDENTICAL
// OFFSET: 0x007A4D30
Blizzard::URL::DownloadCallback::Cookie Blizzard::URL::DownloadCallback::GetCookie() {
    Cookie cookie;
    cookie.m_name = NULL;
    cookie.m_value = NULL;
    return cookie;
}
