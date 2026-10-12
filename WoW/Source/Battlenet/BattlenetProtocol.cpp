#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include <cstring>

// STATIC: 0x00C83010/0x140A87C18
const uint32_t Protocol::TypeIdOf<Battlenet::Toon::FullName>::s_typeId = 412;

// STATIC: 0x00C83014
const uint32_t Protocol::TypeIdOf<Battlenet::Chat::MemberList>::s_typeId = 545;

// STATIC: 0x00C83018
const uint32_t Protocol::TypeIdOf<Battlenet::Event::Creep::Invoke>::s_typeId = 1282;

// STATIC: 0x00C8301C
const uint32_t Protocol::TypeIdOf<Battlenet::Event::Larva::Invoke>::s_typeId = 1838;

// STATIC: 0x00C83020
const uint32_t Protocol::TypeIdOf<Battlenet::Event::Update::Invoke>::s_typeId = 3158;

// STATIC: 0x00C83024
const uint32_t Protocol::TypeIdOf<Battlenet::Event::Header>::s_typeId = 3311;

// STATIC: 0x00C83028
const uint32_t Protocol::TypeIdOf<Battlenet::Friends::FriendInviteContainer>::s_typeId = 3409;

// STATIC: 0x00C8302C
const uint32_t Protocol::TypeIdOf<Battlenet::Friends::FriendList>::s_typeId = 3417;

// STATIC: 0x00C83030
const uint32_t Protocol::TypeIdOf<Battlenet::Friends::FriendsOfFriendList>::s_typeId = 3430;

// STATIC: 0x00C83034
const uint32_t Protocol::TypeIdOf<Battlenet::Presence::FieldVal>::s_typeId = 3675;

// STATIC: 0x00C83038
const uint32_t Protocol::TypeIdOf<Battlenet::Presence::FieldUpdate>::s_typeId = 3711;

// STATIC: 0x00C8303C
const uint32_t Protocol::TypeIdOf<Battlenet::Presence::BlockList>::s_typeId = 3715;

// STATIC: 0x01008E00
BSN::Protocol::Protocol Protocol::s_protocol(Protocol::s_bytecode, sizeof(Protocol::s_bytecode), Protocol::s_codeInfo, sizeof(Protocol::s_codeInfo) / sizeof(Protocol::s_codeInfo[0]));

// STATIC: 0x01008E38
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestGameAccount("RequestGameAccount");

// STATIC: 0x01008E80
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestSurveyInfo("RequestSurveyInfo");

// STATIC: 0x01008EC8
BSN::AsciiString<63> Battlenet::Event::AuthModule::FinalizeMatrixCard("FinalizeMatrixCard");

// STATIC: 0x01008F10
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestToken("RequestToken");

// STATIC: 0x01008F58
BSN::AsciiString<63> Battlenet::Event::AuthModule::CommitMatrixCard("CommitMatrixCard");

// STATIC: 0x01008FA0
BSN::AsciiString<63> Battlenet::Event::AuthModule::SurveyDialogComplete("SurveyDialogComplete");

// STATIC: 0x01008FE8
BSN::AsciiString<63> Battlenet::Event::AuthModule::DismissOutOfBandChallenge("DismissOutOfBandChallenge");

// STATIC: 0x01009030
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestMatrixCard("RequestMatrixCard");

// STATIC: 0x01009078
BSN::AsciiString<63> Battlenet::Event::AuthModule::DisplayAgreement("DisplayAgreement");

// STATIC: 0x010090C0
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestVirtualKeypadPIN("RequestVirtualKeypadPIN");

// STATIC: 0x01009108
BSN::AsciiString<63> Battlenet::Event::AuthModule::EnterMatrixCard("EnterMatrixCard");

// STATIC: 0x01009150
BSN::AsciiString<63> Battlenet::Event::AuthModule::DisplayOutOfBandChallenge("DisplayOutOfBandChallenge");

