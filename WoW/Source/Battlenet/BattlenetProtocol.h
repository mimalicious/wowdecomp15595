#ifndef WOW_SOURCE_BATTLENET_BATTLENETPROTOCOL_H
#define WOW_SOURCE_BATTLENET_BATTLENETPROTOCOL_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/Battlenet.h"
#include "WoW/Source/Battlenet/BSN/Encoder.h"
#include "WoW/Source/Battlenet/BSN/Protocol.h"

namespace Battlenet {
namespace Authentication {
struct Component {
    /* 0000 */ BSN::FourCC m_program;
    /* 0004 */ BSN::FourCC m_platform;
    /* 0008 */ uint32_t m_version;
};

struct Settings {
    /* 0000 */ uint32_t m_seedLength;
    /* 0004 */ uint8_t m_seed[1024];
    /* 0404 */ BSN::FourCC m_program;
    /* 0408 */ BSN::FourCC m_platform;
    /* 040C */ BSN::FourCC m_locale;
    /* 0410 */ BSN::FourCC m_systemLocale;
    /* 0414 */ BSN::Array<Component, 63> m_components;

    Settings();
};

struct ModuleParam : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x40C];

    ModuleParam();
};

struct ModuleMessage {
    /* 0000 */ uint32_t m_moduleId;
    /* 0004 */ BSN::AsciiString<63> m_name;
    /* 0048 */ BSN::Array<ModuleParam, 4> m_params;
};

struct ModuleRequest {
    /* 0000 */ BSN::AsciiString<63> m_name;
    /* 0044 */ uint32_t m_moduleId;
    /* 0048 */ BSN::Array<ModuleParam, 4> m_params;
};

struct Complete {
    /* 0000 */ uint8_t unk_0000[284];
    /* 011C */ BSN::AsciiString<39> m_accountName;
    /* 0148 */ uint32_t m_accountFlags;
    /* 014C */ uint32_t m_accountFlags2;
};

struct Failure {
    /* 0000 */ uint16_t m_error;
};

struct Request : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x108C];

    Request();
};

struct Response : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x108C];

    Response();
};
}

namespace Service {
struct State : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x4];

    State();
};
}

namespace Realm {
struct Address {
    /* 0000 */ uint8_t m_region;
    /* 0001 */ uint8_t m_battlegroup;
    /* 0004 */ uint32_t m_id;
};

struct CharacterCount {
    /* 0000 */ uint8_t m_region;
    /* 0001 */ uint8_t m_battlegroup;
    /* 0004 */ uint32_t m_id;
    /* 0008 */ uint16_t m_count;
};

struct Build {
    /* 0000 */ BSN::AsciiString<23> m_version;
    /* 001C */ uint32_t m_build;
    /* 0020 */ IP4::AddressPort m_address;
};

struct Info {
    /* 0000 */ uint8_t m_flags;
    /* 0004 */ BSN::String<255> m_name;
    /* 040C */ uint32_t m_type;
    /* 0410 */ uint32_t m_category;
    /* 0414 */ uint32_t m_otherFlags;
    /* 0418 */ float m_population;
    /* 041C */ BSN::Optional<Build> m_build;
};

struct InfoUpdate : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x448];

    InfoUpdate();
};

struct Update {
    /* 0000 */ uint8_t m_region;
    /* 0001 */ uint8_t m_battlegroup;
    /* 0004 */ uint32_t m_id;
    /* 0008 */ InfoUpdate m_info;
};

struct ListRequest {
    /* 0000 */ bool m_subscribe;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
};

struct Request : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0xC];

    Request();
};

struct List : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x304];

    List();
};

struct JoinInfo {
    /* 0000 */ BSN::FixedArray<uint8_t, 40> m_sessionKey;
    /* 0028 */ IP4::AddressPort m_address;
};

struct JoinResponse : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x30];

    JoinResponse();
};

struct Response : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x454];

    Response();
};
}

namespace Program {
extern BSN::FourCC WOW;
}

namespace Toon {
struct FullName {
    static const BSN::FourCC s_default_programId;
    static const BSN::String<25> s_default_name;

    static void SetDefault_region(uint8_t& value);
    static void SetDefault_programId(BSN::FourCC& value);
    static void SetDefault_realm(uint32_t& value);
    static void SetDefault_name(BSN::String<25>& value);

    /* 0000 */ uint8_t m_region;
    /* 0004 */ BSN::FourCC m_program;
    /* 0008 */ uint32_t m_realm;
    /* 000C */ BSN::String<25> m_name;
};
}

namespace Presence {
struct AccountName {
    /* 0000 */ BSN::String<32> m_givenName;
    /* 008C */ BSN::String<32> m_surname;
};

struct FieldVal : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x208];

    FieldVal();
};

struct FieldUpdate {
    /* 0000 */ uint32_t m_presenceId;
    /* 0004 */ uint32_t m_field;
    /* 0008 */ FieldVal m_value;
};

struct BlockList {
    /* 0000 */ BSN::Array<uint32_t, 100> m_accounts;
    /* 0194 */ BSN::Array<uint32_t, 100> m_toons;
};
}

