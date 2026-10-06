#include "Manifests.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack_Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: #0x00AB90A4
const char* Blizzard::Streaming::s_httpPrefix = "http://";

// STATIC: #0x00AB90A8
const char* Blizzard::Streaming::s_filePrefix = "file://";

// STATIC: %0x00D81374
Blizzard::Streaming::PROCESSURLFUNC Blizzard::Streaming::s_processUrlFunc;

// STATIC: #0x00B38170
Blizzard::Streaming::ManifestInfo* Blizzard::Streaming::ManifestInfo::s_instance;

// STATIC: #0x00B38660
static Blizzard::Streaming::Manifests::ServerItem s_serverItem;

// PROVEN: fab8fff7 IDENTICAL
// OFFSET: 0x006705F0
bool Blizzard::Streaming::Manifests::ManifestItem::HandleTag(const char* tag, const char* value) {
    return false;
}

// OFFSET: %0x0070E890
bool Blizzard::Streaming::SplitLine(char* line, char** value, char** tag, int32_t* length) {
    if (!*line) {
        return false;
    }

    *tag = NULL;
    *value = line;

    char* cursor = line;

    while (*cursor) {
        char c = *cursor;

        if (c == '=') {
            *cursor = '\0';
            *tag = line;
            *value = cursor + 1;
        } else if (c == '\r' || c == '\n') {
            while (*cursor == '\r' || *cursor == '\n') {
                *cursor++ = '\0';
            }

            *length = cursor - line;
            return true;
        }

        cursor++;
    }

    *length = cursor - line;
    return true;
}

// OFFSET: %0x0071C980
bool STORMAPI Blizzard::Streaming::ManifestInfo::GetItemTag(char* line, char** value, char** tag, int32_t* length) {
    if (!line || !*line) {
        return false;
    }

    *tag = NULL;
    *value = line;

    char* cursor = line;

    while (*cursor) {
        char c = *cursor;

        if (c == '=') {
            *cursor = '\0';
            *tag = line;
            *value = cursor + 1;
        } else if (c == '\r' || c == '\n') {
            while (*cursor == '\r' || *cursor == '\n') {
                *cursor++ = '\0';
            }

            break;
        }

        cursor++;
    }

    *length = static_cast<int32_t>(cursor - line);
    *value = TrimLine(*value);
    *tag = TrimLine(*tag);

    if (*tag) {
        return true;
    }

    return *value && !**value;
}

// OFFSET: %0x00715BA0
bool Blizzard::Streaming::Manifests::TransportItem::HandleTag(const char* tag, const char* value) {
    if (!tag) {
        return false;
    }

    if (!_stricmp(tag, "md5size")) {
        this->m_md5Size = atol(value);
        return true;
    }

    if (_stricmp(tag, "splitsize")) {
        return false;
    }

    this->m_splitSize = atol(value);
    return true;
}

// OFFSET: %0x00715C20
Blizzard::Time::Timestamp Blizzard::Streaming::Manifests::TimeslotItem::ParseTimestamp(const char* time) {
    Blizzard::Time::TimeRec rec;
    memset(&rec.second, 0, 16);
    rec.day = 1;
    rec.month = 1;
    rec.year = 2000;

    int32_t hour;
    int32_t minute;
    sscanf(time, "%d:%d", &hour, &minute);

    rec.hour = hour;
    rec.minute = minute;

    return Blizzard::Time::MakeTime(rec);
}

// OFFSET: #0x0042D7A0
char* STORMAPI Blizzard::Streaming::TrimLine(char* line) {
    char* start = line;

    if (!*start) {
        return start;
    }

    uint32_t length = strlen(line);

    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') {
        start++;
        length--;

        if (!*start) {
            break;
        }
    }

    for (char* end = &start[length]; *end; *end-- = '\0') {
        char c = *end;

        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            break;
        }
    }

    return start;
}

// OFFSET: %0x0071C8D0
bool Blizzard::Streaming::Manifests::TimeslotItem::HandleTag(const char* tag, const char* value) {
    if (!tag) {
        return false;
    }

    if (!_stricmp(tag, "starttime")) {
        this->m_startTime = ParseTimestamp(value);
        return true;
    }

    if (!_stricmp(tag, "endtime")) {
        this->m_endTime = ParseTimestamp(value);
        return true;
    }

    if (_stricmp(tag, "usage")) {
        return false;
    }

    this->m_usage = atol(value);
    return true;
}

// OFFSET: #0x00437A80
Blizzard::Streaming::Manifests::ManifestItem::~ManifestItem() {
}

// OFFSET: [#0x00437D20, %0x00724740]
Blizzard::Streaming::Manifests::ParameterItem::~ParameterItem() {
}

// OFFSET: %0x00724710
Blizzard::Streaming::Manifests::ParameterItem::ParameterItem() {
}

// PROVEN: f733ec05 IDENTICAL
// OFFSET: 0x007C6030
int32_t Blizzard::Streaming::Manifests::ServerItem::GetUsage(Blizzard::Time::Timestamp time) {
    int32_t usage = 0;

    for (std::vector<UsageSlot>::iterator it = this->m_usageSlots.begin(); it != this->m_usageSlots.end(); ++it) {
        if (it->m_time > time || !this->m_active) {
            break;
        }

        usage = it->m_usage;
    }

    return usage;
}