// STATIC: 0x01009198
BSN::AsciiString<63> Battlenet::Event::AuthModule::ClearGameAccounts("ClearGameAccounts");

// STATIC: 0x010091E0
BSN::AsciiString<63> Battlenet::Event::AuthModule::RevertMatrixCard("RevertMatrixCard");

// STATIC: 0x01009228
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestPassword("RequestPassword");

// STATIC: 0x01009270
BSN::AsciiString<63> Battlenet::Event::AuthModule::AddGameAccount("AddGameAccount");

// STATIC: 0x010092B8
BSN::AsciiString<63> Battlenet::Event::AuthModule::ShowSurveyDialog("ShowSurveyDialog");

// STATIC: 0x01009300
BSN::AsciiString<63> Battlenet::Event::AuthModule::AcceptAgreement("AcceptAgreement");

// STATIC: 0x01009344/0x140EE0DF8
BSN::FourCC Battlenet::InviteSource::MISC("Misc");

// STATIC: 0x01009350/0x140EE0E04
BSN::FourCC Battlenet::Program::WOW("WoW");

// OFFSET: 0x00627D80
Battlenet::Authentication::Settings::Settings() {
    this->m_seed.m_length = 0;
}

// OFFSET: 0x00B2E610/0x1408F6F70
int32_t ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort>::Encode(const Battlenet::IP4::AddressPort& value, BSN::Hard::Encoder& encoder) {
    int32_t result = encoder.PutAlignedData(4, value.m_address.m_data);

    if (result) {
        return result;
    }

    return encoder.PutAlignedData(2, value.m_port.m_data);
}

// OFFSET: 0x00B2E640/0x1408F6FB0
int32_t ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort>::Decode(Battlenet::IP4::AddressPort& value, BSN::Hard::Decoder& decoder) {
    int32_t result = decoder.GetAlignedData(4, value.m_address.m_data);

    if (result) {
        return result;
    }

    return decoder.GetAlignedData(2, value.m_port.m_data);
}

// OFFSET: 0x00B2E670/0x1408F6FF0
int32_t ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort>::Encode(const Battlenet::IP4::AddressPort& value, BSN::Hard::SizeEncoder& encoder) {
    encoder.m_size = ((((encoder.m_size + 7) & ~7u) + 32) & ~7u) + 16;
    return BSN::CODEC_VALID;
}

// OFFSET: 0x00B2E690/0x1408F7010
int32_t ProtocolHard::TypeInfoOf<Battlenet::Client::DNSInfo>::Encode(const Battlenet::Client::DNSInfo& value, BSN::Hard::Encoder& encoder) {
    uint32_t length = value.m_canonicalName.m_length;

    if (length > 0xFF) {
        return BSN::CODEC_INVALID;
    }

    int32_t result = encoder.PutBits(8, length);

    if (result) {
        return result;
    }

    result = encoder.PutAsciiData(length, value.m_canonicalName.m_data);

    if (result) {
        return result;
    }

    uint32_t count = value.m_addresses.m_count;

    if (count > 0x7F) {
        return BSN::CODEC_INVALID;
    }

    result = encoder.PutBits(7, count);

    if (result) {
        return result;
    }

    for (uint32_t i = 0; i < count; i++) {
        result = encoder.PutAlignedData(4, value.m_addresses.m_data[i].m_data);

        if (result) {
            return result;
        }
    }

    return BSN::CODEC_VALID;
}

// OFFSET: 0x00B2E840/0x1408F7230
int32_t ProtocolHard::TypeInfoOf<Battlenet::Client::DNSInfo>::Encode(const Battlenet::Client::DNSInfo& value, BSN::Hard::SizeEncoder& encoder) {
    uint32_t length = value.m_canonicalName.m_length;

    if (length > 0xFF) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 8;

    if (!BSN::IsValidAscii(length, value.m_canonicalName.m_data)) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size = ((encoder.m_size + 7) & ~7u) + 8 * length;

    uint32_t count = value.m_addresses.m_count;

    if (count > 0x7F) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 7;

    for (; count; count--) {
        encoder.m_size = ((encoder.m_size + 7) & ~7u) + 32;
    }

    return BSN::CODEC_VALID;
}

