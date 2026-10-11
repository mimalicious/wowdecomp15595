#ifndef WOW_SOURCE_BATTLENET_BATTLENETPROTOCOL_H
#define WOW_SOURCE_BATTLENET_BATTLENETPROTOCOL_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/Battlenet.h"
#include "WoW/Source/Battlenet/BSN/Encoder.h"
#include "WoW/Source/Battlenet/BSN/Protocol.h"
#include "WoW/Source/Battlenet/BattlenetProtocolTypes.h"

namespace Battlenet {
namespace InviteSource {
extern BSN::FourCC MISC;
}

namespace Program {
extern BSN::FourCC WOW;
}

namespace Event {
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
extern BSN::AsciiString<63> DisplayOutOfBandChallenge;
extern BSN::AsciiString<63> DismissOutOfBandChallenge;
}
}
}

class Protocol {
    public:
    template <class T>
    struct TypeIdOf;

    static const uint8_t s_bytecode[0x30D56];
    static const void* const s_codeInfo[0x2D2C];
    static BSN::Protocol::Protocol s_protocol;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Header> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Creep::Invoke> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Larva::Invoke> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Event::Update::Invoke> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Chat::MemberList> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Presence::FieldVal> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Presence::BlockList> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Presence::FieldUpdate> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Toon::FullName> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendsOfFriendList> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendInviteContainer> {
    static const uint32_t s_typeId;
};

template <>
struct Protocol::TypeIdOf<Battlenet::Friends::FriendList> {
    static const uint32_t s_typeId;
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
struct ProtocolHard::TypeInfoOf<Battlenet::Version::Record> {
    static int32_t Encode(const Battlenet::Version::Record& value, BSN::Hard::Encoder& encoder);
};

template <>
struct ProtocolHard::TypeInfoOf<Battlenet::Authentication::Settings> {
    static int32_t Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::Encoder& encoder);
    static int32_t Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::SizeEncoder& encoder);
};

#endif