// OFFSET: #0x0043ACA0
bool Blizzard::Streaming::FileManifest::SplitItem(char* line, MANIFEST_ITEM* item) {
    char* field = line;

    if (!*line) {
        return false;
    }

    uint32_t index = 0;
    char* cursor = field;

    do {
        if (*cursor == ';') {
            do {
                *cursor++ = '\0';
            } while (*cursor == ';');

            switch (index) {
                case 0: {
                    char absolute[260];
                    Blizzard::File::MakeAbsolutePath(field, absolute, 260, false);

                    char normalized[260];
                    char* dst = normalized;
                    const char* src = absolute;
                    uint32_t left = 260;

                    while (true) {
                        char c = *src++;
                        left--;

                        if (!c) {
                            *dst = '\0';
                            break;
                        }

                        *dst++ = c == '/' ? '\\' : c;

                        if (!left) {
                            dst[-1] = '\0';
                            break;
                        }
                    }

                    item->m_absolutePath.assign(normalized, strlen(normalized));
                    item->m_path.assign(field, strlen(field));
                    break;
                }

                case 1:
                    item->m_size = _atoi64(field);
                    break;

                case 2:
                    item->m_fileVersion.assign(field, strlen(field));
                    break;

                case 3:
                    item->m_flags = atol(field);
                    break;
            }

            index++;
            field = cursor;
        }

        cursor++;
    } while (*cursor);

    if (index == 3) {
        item->m_flags = atol(field);
    }

    return true;
}

// OFFSET: #0x0043AE10
bool Blizzard::Streaming::SourceManifest::SplitItem(char* line, MANIFEST_ITEM* item) {
    char* field = line;

    if (!*line) {
        return false;
    }

    uint32_t index = 0;
    char* cursor = field;

    do {
        if (*cursor == ';') {
            do {
                *cursor++ = '\0';
            } while (*cursor == ';');

            if (index == 0) {
                item->m_name.assign(field, strlen(field));
            } else if (index == 1) {
                item->m_path.assign(field, strlen(field));
            }

            index++;
            field = cursor;
        }

        cursor++;
    } while (*cursor);

    if (index == 1) {
        item->m_path.assign(field, strlen(field));
    }

    return true;
}

// OFFSET: #0x0043DBF0
bool Blizzard::Streaming::Manifests::ServerPathItem::HandleTag(const char* tag, const char* value) {
    if (!tag || _stricmp(tag, "path")) {
        return false;
    }

    this->m_path = std::string(value);
    return true;
}

// OFFSET: #0x0043DCC0
bool Blizzard::Streaming::Manifests::ParameterItem::HandleTag(const char* tag, const char* value) {
    if (!tag || _stricmp(tag, "value")) {
        return false;
    }

    this->m_value = std::string(value);
    return true;
}

// OFFSET: #0x0043DD90
bool Blizzard::Streaming::FileManifest::SplitServerItem(char* line, std::string& base, std::string& path) {
    if (!*line) {
        return false;
    }

    uint32_t prefixLength;

    if (s_httpPrefix && !_strnicmp(s_httpPrefix, line, strlen(s_httpPrefix))) {
        prefixLength = s_httpPrefix ? strlen(s_httpPrefix) : 0;
    } else if (s_filePrefix && !_strnicmp(s_filePrefix, line, strlen(s_filePrefix))) {
        prefixLength = s_filePrefix ? strlen(s_filePrefix) : 0;
    } else {
        return false;
    }

    char* cursor = &line[prefixLength];

    if (!*cursor) {
        return false;
    }

    while (*cursor != '/') {
        if (!*++cursor) {
            return false;
        }
    }

    for (; *cursor == '/'; cursor++) {
        *cursor = '\0';
    }

    base.assign(line, strlen(line));
    base.append("/", 1);
    path.assign(cursor, strlen(cursor));

    return true;
}

// PROVEN: f41f98f2 IDENTICAL
// OFFSET: 0x007BB4B0
int32_t Blizzard::Streaming::Manifests::FileItem::GetContentId() {
    return atoi(this->m_fileVersion.c_str());
}

// OFFSET: %0x0072DF40
std::string Blizzard::Streaming::Manifests::FileItem::GetRemotePath() {
    return this->m_path + this->m_fileName;
}

// OFFSET: [#0x00444000, %0x007370D0]
Blizzard::Streaming::Manifests::ServerItem::~ServerItem() {
}

// OFFSET: #0x004440A0
Blizzard::Streaming::Manifests::FileItem::FileItem() {
}

// OFFSET: [#0x00444130, #0x00444250]
Blizzard::Streaming::Manifests::FileItem::~FileItem() {
}

// OFFSET: #0x004474D0
Blizzard::Streaming::Manifests::ServerItem::ServerItem() {
    UsageSlot slot;
    slot.m_time = TimeslotItem::ParseTimestamp("0:00");
    slot.m_usage = 100;
    this->m_usageSlots.push_back(slot);

    this->m_uses = 0;
    this->m_canDeactivate = 0;
    this->m_maxRetry = 5;
    this->m_active = true;
}

// OFFSET: #0x004475B0
void Blizzard::Streaming::Manifests::ServerItem::InsertUsageSlot(UsageSlot& slot, Blizzard::Time::Timestamp end) {
    UsageSlot* endSlot = NULL;
    std::vector<UsageSlot>::iterator it = this->m_usageSlots.begin();

    while (it != this->m_usageSlots.end()) {
        if (endSlot) {
            if (it->m_time > endSlot->m_time) {
                break;
            }

            if (&*it == endSlot) {
                ++it;
            } else {
                endSlot->m_usage = it->m_usage;
                it = this->m_usageSlots.erase(it);
            }
        } else {
            if (slot.m_time > it->m_time) {
                ++it;
                continue;
            }

            it = this->m_usageSlots.insert(it, slot);
            ++it;
            endSlot = &*it;
            it->m_time = end;
        }
    }

    if (!endSlot) {
        this->m_usageSlots.push_back(slot);

        if (end) {
            UsageSlot endUsage;
            endUsage.m_time = end;
            endUsage.m_usage = 100;
            this->m_usageSlots.push_back(endUsage);
        }
    }
}

