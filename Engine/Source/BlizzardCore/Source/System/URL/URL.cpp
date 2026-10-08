#include "URL.h"
#include <cstddef>
#include <cstdint>

// PROVEN: b4a4f159 DIFFERENT/IDENTICAL
// OFFSET: 0x007A4D50/0x140909F60
Blizzard::URL::DownloadCallback::~DownloadCallback() {
}

// PROVEN: b74c9f97 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
const char* Blizzard::URL::DownloadCallback::WantIfModifiedSince() {
    return NULL;
}

// PROVEN: a6c56ab6 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool Blizzard::URL::DownloadCallback::WantData() {
    return true;
}

// PROVEN: deafea3c IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void Blizzard::URL::DownloadCallback::GetLastModified(const char* lastModified) {
}

// PROVEN: 21209c1c IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void Blizzard::URL::DownloadCallback::GetContentLength(int64_t length) {
}

// PROVEN: 1d4642d1 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Blizzard::URL::DownloadCallback::ConnectedOK() {
}

// PROVEN: abe175a0 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void Blizzard::URL::DownloadCallback::GetHTTPStatusCode(int32_t code) {
}

// PROVEN: 217020cf IDENTICAL/IDENTICAL
// OFFSET: 0x007A4D00/0x140499940
int32_t Blizzard::URL::DownloadCallback::GetTimeout() {
    return 60000;
}

// PROVEN: f438aba5 IDENTICAL/IDENTICAL
// OFFSET: 0x007A4D10/0x140499950
Blizzard::URL::DownloadCallback::ByteRange Blizzard::URL::DownloadCallback::GetByteRange() {
    ByteRange range;
    range.m_start = -1;
    range.m_end = -1;
    return range;
}

// PROVEN: de849c14 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool Blizzard::URL::DownloadCallback::Stop() {
    return false;
}

// PROVEN: 34678b26 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool Blizzard::URL::DownloadCallback::ShouldBlock() {
    return false;
}

// OFFSET: 0x00401000
void Blizzard::URL::DownloadCallback::WaitForCompletion() {
}

// PROVEN: ba9c6c50 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
const char* Blizzard::URL::DownloadCallback::GetPostData() {
    return NULL;
}

// PROVEN: 58d31dfe IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
const char* Blizzard::URL::DownloadCallback::GetPostContentType() {
    return NULL;
}

// PROVEN: 18329adf IDENTICAL/IDENTICAL
// OFFSET: 0x007A4D30/0x140499970
Blizzard::URL::DownloadCallback::Cookie Blizzard::URL::DownloadCallback::GetCookie() {
    Cookie cookie;
    cookie.m_name = NULL;
    cookie.m_value = NULL;
    return cookie;
}
