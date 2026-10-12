#include "URL.h"
#include <cstddef>
#include <cstdint>

// OFFSET: 0x007A4D50/0x140909F60
Blizzard::URL::DownloadCallback::~DownloadCallback() {
}

// OFFSET: 0x00842700/0x1407FDD80
const char* Blizzard::URL::DownloadCallback::WantIfModifiedSince() {
    return NULL;
}

// OFFSET: 0x00A3AA30/0x1404BDD20
bool Blizzard::URL::DownloadCallback::WantData() {
    return true;
}

// OFFSET: 0x00847950/0x1407D7E80
void Blizzard::URL::DownloadCallback::GetLastModified(const char* lastModified) {
}

// OFFSET: 0x00841AE0/0x140211B90
void Blizzard::URL::DownloadCallback::GetContentLength(int64_t length) {
}

// OFFSET: 0x00401000/0x1407D7E80
void Blizzard::URL::DownloadCallback::ConnectedOK() {
}

// OFFSET: 0x00847950/0x1407D7E80
void Blizzard::URL::DownloadCallback::GetHTTPStatusCode(int32_t code) {
}

// OFFSET: 0x007A4D00/0x140499940
int32_t Blizzard::URL::DownloadCallback::GetTimeout() {
    return 60000;
}

// OFFSET: 0x007A4D10/0x140499950
Blizzard::URL::DownloadCallback::ByteRange Blizzard::URL::DownloadCallback::GetByteRange() {
    ByteRange range;
    range.m_start = -1;
    range.m_end = -1;
    return range;
}

// OFFSET: 0x00B06D20/0x140317F90
bool Blizzard::URL::DownloadCallback::Stop() {
    return false;
}

// OFFSET: 0x00B06D20/0x140317F90
bool Blizzard::URL::DownloadCallback::ShouldBlock() {
    return false;
}

// OFFSET: 0x00401000/0x1407D7E80
void Blizzard::URL::DownloadCallback::WaitForCompletion() {
}

// OFFSET: 0x00842700/0x1407FDD80
const char* Blizzard::URL::DownloadCallback::GetPostData() {
    return NULL;
}

// OFFSET: 0x00842700/0x1407FDD80
const char* Blizzard::URL::DownloadCallback::GetPostContentType() {
    return NULL;
}

// OFFSET: 0x007A4D30/0x140499970
Blizzard::URL::DownloadCallback::Cookie Blizzard::URL::DownloadCallback::GetCookie() {
    Cookie cookie;
    cookie.m_name = NULL;
    cookie.m_value = NULL;
    return cookie;
}