// PROVEN: d3f26c76 IDENTICAL
// OFFSET: 0x007D9AF0
void Blizzard::Streaming::Manifests::ServerItem::InsertTimeslot(TimeslotItem* timeslot) {
    UsageSlot slot;

    if (timeslot->m_startTime == timeslot->m_endTime) {
        this->m_usageSlots.clear();
        slot.m_time = TimeslotItem::ParseTimestamp("0:00");
        slot.m_usage = timeslot->m_usage;
        this->InsertUsageSlot(slot, 0);
    } else if (timeslot->m_startTime > timeslot->m_endTime) {
        slot.m_time = TimeslotItem::ParseTimestamp("0:00");
        slot.m_usage = timeslot->m_usage;
        this->InsertUsageSlot(slot, timeslot->m_endTime);
        slot.m_time = timeslot->m_startTime;
        this->InsertUsageSlot(slot, 0);
    } else {
        slot.m_time = timeslot->m_startTime;
        slot.m_usage = timeslot->m_usage;
        this->InsertUsageSlot(slot, timeslot->m_endTime);
    }
}

// OFFSET: #0x00447810
bool Blizzard::Streaming::ManifestInfo::FinalizeManifestItems() {
    bool result = true;

    for (std::map<std::string, Manifests::FileItem>::iterator it = this->m_files.begin(); it != this->m_files.end(); ++it) {
        Manifests::FileItem& file = it->second;

        if (!file.m_size) {
            continue;
        }

        if (file.m_path.size()) {
            std::map<std::string, Manifests::ServerPathItem>::iterator path = this->m_serverPaths.find(file.m_path);

            if (path == this->m_serverPaths.end()) {
                result = false;
            } else {
                file.m_path = path->second.m_path;
            }
        }

        if (!file.m_transportItem.size()) {
            file.m_transport = NULL;
            continue;
        }

        std::map<std::string, Manifests::TransportItem>::iterator transport = this->m_transports.find(file.m_transportItem);

        if (transport == this->m_transports.end()) {
            if (this->m_version <= 1) {
                file.m_transport = NULL;
            } else {
                result = false;
            }
        } else {
            file.m_transport = &transport->second;
        }
    }

    for (std::map<std::string, Manifests::ServerItem>::iterator it = this->m_servers.begin(); it != this->m_servers.end(); ++it) {
        for (std::vector<std::string>::iterator id = it->second.m_timeslotIds.begin(); id != it->second.m_timeslotIds.end(); ++id) {
            std::map<std::string, Manifests::TimeslotItem>::iterator timeslot = this->m_timeslots.find(*id);

            if (timeslot == this->m_timeslots.end()) {
                result = false;
            } else {
                it->second.InsertTimeslot(&timeslot->second);
            }
        }
    }

    return result;
}

// OFFSET: #0x0044C630
bool Blizzard::Streaming::Transport::SplitItem(char* line, TRANSPORT_ITEM* item) {
    char* field = line;

    if (!*line) {
        return false;
    }

    uint32_t index = 0;
    item->m_md5Size = 0;
    item->m_splitSize = 0;

    char* cursor = field;

    if (!*cursor) {
        return true;
    }

    do {
        if (*cursor == ';') {
            do {
                *cursor++ = '\0';
            } while (*cursor == ';');

            switch (index) {
                case 0:
                    item->m_name.assign(field, strlen(field));
                    index = 1;
                    break;

                case 1:
                    item->m_md5Size = atol(field);
                    index = 2;
                    break;

                case 2:
                    item->m_splitSize = atol(field);
                    index = 3;
                    break;

                case 3:
                    index = 4;
                    break;

                case 4:
                    item->m_servers.push_back(std::string(field));
                    break;
            }

            field = cursor;
        }

        cursor++;
    } while (*cursor);

    if (index == 4) {
        item->m_servers.push_back(std::string(field));
    }

    return true;
}

// OFFSET: #0x0044C790
bool Blizzard::Streaming::Manifests::FileItem::HandleTag(const char* tag, const char* value) {
    if (tag) {
        if (!_stricmp(tag, "name")) {
            this->m_fileName = std::string(value);
            return true;
        }

        if (!_stricmp(tag, "size")) {
            this->m_size = _atoi64(value);
            return true;
        }

        if (!_stricmp(tag, "fileversion")) {
            this->m_fileVersion = std::string(value);
            return true;
        }

        if (!_stricmp(tag, "flags")) {
            this->m_flags = atol(value);
            return true;
        }
    }

    if (Blizzard::String::EqualI(tag, "path")) {
        this->m_path = std::string(value);
        return true;
    }

    if (Blizzard::String::EqualI(tag, "transportitem")) {
        this->m_transportItem = std::string(value);
        return true;
    }

    if (Blizzard::String::EqualI(tag, "validserver")) {
        this->m_validServers.push_back(std::string(value));
        return true;
    }

    return false;
}

// OFFSET: #0x0044C9F0
bool Blizzard::Streaming::Manifests::ServerItem::HandleTag(const char* tag, const char* value) {
    if (tag) {
        if (!_stricmp(tag, "location")) {
            this->m_location = std::string(value);
            return true;
        }

        if (!_stricmp(tag, "timeslotid")) {
            this->m_timeslotIds.push_back(std::string(value));
            return true;
        }

        if (!_stricmp(tag, "maxretry")) {
            this->m_maxRetry = atol(value);
            return true;
        }

        if (!_stricmp(tag, "candeactivate")) {
            this->m_canDeactivate = atol(value);
            return true;
        }
    }

    return false;
}