// OFFSET: 0x00B2E8C0/0x1408F72E0
int32_t ProtocolHard::TypeInfoOf<Battlenet::Version::Record>::Encode(const Battlenet::Version::Record& value, BSN::Hard::Encoder& encoder) {
    int32_t result = encoder.PutBits(32, value.m_version);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_programId.m_value);

    if (result) {
        return result;
    }

    return encoder.PutUInt32(value.m_component.m_value);
}

// OFFSET: 0x00B2E9A0/0x1408F7400
int32_t ProtocolHard::TypeInfoOf<Battlenet::Authentication::Settings>::Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::Encoder& encoder) {
    uint32_t checksum = 7;
    int32_t result = encoder.PutUInt32(value.m_platform.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutChecksum(13, checksum);

    if (result) {
        return result;
    }

    bool exists = value.m_cacheOverridePath.m_exists;
    result = encoder.PutBool(exists);

    if (result) {
        return result;
    }

    if (exists) {
        uint32_t size = value.m_cacheOverridePath.m_value.m_length;

        if (size > 0x3E0) {
            return BSN::CODEC_INVALID;
        }

        result = encoder.PutBits(10, size);

        if (result) {
            return result;
        }

        result = encoder.PutAlignedData(size, reinterpret_cast<const uint8_t*>(value.m_cacheOverridePath.m_value.m_data));

        if (result) {
            return result;
        }
    }

    result = encoder.PutUInt32(value.m_productLocale.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_textLocale.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_program.m_value);

    if (result) {
        return result;
    }

    uint32_t seedLength = value.m_seed.m_length;

    if (seedLength > 0x3FF) {
        return BSN::CODEC_INVALID;
    }

    result = encoder.PutBits(10, seedLength);

    if (result) {
        return result;
    }

    result = encoder.PutAlignedData(seedLength, value.m_seed.m_data);

    if (result) {
        return result;
    }

    result = encoder.PutChecksum(8, checksum);

    if (result) {
        return result;
    }

    uint32_t count = value.m_versions.m_count;

    if (count > 0x3F) {
        return BSN::CODEC_INVALID;
    }

    result = encoder.PutBits(6, count);

    if (result) {
        return result;
    }

    for (uint32_t i = 0; i < count; i++) {
        result = ProtocolHard::TypeInfoOf<Battlenet::Version::Record>::Encode(value.m_versions.m_data[i], encoder);

        if (result) {
            return result;
        }
    }

    return BSN::CODEC_VALID;
}

// OFFSET: 0x00B2ECD0/0x1408F77A0
int32_t ProtocolHard::TypeInfoOf<Battlenet::Authentication::Settings>::Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::SizeEncoder& encoder) {
    encoder.m_size += 45;
    bool exists = value.m_cacheOverridePath.m_exists;
    encoder.m_size += 1;

    if (exists) {
        uint32_t size = value.m_cacheOverridePath.m_value.m_length;

        if (size > 0x3E0) {
            return BSN::CODEC_INVALID;
        }

        encoder.m_size += 10;
        encoder.m_size = ((encoder.m_size + 7) & ~7u) + 8 * size;
    }

    encoder.m_size += 96;

    uint32_t seedLength = value.m_seed.m_length;

    if (seedLength > 0x3FF) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 10;
    encoder.m_size = ((encoder.m_size + 7) & ~7u) + 8 * seedLength + 8;

    uint32_t count = value.m_versions.m_count;

    if (count > 0x3F) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 6;

    for (uint32_t i = 0; i < count; i++) {
        encoder.m_size += 96;
    }

    return BSN::CODEC_VALID;
}