namespace Chat {
struct MemberList {
    /* 0000 */ BSN::Array<uint32_t, 12> m_members;
};
}

namespace Friends {
struct FriendInvite {
    /* 0000 */ uint32_t m_presenceId;
    /* 0004 */ BSN::Optional<BSN::String<127>> m_message;
    /* 0210 */ uint8_t unk_0210;
    /* 0214 */ int32_t m_time;

    FriendInvite& operator=(const FriendInvite& other);
};

struct FriendInviteContainer {
    /* 0000 */ BSN::Array<FriendInvite, 100> m_invites;
};

struct FriendList {
    /* 0000 */ BSN::Array<uint32_t, 100> m_friends;
    /* 0194 */ BSN::Array<uint32_t, 100> unk_0194;
};

struct FriendsOfFriendList {
    /* 0000 */ uint32_t m_presenceId;
    /* 0004 */ BSN::Array<uint32_t, 100> m_friends;
};
}

namespace Event {
namespace Creep {
struct Portal : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x24];

    Portal();
};

struct Logoff {
    /* 0000 */ uint32_t unk_0000[3];
};

struct Invoke : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0xBEFF0];

    Invoke();
};
}

namespace Larva {
struct Invoke : public BSN::Choice {
    /* 0004 */ uint8_t m_value[0x27124];

    Invoke();
};
}

namespace Update {
struct Invoke {
    /* 0000 */ BSN::FourCC m_program;
    /* 0004 */ BSN::FourCC m_component;
    /* 0008 */ BSN::AsciiString<255> m_patch;
    /* 010C */ uint32_t unk_010C;
};
}

namespace AuthModule {
extern BSN::AsciiString<63> RequestGameAccount;
extern BSN::AsciiString<63> ShowSurveyDialog;
extern BSN::AsciiString<63> FinalizeMatrixCard;
extern BSN::AsciiString<63> RequestToken;
extern BSN::AsciiString<63> CommitMatrixCard;
extern BSN::AsciiString<63> SurveyDialogComplete;
extern BSN::AsciiString<63> RequestMatrixCard;
extern BSN::AsciiString<63> AcceptAgreement;
extern BSN::AsciiString<63> RequestVirtualKeypadPIN;
extern BSN::AsciiString<63> EnterMatrixCard;
extern BSN::AsciiString<63> ClearGameAccounts;
extern BSN::AsciiString<63> RevertMatrixCard;
extern BSN::AsciiString<63> RequestPassword;
extern BSN::AsciiString<63> AddGameAccount;
extern BSN::AsciiString<63> RequestSurveyInfo;
extern BSN::AsciiString<63> DisplayAgreement;
}
}
}

class Protocol {
    public:
    template <class T>
    struct TypeIdOf;

    static const uint8_t s_bytecode[0x2CA4E];
    static const void* const s_codeInfo[0x28D9];
    static BSN::Protocol::Protocol s_protocol;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Header> {
    static const uint32_t s_typeId = 3037;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Creep::Invoke> {
    static const uint32_t s_typeId = 1137;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Larva::Invoke> {
    static const uint32_t s_typeId = 1672;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Update::Invoke> {
    static const uint32_t s_typeId = 2901;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Chat::MemberList> {
    static const uint32_t s_typeId = 466;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendInvite> {
    static const uint32_t s_typeId = 3086;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Presence::FieldVal> {
    static const uint32_t s_typeId = 3329;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Presence::BlockList> {
    static const uint32_t s_typeId = 3368;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Presence::FieldUpdate> {
    static const uint32_t s_typeId = 3364;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Toon::FullName> {
    static const uint32_t s_typeId = 371;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendsOfFriendList> {
    static const uint32_t s_typeId = 3105;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendInviteContainer> {
    static const uint32_t s_typeId = 3092;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendList> {
    static const uint32_t s_typeId = 3097;
};

class ProtocolHard {
    public:
    template <class T>
    struct TypeInfoOf;
};

template <>
struct ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort> {
    static int32_t Encode(const Battlenet::IP4::AddressPort& value, BSN::Hard::Encoder& encoder);
    static int32_t Encode(const Battlenet::IP4::AddressPort& value, BSN::Hard::SizeEncoder& encoder);
    static int32_t Decode(Battlenet::IP4::AddressPort& value, BSN::Hard::Decoder& decoder);
};

template <>
struct ProtocolHard::TypeInfoOf<Battlenet::Client::DNSInfo> {
    static int32_t Encode(const Battlenet::Client::DNSInfo& value, BSN::Hard::Encoder& encoder);
    static int32_t Encode(const Battlenet::Client::DNSInfo& value, BSN::Hard::SizeEncoder& encoder);
};

template <>
struct ProtocolHard::TypeInfoOf<Battlenet::Authentication::Component> {
    static int32_t Encode(const Battlenet::Authentication::Component& value, BSN::Hard::Encoder& encoder);
};

template <>
struct ProtocolHard::TypeInfoOf<Battlenet::Authentication::Settings> {
    static int32_t Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::Encoder& encoder);
    static int32_t Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::SizeEncoder& encoder);
};

#endif