// OFFSET: #0x004500B0
Blizzard::Streaming::Manifests::ManifestItem* Blizzard::Streaming::ManifestInfo::HandleBaseTag(const char* tag, const char* value) {
    if (tag) {
        if (!_stricmp(tag, "file")) {
            Manifests::FileItem item;
            item.m_name.assign(value, strlen(value));
            std::string key = System_File::GetInode(item.m_name);

            this->m_files[key] = item;
            this->m_files[key].m_localPath = GetAbsolutePath(item.m_name);
            this->m_files[key].m_flags = 0;

            return &this->m_files[key];
        }

        if (!_stricmp(tag, "server")) {
            Manifests::ServerItem item;
            item.m_name.assign(value, strlen(value));
            this->m_servers[item.m_name] = item;

            return &this->m_servers[item.m_name];
        }

        if (!_stricmp(tag, "serverpath")) {
            Manifests::ServerPathItem item;
            item.m_name.assign(value, strlen(value));
            this->m_serverPaths[item.m_name] = item;

            return &this->m_serverPaths[item.m_name];
        }

        if (!_stricmp(tag, "timeslot")) {
            Manifests::TimeslotItem item;
            item.m_name = value;
            this->m_timeslots[item.m_name] = item;

            return &this->m_timeslots[item.m_name];
        }
    }

    if (Blizzard::String::EqualI(tag, "transport")) {
        Manifests::TransportItem item;
        item.m_name = value;
        this->m_transports[item.m_name] = item;

        return &this->m_transports[item.m_name];
    }

    if (Blizzard::String::EqualI(tag, "parameter")) {
        Manifests::ParameterItem item;
        item.m_name = value;
        this->m_parameters[item.m_name] = item;

        return &this->m_parameters[item.m_name];
    }

    return NULL;
}

// OFFSET: #0x00450580
std::string Blizzard::Streaming::ManifestInfo::GetRandomServerLocation(const std::vector<std::string>& validServers) {
    std::string names[5];
    int32_t weights[5];
    int32_t count = 0;
    int32_t total = 0;

    Blizzard::Time::TimeRec rec;
    Blizzard::Time::BreakTime(Blizzard::Time::GetTimestamp(), rec);
    rec.year = 2000;
    rec.month = 1;
    rec.day = 1;
    rec.nanosecond = 0;
    rec.second = 0;
    Blizzard::Time::Timestamp now = Blizzard::Time::MakeTime(rec);

    if (validServers.size()) {
        for (std::vector<std::string>::const_iterator it = validServers.begin(); it != validServers.end(); ++it) {
            std::map<std::string, Manifests::ServerItem>::iterator server = this->m_servers.find(*it);

            if (server == this->m_servers.end()) {
                continue;
            }

            int32_t usage = server->second.GetUsage(now);

            if (usage > 0) {
                total += usage;
                names[count] = server->first;
                weights[count++] = total;
            }
        }
    } else {
        for (std::map<std::string, Manifests::ServerItem>::iterator server = this->m_servers.begin(); server != this->m_servers.end(); ++server) {
            int32_t usage = server->second.GetUsage(now);

            if (usage > 0) {
                total += usage;
                names[count] = server->first;
                weights[count++] = total;
            }
        }
    }

    if (total) {
        int32_t roll = this->m_randomGenerator->RandomGenerator(total);

        for (int32_t i = 0; i < count; i++) {
            if (weights[i] > roll) {
                Manifests::ServerItem& server = this->m_servers[names[i]];
                server.m_uses++;
                return server.m_location;
            }
        }
    }

    return std::string("", static_cast<size_t>(0));
}

// OFFSET: %0x00747240
Blizzard::Streaming::ManifestInfo::ManifestInfo() {
    this->m_version = 0;
    srand(static_cast<uint32_t>(_time32(NULL)));
    this->m_randomGenerator = new ManifestRandomNumberGenerator();
}

// OFFSET: #0x00452040
Blizzard::Streaming::ManifestInfo::~ManifestInfo() {
}

// OFFSET: %0x0070E750
Blizzard::Streaming::ManifestRandomNumberGenerator::~ManifestRandomNumberGenerator() {
}

// PROVEN: 35575275 IDENTICAL
// OFFSET: 0x007AB950
int32_t Blizzard::Streaming::ManifestRandomNumberGenerator::RandomGenerator(int32_t range) {
    return rand() % range;
}

// OFFSET: %0x007473F0
Blizzard::Streaming::ManifestInfo* Blizzard::Streaming::ManifestInfo::Get() {
    if (!s_instance) {
        s_instance = new ManifestInfo();
    }

    return s_instance;
}

// OFFSET: %0x00747460
std::string Blizzard::Streaming::ManifestInfo::GetParameter(const char* name) {
    std::map<std::string, Manifests::ParameterItem>::iterator it = Get()->m_parameters.find(std::string(name));

    if (it == Get()->m_parameters.end()) {
        return std::string();
    }

    return it->second.m_value;
}

// OFFSET: %0x00747580
void Blizzard::Streaming::ManifestInfo::SetParameter(const char* name, const char* value) {
    Manifests::ParameterItem item;
    item.m_name.assign(name, strlen(name));
    item.m_value = std::string(value);
    Get()->m_parameters[std::string(name)] = item;
}

// OFFSET: %0x00747690
void Blizzard::Streaming::ManifestInfo::SetParameter(const char* name, int32_t value) {
    char text[16];
    Blizzard::String::Format(text, sizeof(text), "%d", value);

    Manifests::ParameterItem item;
    item.m_name.assign(name, strlen(name));
    item.m_value = std::string(text);
    Get()->m_parameters[std::string(name)] = item;
}

// OFFSET: #0x00452970
void Blizzard::Streaming::ManifestInfo::SetServerPathItem(std::string name, std::string path) {
    std::string serverPathName = name;
    std::string serverPathPath = path;

    Manifests::ServerPathItem& serverPath = Get()->m_serverPaths[name];
    serverPath.m_name = serverPathName;
    serverPath.m_path = serverPathPath;
}

