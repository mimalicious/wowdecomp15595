#include "Tracker.h"
#include <cstdint>
#include <cstdlib>
#include <string>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Report.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// STATIC: %0x00D812E8
static int64_t s_repairSuccessBytes;

// STATIC: %0x00D812F0
static int64_t s_repairFailBytes;

// STATIC: %0x00D87628
static bool s_trackerDisabled;

// STATIC: %0x00D8762C
const char* Tracker::trackerAddress;

// STATIC: %0x00D812D8
static const char* s_localeTrackerAddress;

// STATIC: %0x00D812DC
static char s_locale[5];

// STATIC: %0x00D812E4
static int32_t s_buildnum;

// STATIC: %0x00D87944
static char s_peerId[21];

// STATIC: %0x00D87970
static char s_key[9];

// PROVEN: e4cc653f IDENTICAL
// OFFSET: 0x007A4D90
void Tracker::TrackerHttpDataDownloadCallBack::Completed(Blizzard::URL::DownloadCallback::Status status) {
    Blizzard::Lock::EventSet(this->m_event);
}

// OFFSET: %0x00762A60
char* Tracker::GetPeerID() {
    if (s_peerId[0]) {
        return s_peerId;
    }

    Blizzard::Hardware::Fingerprint fingerprint;

    if (Blizzard::Hardware::GetFingerprint(&fingerprint)) {
        for (int32_t i = 0; i < 10; i++) {
            s_peerId[2 * i] = Blizzard::Util::itox(fingerprint.m_data[i] >> 4);
            s_peerId[2 * i + 1] = Blizzard::Util::itox(fingerprint.m_data[i]);
        }
    } else {
        for (int32_t i = 0; i < 20; i++) {
            s_peerId[i] = rand() % 222 + 33;
        }
    }

    s_peerId[20] = '\0';

    return s_peerId;
}

// PROVEN: 99e03d51 IDENTICAL
// OFFSET: 0x007FCDD0
char* Tracker::GetKey() {
    if (!s_key[0]) {
        for (int32_t i = 0; i < 8; i++) {
            s_key[i] = rand() % 9 + '0';
        }

        s_key[8] = '\0';
    }

    return s_key;
}

// PROVEN: 8c7278d9 IDENTICAL
// OFFSET: 0x007FB3C0
const char* Tracker::GetTrackerAddress(const char* locale) {
    if (!Blizzard::String::Equal(locale, "enUS") || !Blizzard::String::Equal(locale, "esMX")) {
        return "http://us.tracker.worldofwarcraft.com:3724/announce";
    }

    if (!Blizzard::String::Equal(locale, "deDE") || !Blizzard::String::Equal(locale, "enGB") || !Blizzard::String::Equal(locale, "esES") || !Blizzard::String::Equal(locale, "frFR")) {
        return "http://eu.tracker.worldofwarcraft.com:3724/announce";
    }

    return NULL;
}

// PROVEN: fbb71344 IDENTICAL
// OFFSET: 0x007FB450
void Tracker::SetLocale(const char* locale) {
    s_localeTrackerAddress = GetTrackerAddress(locale);
    Blizzard::String::Copy(s_locale, locale, sizeof(s_locale));
}

// OFFSET: %0x0070BBC0
void Tracker::SetBuildnum(int32_t buildnum) {
    s_buildnum = buildnum;
}

// OFFSET: %0x0070BEF0
void Tracker::RepairReportFail(const char* name, int64_t offset, uint32_t size, const char* error) {
    s_repairFailBytes += size;
    Report::SetData("rpfl", s_repairFailBytes);
    Report::SetData("rpfn", std::string(name));
    Report::SetData("rper", std::string(error));
    Report::SendReport();
    Report::DeleteData("rpfn");
    Report::DeleteData("rper");
}

// OFFSET: %0x0070C070
void Tracker::RepairReportSuccess(const char* name, int64_t offset, uint32_t size) {
    s_repairSuccessBytes += size;
    Report::SetData("rpsc", s_repairSuccessBytes);
}

