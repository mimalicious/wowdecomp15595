#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMUSERCLIENT_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMUSERCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

struct JamUserClientReorderEntry {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ uint8_t m_newIndex;
    /* 0009 */ uint8_t unk_0009[7];

    JamUserClientReorderEntry();
    JamUserClientReorderEntry(const JamUserClientReorderEntry& source);
};

class UserClientEnumCharacters : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientEnumCharacters();
    ~UserClientEnumCharacters();
};

class UserClientPlayerLogin : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guid[8];

    void Put(CDataStore* msg) override;

    UserClientPlayerLogin(uint64_t guid);
    ~UserClientPlayerLogin();
};

class UserClientReorderCharacters : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<JamUserClientReorderEntry> m_entries;

    void Put(CDataStore* msg) override;

    UserClientReorderCharacters();
    ~UserClientReorderCharacters();
};

class UserClientGenerateRandomCharacterName : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_raceID;
    /* 0011 */ uint8_t m_sexID;

    void Put(CDataStore* msg) override;

    UserClientGenerateRandomCharacterName(uint8_t raceID, uint8_t sexID);
    ~UserClientGenerateRandomCharacterName();
};

#endif