// OFFSET: #0x00452A90
void Blizzard::Streaming::ManifestInfo::SetTransportItem(std::string name, const Manifests::TransportItem& item) {
    Get()->m_transports[name] = item;
}

// OFFSET: #0x00452B10
void Blizzard::Streaming::ManifestInfo::SetFileItem(std::string name, const Manifests::FileItem& item) {
    Get()->m_files[name] = item;

    if (Get()->m_transports.find(item.m_name) != Get()->m_transports.end()) {
        Get()->m_files[name].m_transportItem = item.m_name;
    }

    if (Get()->m_serverPaths.find(item.m_name) != Get()->m_serverPaths.end()) {
        Get()->m_files[name].m_path = item.m_name;
    }
}

// OFFSET: #0x00452C30
void Blizzard::Streaming::ManifestInfo::SetServerItem(std::string name, const Manifests::ServerItem& item) {
    Get()->m_servers[name] = item;
}

// OFFSET: %0x00728E50
Blizzard::Streaming::Manifests::FileItem* Blizzard::Streaming::ManifestInfo::GetFileItem(const std::string& name) {
    std::map<std::string, Manifests::FileItem>::iterator it = this->m_files.find(name);

    if (it == this->m_files.end()) {
        return NULL;
    }

    return &it->second;
}

// OFFSET: %0x007477F0
Blizzard::Streaming::Manifests::TransportItem* Blizzard::Streaming::ManifestInfo::GetTransportItem(const std::string& name) {
    std::map<std::string, Manifests::FileItem>::iterator it = Get()->m_files.find(name);

    if (it == Get()->m_files.end()) {
        return NULL;
    }

    return it->second.m_transport;
}

// OFFSET: %0x00747870
std::string Blizzard::Streaming::ManifestInfo::GetRemotePath(const std::string& name) {
    Manifests::FileItem* file = Get()->GetFileItem(name);

    if (!file) {
        return std::string();
    }

    std::string location = Get()->GetRandomServerLocation(file->m_validServers);
    return location + file->GetRemotePath();
}

// OFFSET: #0x00452E50
std::vector<std::string> Blizzard::Streaming::ManifestInfo::GetFileList(const char* path) {
    std::vector<std::string> files;

    char directory[260];
    Blizzard::File::MakeAbsolutePath(path, directory, sizeof(directory), false);

    char separator = Blizzard::String::FirstPathSeparator(directory);
    int32_t length = strlen(directory);
    char last = length > 0 ? directory[length - 1] : '\0';

    if (!length || last != separator) {
        if (last == '/' || last == '\\') {
            length--;
        }

        if (length > 258) {
            length = 258;
        }

        directory[length] = separator;
        directory[length + 1] = '\0';
    }

    char normalized[260];
    char* dst = normalized;
    const char* src = directory;
    uint32_t left = 260;

    while (true) {
        char c = *src++;
        left--;

        if (!c) {
            break;
        }

        *dst++ = c == '/' ? '\\' : c;

        if (!left) {
            dst--;
            break;
        }
    }

    *dst = '\0';

    for (std::map<std::string, Manifests::FileItem>::iterator it = Get()->m_files.begin(); it != Get()->m_files.end(); ++it) {
        if (it->second.m_size <= 0) {
            continue;
        }

        const char* localPath = it->second.m_localPath.c_str();
        char localDirectory[260];
        char fileName[260];

        if (localPath) {
            const char* name = localPath;

            for (const char* cursor = localPath; *cursor; cursor++) {
                if (*cursor == '/' || *cursor == '\\') {
                    name = cursor + 1;
                }
            }

            int32_t directoryLength = name - localPath + 1;
            Blizzard::String::Copy(localDirectory, localPath, directoryLength <= 260 ? directoryLength : 260);
            Blizzard::String::Copy(fileName, name, 260);
        } else {
            localDirectory[0] = '\0';
            fileName[0] = '\0';
        }

        if (!_stricmp(normalized, localDirectory)) {
            files.push_back(std::string(fileName));
        }
    }

    return files;
}

// OFFSET: %0x00747C60
std::string Blizzard::Streaming::ManifestInfo::GetBandwidthDataString() {
    std::string result = GetBandwidthString();

    for (std::map<std::string, Manifests::ServerItem>::iterator it = Get()->m_servers.begin(); it != Get()->m_servers.end(); ++it) {
        if (it->second.m_uses > 0) {
            result += Blizzard::String::QuickFormat<1024>(",%s,%d,%d", it->second.m_name.c_str(), it->second.m_uses, it->second.m_active != 0).m_buffer;
        }
    }

    return result;
}

// OFFSET: %0x00747E10
void Blizzard::Streaming::ManifestInfo::DeactivateServer(const std::string& url, const std::string& name) {
    if (!Get()->GetFileItem(name)) {
        return;
    }

    for (std::map<std::string, Manifests::ServerItem>::iterator it = Get()->m_servers.begin(); it != Get()->m_servers.end(); ++it) {
        uint32_t length = it->second.m_location.size();
        const char* location = it->second.m_location.c_str();

        if (location && !_strnicmp(url.c_str(), location, length) && it->second.m_canDeactivate >= 1) {
            it->second.m_active = false;
        }
    }
}

// OFFSET: %0x00747F40
int32_t Blizzard::Streaming::ManifestInfo::GetServerItemMaxRetry(const std::string& url, const std::string& name) {
    if (!Get()->GetFileItem(name)) {
        return 5;
    }

    for (std::map<std::string, Manifests::ServerItem>::iterator it = Get()->m_servers.begin(); it != Get()->m_servers.end(); ++it) {
        uint32_t length = it->second.m_location.size();
        const char* location = it->second.m_location.c_str();

        if (location && !_strnicmp(url.c_str(), location, length)) {
            return it->second.m_maxRetry;
        }
    }

    return 5;
}