// OFFSET: #0x00462500
Tracker::TrackerHttpDataDownloadCallBack::TrackerHttpDataDownloadCallBack() {
    this->m_timeout = 10000;
    this->m_shouldBlock = true;
    Blizzard::Lock::EventCreate(this->m_event);
}

// PROVEN: 1e8a1346 IDENTICAL
// OFFSET: 0x007FD0B0
int32_t Tracker::TrackerHttpDataDownloadCallBack::GetTimeout() {
    return this->m_timeout;
}

// OFFSET: %0x00762E00
bool Tracker::TrackerHttpDataDownloadCallBack::ShouldBlock() {
    return this->m_shouldBlock;
}

// PROVEN: 6d8dcf79 IDENTICAL
// OFFSET: 0x007FD0D0
void Tracker::TrackerHttpDataDownloadCallBack::SetTimeOutValue(uint32_t timeout) {
    this->m_timeout = timeout;
}

// OFFSET: [#0x004625A0, %0x00762E90]
Tracker::TrackerHttpDataDownloadCallBack::~TrackerHttpDataDownloadCallBack() {
    Blizzard::Lock::EventDestroy(this->m_event);
}

// OFFSET: %0x007632A0
bool Tracker::TrackerHttpDataDownloadCallBack::ReceivedData(const void* data, int32_t size) {
    this->m_data.append(static_cast<const char*>(data), size);
    return true;
}

// OFFSET: #0x00462660
std::string Tracker::ByteToHex(uint8_t byte) {
    char digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    std::string hex;
    hex.append(1, digits[byte >> 4]);
    hex.append(1, digits[byte & 0xF]);
    return hex;
}

// OFFSET: #0x004627D0
std::string Tracker::BytesToHex(const uint8_t* bytes, int32_t count) {
    std::string hex;

    for (int32_t i = 0; i < count; i++) {
        hex += ByteToHex(bytes[i]);
    }

    return hex;
}

// OFFSET: #0x00462870
std::string Tracker::Quote(const char* str, uint32_t length) {
    std::string quoted;

    for (uint32_t i = 0; i < length; i++) {
        char c = str[i];

        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '.' || c == '-' || c == '/') {
            quoted.append(1, str[i]);
        } else {
            quoted.append(1, '%');
            quoted += BytesToHex(reinterpret_cast<const uint8_t*>(&str[i]), 1);
        }
    }

    return quoted;
}

// OFFSET: %0x00763450
std::string Tracker::MakeTrackerURL(const char* stats, bool start) {
    std::string key = GetKey();
    std::string peerId = GetPeerID();
    std::string port = "3724";

    std::string url = Tracker::trackerAddress;
    url += "?info_hash=" + Quote("DOTRYWORLDOFWARCRAFT", 20);
    url += "&peer_id=" + Quote(peerId.c_str(), peerId.size());
    url += "&key=" + Quote(key.c_str(), 8);
    url.append("&port=", 6);
    url += port.c_str();
    url.append("&uploaded=0", 11);

    if (start) {
        url.append("&downloaded=0", 13);
        url.append("&left=1", 7);
        url.append("&event=started", 14);
        url += stats;
    } else {
        url.append("&downloaded=1", 13);
        url.append("&left=0", 7);
        url.append("&event=stopped", 14);
    }

    return url;
}

// OFFSET: %0x007637B0
void Tracker::SendToTracker(const char* stats) {
    if (s_trackerDisabled || !Tracker::trackerAddress) {
        return;
    }

    TrackerHttpDataDownloadCallBack callback;
    std::string url = MakeTrackerURL(stats, true);
    Blizzard::URL::DownloadURL(url.c_str(), &callback);
    Blizzard::Lock::EventWaitForever(callback.m_event);

    url = MakeTrackerURL(stats, false);
    Blizzard::URL::DownloadURL(url.c_str(), &callback);
    Blizzard::Lock::EventWaitForever(callback.m_event);
}