// OFFSET: #0x004533E0
int32_t Blizzard::Streaming::FileManifest::GetBGPreloadSleep() {
    std::string value = ManifestInfo::Get()->GetParameter("bgpreloadsleep");

    if (!value.size()) {
        return 100;
    }

    int32_t sleep = atoi(value.c_str());

    if (sleep < 0) {
        sleep = 0;
    }

    return sleep;
}

// OFFSET: #0x00453480
bool Blizzard::Streaming::FileManifest::GetIsStreamingTrial() {
    std::string value = ManifestInfo::Get()->GetParameter("istrial");

    if (!value.size()) {
        return false;
    }

    return atoi(value.c_str()) != 0;
}

// OFFSET: #0x00453510
bool Blizzard::Streaming::FileManifest::SetSpeedTest(int32_t speedTest) {
    if (speedTest) {
        int32_t minimum = 1;
        int32_t maximum = 1000;
        int32_t sleep = 25000000 / speedTest;
        int32_t* value = sleep >= 1 ? &sleep : &minimum;

        if (*value > 1000) {
            value = &maximum;
        }

        ManifestInfo::Get()->SetParameter("bgpreloadsleep", *value);
    }

    return true;
}

// OFFSET: #0x004536B0
std::vector<std::string> Blizzard::Streaming::FileManifest::GetFileList(const char* path) {
    return ManifestInfo::GetFileList(path);
}

// OFFSET: #0x004536E0
void Blizzard::Streaming::FileManifest::SetDefaultBgPreloadSleep(int32_t sleep) {
    ManifestInfo::Get()->SetParameter("bgpreloadsleep", sleep);
}

// OFFSET: #0x00455D40
bool Blizzard::Streaming::Transport::ReadDirectFileFromBuffer(char* buffer, int32_t size) {
    char* line = buffer;

    if (!*line) {
        return true;
    }

    char* end = buffer + size;

    while (line < end) {
        char* value;
        char* tag;
        int32_t length;

        if (!SplitLine(line, &value, &tag, &length)) {
            Blizzard::File::AddToLastErrorStack(0, "Transport::ReadDirectFile - GetItemTag failed", 20);
            return false;
        }

        line += length;

        if (tag && (tag == "version" || !_strnicmp(tag, "version", 7))) {
            if (atol(value) != 1) {
                return false;
            }
        } else {
            TRANSPORT_ITEM item;
            SplitItem(value, &item);

            Manifests::TransportItem transport;
            transport.m_name = item.m_name;
            transport.m_md5Size = item.m_md5Size;
            transport.m_splitSize = item.m_splitSize;

            ManifestInfo::SetTransportItem(std::string(item.m_name), transport);
        }

        if (!*line) {
            return true;
        }
    }

    return true;
}

// OFFSET: %0x007561A0
bool Blizzard::Streaming::SourceManifest::Initialize(const char* path) {
    if (!path || !*path) {
        return true;
    }

    if (s_httpPrefix && !_strnicmp(path, s_httpPrefix, strlen(s_httpPrefix))) {
        return ReadIndirectFile(path);
    }

    return ReadDirectFile(path);
}

// OFFSET: %0x00759CC0
bool Blizzard::Streaming::ManifestInfo::ReadDirectFile(const char* path) {
    void* data;
    uint32_t size;

    if (!System_File::LoadFile(path, &data, &size, 4)) {
        Blizzard::File::AddToLastErrorStack(0, "Manifests::ReadDirectFile - LoadFile failed", 20);
        return false;
    }

    bool result = this->ReadDirectFileFromBuffer(static_cast<char*>(data), size);
    Blizzard::Memory::Free(data);

    if (!result) {
        Blizzard::File::AddToLastErrorStack(0, "Manifests::ReadDirectFile - ReadDirectFileFromBuffer failed", 20);
    }

    return result;
}

// OFFSET: %0x00750DB0
bool Blizzard::Streaming::FileManifest::ReadDirectFile(const char* path) {
    void* data;
    uint32_t size;

    if (!System_File::LoadFile(path, &data, &size, 4)) {
        Blizzard::File::AddToLastErrorStack(0, "FileManifest::ReadDirectFile - LoadFile failed", 20);
        return false;
    }

    bool result = ReadDirectFileFromBuffer(static_cast<char*>(data), size);
    Blizzard::Memory::Free(data);

    if (!result) {
        Blizzard::File::AddToLastErrorStack(0, "FileManifest::ReadDirectFile - ReadDirectFileFromBuffer failed", 20);
    }

    return result;
}

// OFFSET: #0x00456A00
bool Blizzard::Streaming::SourceManifest::ReadDirectFileFromBuffer(char* buffer, int32_t size) {
    std::string base;
    std::string path;

    char* line = buffer;

    if (!*line) {
        return true;
    }

    char* end = buffer + size;

    while (line < end) {
        char* value;
        char* tag;
        int32_t length;

        if (!SplitLine(line, &value, &tag, &length)) {
            Blizzard::Memory::Free(buffer);
            Blizzard::File::AddToLastErrorStack(0, "SourceManifest::ReadDirectfile - GetItemTag failed", 20);
            return false;
        }

        char* next = &line[length];

        if (tag && (tag == "version" || !_strnicmp(tag, "version", 7))) {
            if (atol(value) != 1) {
                return false;
            }
        } else if (tag && (tag == "source" || !_strnicmp(tag, "source", 6))) {
            if (FileManifest::SplitServerItem(value, base, path)) {
                Manifests::ServerItem server;
                server.m_name = base;
                server.m_location = base;
                ManifestInfo::SetServerItem(std::string(base), server);
                ManifestInfo::SetServerPathItem(std::string("old-version1-base"), std::string(path));
            }
        } else {
            MANIFEST_ITEM item;
            SplitItem(value, &item);

            char sourcePath[260];

            if (item.m_path.size() != static_cast<size_t>(-1)) {
                const char* src = item.m_path.c_str();

                if (src) {
                    char* dst = sourcePath;
                    char* limit = &sourcePath[item.m_path.size()];

                    while (dst < limit) {
                        char c = *src++;

                        if (!c) {
                            break;
                        }

                        *dst++ = c;
                    }

                    *dst = '\0';
                } else {
                    sourcePath[0] = '\0';
                }
            }

            if (FileManifest::SplitServerItem(sourcePath, base, path)) {
                ManifestInfo::SetServerPathItem(std::string(item.m_name), std::string(path));
            }
        }

        if (!*next) {
            return true;
        }

        line = next;
    }

    return true;
}

// OFFSET: %0x00754AE0
bool Blizzard::Streaming::Transport::ReadDirectFile(const char* path) {
    void* data;
    uint32_t size;

    if (!System_File::LoadFile(path, &data, &size, 4)) {
        Blizzard::File::AddToLastErrorStack(0, "Transport::ReadDirectFile - failed to load file", 20);
        return false;
    }

    bool result = ReadDirectFileFromBuffer(static_cast<char*>(data), size);
    Blizzard::Memory::Free(data);

    return result;
}

// OFFSET: %0x00754E50
bool Blizzard::Streaming::SourceManifest::ReadDirectFile(const char* path) {
    void* data;
    uint32_t size;

    if (!System_File::LoadFile(path, &data, &size, 4)) {
        Blizzard::File::AddToLastErrorStack(0, "SourceManifest::ReadDirectfile - failed to load file", 20);
        return false;
    }

    bool result = ReadDirectFileFromBuffer(static_cast<char*>(data), size);
    Blizzard::Memory::Free(data);

    return result;
}

// OFFSET: #0x0045D1E0
bool Blizzard::Streaming::FileManifest::ReadDirectFileFromBuffer(char* buffer, int32_t size) {
    char* line = buffer;

    if (!*line) {
        return true;
    }

    char* end = buffer + size;

    while (line < end) {
        char* value;
        char* tag;
        int32_t length;

        if (!SplitLine(line, &value, &tag, &length)) {
            Blizzard::File::AddToLastErrorStack(0, "FileManifest::ReadDirectFileFromBuffer - GetItemTag failed", 20);
            return false;
        }

        char* next = &line[length];
        bool isItem = !tag;

        if (isItem) {
        } else if (tag == "version" || !_strnicmp(tag, "version", 7)) {
            if (atol(value) != 1) {
                return false;
            }
        } else if (tag == "isTrial" || !_strnicmp(tag, "isTrial", 7)) {
            ManifestInfo::Get()->SetParameter("istrial", value);
        } else if (tag == "manifest" || !_strnicmp(tag, "manifest", 8)) {
            uint32_t prefixLength = s_httpPrefix ? strlen(s_httpPrefix) : 0;

            if (value && s_httpPrefix && !_strnicmp(value, s_httpPrefix, prefixLength)) {
                return ReadIndirectFile(value);
            }

            return ReadDirectFile(value);
        } else if (tag == "sourcemanifest" || !_strnicmp(tag, "sourcemanifest", 14)) {
            if (!SourceManifest::Initialize(value)) {
                Blizzard::File::AddToLastErrorStack(0, "FileManifest::ReadDirectFileFromBuffer - SourceManifest::Initialize failed", 20);
                return false;
            }
        } else if (tag == "transportmanifest" || !_strnicmp(tag, "transportmanifest", 17)) {
            if (value && *value) {
                bool result = _strnicmp(value, "http://", 7) ? Transport::ReadDirectFile(value) : Transport::ReadIndirectFile(value);

                if (!result) {
                    Blizzard::File::AddToLastErrorStack(0, "FileManifest::ReadDirectFileFromBuffer - Transport::Initialize failed", 20);
                    return false;
                }
            }
        } else if (tag == "source" || !_strnicmp(tag, "source", 6)) {
            std::string base;
            std::string path;

            if (SplitServerItem(value, base, path)) {
                Manifests::ServerItem server;
                server.m_name = base;
                server.m_location = base;
                ManifestInfo::SetServerItem(std::string(base), server);
                ManifestInfo::SetServerPathItem(std::string("old-version1-base"), std::string(path));
            }
        } else if (tag == "bgpreloadsleep" || !_strnicmp(tag, "bgpreloadsleep", 14)) {
            ManifestInfo::Get()->SetParameter("bgpreloadsleep", value);
        } else {
            isItem = true;
        }

        if (isItem) {
            MANIFEST_ITEM item;
            SplitItem(value, &item);

            if (!item.m_size) {
                if (Blizzard::File::Exists(item.m_absolutePath.c_str()) != 2) {
                    Blizzard::File::CreateDirectory(item.m_absolutePath.c_str(), true);
                }
            }

            std::string key = System_File::GetInode(item.m_path);

            Manifests::FileItem file;
            file.m_name = item.m_path;
            file.m_fileName = item.m_path;
            file.m_localPath = item.m_absolutePath;
            file.m_size = item.m_size;
            file.m_path.assign("old-version1-base", 17);
            file.m_fileVersion = item.m_fileVersion;
            file.m_transportItem = item.m_path;

            ManifestInfo::SetFileItem(std::string(key), file);
        }

        line = next;

        if (!*line) {
            return true;
        }
    }

    return true;
}

// OFFSET: #0x00461290
bool Blizzard::Streaming::ManifestInfo::ReadDirectFileFromBuffer(char* buffer, int32_t size) {
    Manifests::ManifestItem* item = NULL;
    char* line = buffer;

    if (!*line) {
        return true;
    }

    char* end = &buffer[size];

    while (line < end) {
        char* value;
        char* tag;
        int32_t length;

        if (!GetItemTag(line, &value, &tag, &length)) {
            Blizzard::File::AddToLastErrorStack(0, "ManifestInfo::ReadDirectFileFromBuffer - GetItemTag failed", 20);
            return false;
        }

        char* next = &line[length];

        if (tag && !_stricmp(tag, "version")) {
            this->m_version = atol(value);

            if (this->m_version == 1) {
                return FileManifest::ReadDirectFileFromBuffer(next, size + next - buffer);
            }
        } else if (tag && !_stricmp(tag, "manifest")) {
            if (!this->ReadFile(value)) {
                Blizzard::File::AddToLastErrorStack(0, "ManifestInfo::ReadDirectFileFromBuffer - manifest tag failed", 20);
                return false;
            }
        } else if (tag && !_stricmp(tag, "manifest_partial")) {
            std::string url = this->GetRandomServerLocation(std::vector<std::string>());
            url.append(value, strlen(value));

            if (!this->ReadFile(url.c_str())) {
                Blizzard::File::AddToLastErrorStack(0, "ManifestInfo::ReadDirectFileFromBuffer - manifest_partial tag failed", 20);
                return false;
            }
        } else if (!item || !item->HandleTag(tag, value)) {
            item = this->HandleBaseTag(tag, value);

            if (!item) {
                Blizzard::File::AddToLastErrorStack(0, "ManifestInfo::ReadDirectFileFromBuffer - Tag Parsing failed", 20);
                return false;
            }
        }

        if (!*next) {
            return true;
        }

        line = next;
    }

    return true;
}

// OFFSET: %0x0075A210
bool Blizzard::Streaming::ManifestInfo::ReadFile(const char* path) {
    if (path && s_httpPrefix && !_strnicmp(path, s_httpPrefix, s_httpPrefix ? strlen(s_httpPrefix) : 0)) {
        return this->ReadIndirectFile(path);
    }

    return this->ReadDirectFile(path);
}

// OFFSET: %0x0075A280
bool Blizzard::Streaming::ManifestInfo::Initialize(const char* path) {
    ManifestInfo* info = Get();

    if (!path) {
        return true;
    }

    bool result = info->ReadFile(path);

    if (!result) {
        return result;
    }

    return info->FinalizeManifestItems() & result;
}

// OFFSET: %0x00750B00
bool Blizzard::Streaming::ManifestInfo::ValidateMD5(const char* url, const void* data, uint32_t size) {
    char buffer[1024];
    const char* filename = Blizzard::String::FindFilename(url);

    if (!filename) {
        Blizzard::String::Format(buffer, sizeof(buffer), "%s - Warning, failed to find file name from %s", "Blizzard::Streaming::ManifestInfo::ValidateMD5", url);
        Blizzard::File::AddToLastErrorStack(0, buffer, 20);
        return true;
    }

    const char* dash = Blizzard::String::FindReverse(filename, '-', 0x7FFFFFFF);

    if (dash && dash != reinterpret_cast<const char*>(-1)) {
        char hex[33];
        char* dst = hex;
        const char* src = dash + 1;

        do {
            char c = *src++;

            if (!c) {
                break;
            }

            *dst++ = c;
        } while (dst < &hex[32]);

        *dst = '\0';

        if (dst - hex >= 32) {
            Blizzard::Util::Digest<16> expected;
            expected.FromHexString(hex);

            Blizzard::Util::MD5 md5;
            md5.Process(data, size);

            Blizzard::Util::Digest<16> actual;
            md5.Finish(actual.m_data);

            return expected == actual;
        }
    }

    Blizzard::String::Format(buffer, sizeof(buffer), "%s - Warning, failed to find MD5 from %s", "Blizzard::Streaming::ManifestInfo::ValidateMD5", url);
    Blizzard::File::AddToLastErrorStack(0, buffer, 20);

    return true;
}

// OFFSET: %0x007293E0
std::string Blizzard::Streaming::ManifestInfo::GetAbsolutePath(const std::string& path) {
    char absolute[260];

    if (!Blizzard::File::MakeAbsolutePath(path.c_str(), absolute, sizeof(absolute), false)) {
        return std::string();
    }

    char normalized[260];
    char* dst = normalized;
    const char* src = absolute;
    uint32_t left = 260;

    while (true) {
        char c = *src++;
        left--;

        if (!c) {
            *dst = '\0';
            break;
        }

        *dst++ = c == '/' ? '\\' : c;

        if (!left) {
            dst[-1] = '\0';
            break;
        }
    }

    return std::string(normalized);
}

// OFFSET: %0x00537E70
int32_t System_Streaming::HttpCallback::GetTimeout() {
    return this->m_timeout;
}

// PROVEN: d6978e4c IDENTICAL
// OFFSET: 0x007C3BD0
void System_Streaming::HttpCallback::SetTimeOutValue(uint32_t timeout) {
    this->m_timeout = timeout;
}

// OFFSET: #0x0043B060
bool System_Streaming::HttpCallback::ReceivedData(const void* data, int32_t size) {
    if (size > 0) {
        this->m_data.append(static_cast<const char*>(data), size);
        this->m_received += size;
    }

    return true;
}

// PROVEN: ebd8079b IDENTICAL
// OFFSET: 0x00A3AA30
bool System_Streaming::HttpCallback::ShouldBlock() {
    return true;
}
