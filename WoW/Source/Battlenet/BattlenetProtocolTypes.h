#ifndef WOW_SOURCE_BATTLENET_BATTLENETPROTOCOLTYPES_H
#define WOW_SOURCE_BATTLENET_BATTLENETPROTOCOLTYPES_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "WoW/Source/Battlenet/BSN/Protocol.h"
#include "WoW/Source/Battlenet/BSN/Types.h"

#ifdef OPTIONAL
#undef OPTIONAL
#endif

namespace Battlenet { namespace Achievement { namespace CriteriaModifierOperand {
enum Enum {
    AND = 0,
    OR = 1,
};
}}}

namespace Battlenet { namespace Attribute { namespace Access {
enum Enum {
    NO_ONE = 0,
    SELF = 1,
    SPECIAL = 2,
    EVERYONE = 3,
};
}}}

namespace Battlenet { namespace Attribute { namespace Arbitration {
enum Enum {
    NONE = 0,
    FCFS = 1,
};
}}}

namespace Battlenet { namespace Attribute { namespace Scope {
enum Enum {
    SLOT1 = 0,
    SLOT2 = 1,
    SLOT3 = 2,
    SLOT4 = 3,
    SLOT5 = 4,
    SLOT6 = 5,
    SLOT7 = 6,
    SLOT8 = 7,
    SLOT9 = 8,
    SLOT10 = 9,
    SLOT11 = 10,
    SLOT12 = 11,
    SLOT13 = 12,
    SLOT14 = 13,
    SLOT15 = 14,
    SLOT16 = 15,
    MAX_SLOT = 15,
    NUM_SLOTS = 16,
    NUM_UNUSABLE_SLOTS = 0,
    GAME = 20,
    SLOT = 21,
    NONE = 22,
};
}}}

namespace Battlenet { namespace Attribute { namespace Visibility {
enum Enum {
    NO_ONE = 0,
    SELF = 1,
    SPECIAL = 2,
    EVERYONE = 3,
};
}}}

namespace Battlenet { namespace Cache { namespace StreamDirection {
enum Enum {
    BEFORE = 0,
    AFTER = 1,
};
}}}

namespace Battlenet { namespace League { namespace League {
enum Enum {
    BRONZE = 0,
    SILVER = 1,
    GOLD = 2,
    PLATINUM = 3,
    DIAMOND = 4,
    MASTER = 5,
    GRANDMASTER = 6,
    TOTAL_COUNT = 7,
};
}}}

namespace Battlenet { namespace League { namespace LeagueAssignmentReason {
enum Enum {
    INITIAL = 0,
    PROMOTION = 1,
    DEMOTION = 2,
};
}}}

namespace Battlenet { namespace League { namespace SelfEvaluation {
enum Enum {
    TRAINEE = 1,
    EXPERIENCED = 2,
    EXPERT = 3,
};
}}}

namespace Battlenet { namespace Result { namespace Form {
enum Enum {
    SCORE_TEMPLATE = 0,
    SUMMARY_TEMPLATE = 1,
    HEADING = 2,
    VALUE = 3,
    EVENT = 4,
    BUILD_ORDER_TEMPLATE = 5,
    COMMENT = 6,
    COMMENT_TEMPLATE = 7,
    CURRENT_LADDER_TEMPLATE = 8,
    VALUE_PANE_4_TEMPLATE = 9,
    GRAPH_TEMPLATE = 10,
};
}}}

namespace Battlenet { namespace S2Game { namespace PlayerResultCode {
enum Enum {
    WIN = 0,
    LOSS = 1,
    TIE = 2,
    UNDECIDED = 3,
    BAILER = 4,
    DISAGREE = 5,
    WATCHER = 6,
};
}}}

namespace Battlenet { namespace S2Map { namespace LinkEntryType {
enum Enum {
    PRIMARY = 0,
    OPTIONAL = 1,
    DEPENDENCY = 2,
};
}}}

namespace Battlenet { namespace S2Map { namespace MapFilterType {
enum Enum {
    EVERYTHING = 0,
    CATEGORY = 1,
};
}}}

namespace Battlenet { namespace S2Master { namespace AdvertPostJoinedOrCreated {
enum Enum {
    JOINED = 1,
    CREATED = 2,
};
}}}

namespace Battlenet { namespace S2Master { namespace AdvertPostMode {
enum Enum {
    JOIN_PUBLIC = 1,
    JOIN_OR_CREATE_PUBLIC = 2,
    CREATE_PUBLIC = 3,
    CREATE_PRIVATE = 4,
    RANDOM_HOT_OR_NOT = 5,
};
}}}

namespace Battlenet { namespace S2Master { namespace OGLCriticId {
enum Enum {
    BAD_ID = 0,
    POPULARITY = 1,
    FEATURED = 2,
    UP_AND_COMING = 3,
    HOT_OR_NOT_RAW = 4,
    HOT_OR_NOT_WARM = 5,
    NEW_MAPS = 6,
};
}}}

namespace Battlenet { namespace S2Master { namespace OGLFilterType {
enum Enum {
    EVERYTHING = 0,
    CATEGORY = 1,
    MODE = 2,
    SPEED = 3,
    TRIAL = 4,
};
}}}

namespace Battlenet { namespace Statistics { namespace CategoryEnum {
enum Enum {
    USERS = 0,
    USERS_REGION = 1,
    USERS_PROGRAM = 2,
    USERS_PROGRAM_REGION = 3,
    GAMES = 4,
    GAMES_REGION = 5,
    GAMES_PROGRAM = 6,
    GAMES_PROGRAM_REGION = 7,
};
}}}

namespace Battlenet { namespace WoW { namespace AuthResult {
enum Enum {
    WOW_SUCCESS = 0,
    WOW_FAIL_BANNED = 3,
    WOW_FAIL_UNKNOWN_ACCOUNT = 4,
    WOW_FAIL_INCORRECT_PASSWORD = 5,
    WOW_FAIL_ALREADY_ONLINE = 6,
    WOW_FAIL_NO_TIME = 7,
    WOW_FAIL_DB_BUSY = 8,
    WOW_FAIL_VERSION_INVALID = 9,
    WOW_FAIL_VERSION_UPDATE = 10,
    WOW_FAIL_INVALID_SERVER = 11,
    WOW_FAIL_SUSPENDED = 12,
    WOW_FAIL_FAIL_NOACCESS = 13,
    WOW_SUCCESS_SURVEY = 14,
    WOW_FAIL_PARENTCONTROL = 15,
    WOW_FAIL_LOCKED_ENFORCED = 16,
    WOW_FAIL_TRIAL_ENDED = 17,
    WOW_FAIL_OVERMIND_CONVERTED = 18,
    WOW_FAIL_ANTI_INDULGENCE = 19,
    WOW_FAIL_EXPIRED = 20,
    WOW_FAIL_NO_GAME_ACCOUNT = 21,
    WOW_FAIL_BILLING_LOCK = 22,
    WOW_FAIL_IGR_WITHOUT_BNET = 23,
    WOW_FAIL_AA_LOCK = 24,
    WOW_FAIL_UNLOCKABLE_LOCK = 25,
    WOW_FAIL_MUST_USE_BNET = 26,
    WOW_FAIL_OTHER = 255,
};
}}}

namespace Battlenet { namespace Account {
struct FullName {
    public:
    static const BSN::String<32> s_default_givenName;
    static const BSN::String<32> s_default_surname;

    static void SetDefault_givenName(BSN::String<32>& value);
    static void SetDefault_surname(BSN::String<32>& value);

    /* 0000 */ BSN::String<32> m_givenName;
    /* 008C */ BSN::String<32> m_surname;
};
}}

namespace Battlenet { namespace Achievement {
struct PersistentRecord {
    public:
    static void SetDefault_achievementId(uint64_t& value);
    static void SetDefault_completion(int32_t& value);

    /* 0000 */ uint64_t m_achievementId;
    /* 0008 */ int32_t m_completion;
};
}}

namespace Battlenet { namespace Achievement {
struct CriteriaModifier {
    public:
    static const BSN::FourCC s_default_target;

    static void SetDefault_necessaryCondition(uint32_t& value);
    static void SetDefault_target(BSN::FourCC& value);
    static void SetDefault_operand(Battlenet::Achievement::CriteriaModifierOperand::Enum& value);
    static void SetDefault_comparand(uint64_t& value);

    /* 0000 */ uint32_t m_necessaryCondition;
    /* 0004 */ BSN::FourCC m_target;
    /* 0008 */ Battlenet::Achievement::CriteriaModifierOperand::Enum m_operand;
    /* 0010 */ uint64_t m_comparand;
};
}}

namespace Battlenet { namespace Achievement {
struct CriteriaEvent {
    public:
    static void SetDefault_id(uint32_t& value);
    static void SetDefault_comparand(BSN::Optional<uint64_t>& value);
    static void SetDefault_modifier(BSN::Array<Battlenet::Achievement::CriteriaModifier, 32>& value);

    /* 0000 */ uint32_t m_id;
    /* 0008 */ BSN::Optional<uint64_t> m_comparand;
    /* 0018 */ BSN::Array<Battlenet::Achievement::CriteriaModifier, 32> m_modifier;
};
}}

namespace Battlenet { namespace Achievement {
struct CriteriaStartEvent : public Battlenet::Achievement::CriteriaEvent {
    public:
    static void SetDefault_CriteriaEvent(Battlenet::Achievement::CriteriaEvent& value);
    static void SetDefault_timerDuration(BSN::Optional<int32_t>& value);

    /* 0320 */ BSN::Optional<int32_t> m_timerDuration;
};
}}

namespace Battlenet { namespace Achievement {
struct CriteriaUpdateRecord {
    public:
    static void SetDefault_criteriaId(uint64_t& value);
    static void SetDefault_startTime(BSN::Optional<int32_t>& value);
    static void SetDefault_quantity(BSN::Optional<uint64_t>& value);
    static void SetDefault_flags(uint32_t& value);

    /* 0000 */ uint64_t m_criteriaId;
    /* 0008 */ BSN::Optional<int32_t> m_startTime;
    /* 0010 */ BSN::Optional<uint64_t> m_quantity;
    /* 0020 */ uint32_t m_flags;
};
}}

namespace Battlenet { namespace Achievement {
struct TrackedCriteria {
    public:
    static void SetDefault_criteriaId(uint64_t& value);
    static void SetDefault_baseQuantity(uint64_t& value);
    static void SetDefault_deltaQuantity(uint64_t& value);
    static void SetDefault_pendingQuantity(uint64_t& value);
    static void SetDefault_startTime(BSN::Optional<int32_t>& value);
    static void SetDefault_flags(uint32_t& value);

    /* 0000 */ uint64_t m_criteriaId;
    /* 0008 */ uint64_t m_baseQuantity;
    /* 0010 */ uint64_t m_deltaQuantity;
    /* 0018 */ uint64_t m_pendingQuantity;
    /* 0020 */ BSN::Optional<int32_t> m_startTime;
    /* 0028 */ uint32_t m_flags;
};
}}

namespace Battlenet { namespace Achievement {
struct ImageHandle {
    public:
    static void SetDefault_offset(uint8_t& value);
    static void SetDefault_textureFileIndex(uint8_t& value);

    /* 0000 */ uint8_t m_offset;
    /* 0001 */ uint8_t m_textureFileIndex;
};
}}

namespace Battlenet { namespace Achievement {
struct ProgramEvaluatorTuple {
    public:
    static const BSN::FourCC s_default_program;
    static const BSN::FourCC s_default_evaluator;

    static void SetDefault_program(BSN::FourCC& value);
    static void SetDefault_evaluator(BSN::FourCC& value);

    /* 0000 */ BSN::FourCC m_program;
    /* 0004 */ BSN::FourCC m_evaluator;
};
}}

namespace Battlenet { namespace Achievement {
struct ProgramHandleAggregation {
    public:
    static const BSN::FourCC s_default_program;
    static const BSN::FixedBlob<40> s_default_handle;

    static void SetDefault_program(BSN::FourCC& value);
    static void SetDefault_handle(BSN::FixedBlob<40>& value);

    /* 0000 */ BSN::FourCC m_program;
    /* 0004 */ BSN::FixedBlob<40> m_handle;
};
}}

namespace Battlenet { namespace Attribute {
struct DefinitionHandle {
    public:
    static void SetDefault_nameSpace(uint32_t& value);
    static void SetDefault_id(uint32_t& value);

    /* 0000 */ uint32_t m_nameSpace;
    /* 0004 */ uint32_t m_id;
};
}}

namespace Battlenet { namespace Attribute {
struct VisibilityOverride {
    public:
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_hidden(bool& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ bool m_hidden;
};
}}

namespace Battlenet { namespace Attribute {
struct IndexRestriction {
    public:
    struct Data : public BSN::Choice {
        public:
        struct AllSlots {
            public:
            static void SetDefault_byIndex(BSN::BitArray<32>& value);

            /* 0000 */ BSN::BitArray<32> m_byIndex;
        };

        struct PerSlot {
            public:
            static void SetDefault_bySlot(BSN::Array<BSN::BitArray<32>, 16>& value);

            /* 0000 */ BSN::Array<BSN::BitArray<32>, 16> m_bySlot;
        };

        /* 0004 */ uint8_t m_value[132];

        Data();
    };

    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_data(Battlenet::Attribute::IndexRestriction::Data& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ Battlenet::Attribute::IndexRestriction::Data m_data;
};
}}

namespace Battlenet { namespace Permission {
struct Handle {
    public:
    static const BSN::FourCC s_default_scope;

    static void SetDefault_scope(BSN::FourCC& value);
    static void SetDefault_id(uint64_t& value);

    /* 0000 */ BSN::FourCC m_scope;
    /* 0008 */ uint64_t m_id;
};
}}

namespace Battlenet { namespace Attribute {
struct PermissionRestriction {
    public:
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_indexes(BSN::BitArray<32>& value);
    static void SetDefault_requiredPermissions(BSN::Array<Battlenet::Permission::Handle, 16>& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ BSN::BitArray<32> m_indexes;
    /* 0010 */ BSN::Array<Battlenet::Permission::Handle, 16> m_requiredPermissions;
};
}}

namespace Battlenet { namespace Attribute {
struct ArtId {
    public:
    static void SetDefault_visualFileIndex(uint8_t& value);
    static void SetDefault_top(uint16_t& value);
    static void SetDefault_left(uint16_t& value);
    static void SetDefault_height(uint16_t& value);
    static void SetDefault_width(uint16_t& value);

    /* 0000 */ uint8_t m_visualFileIndex;
    /* 0002 */ uint16_t m_top;
    /* 0004 */ uint16_t m_left;
    /* 0006 */ uint16_t m_height;
    /* 0008 */ uint16_t m_width;
};
}}

namespace Battlenet { namespace Attribute {
struct LocalizedString {
    public:
    static void SetDefault_colorHint(BSN::Optional<uint32_t>& value);
    static void SetDefault_fileIndex(uint8_t& value);
    static void SetDefault_id(uint16_t& value);

    /* 0000 */ BSN::Optional<uint32_t> m_colorHint;
    /* 0008 */ uint8_t m_fileIndex;
    /* 000A */ uint16_t m_id;
};
}}

namespace Battlenet { namespace Attribute {
struct LocaleTableEntry {
    public:
    static const BSN::FourCC s_default_locale;

    static void SetDefault_locale(BSN::FourCC& value);
    static void SetDefault_stringFileTable(BSN::Array<BSN::FixedBlob<40>, 10>& value);

    /* 0000 */ BSN::FourCC m_locale;
    /* 0004 */ BSN::Array<BSN::FixedBlob<40>, 10> m_stringFileTable;
};
}}

namespace Battlenet { namespace Attribute {
struct Visual {
    public:
    static void SetDefault_text(BSN::Optional<Battlenet::Attribute::LocalizedString>& value);
    static void SetDefault_tip(BSN::Optional<Battlenet::Attribute::LocalizedString>& value);
    static void SetDefault_art(BSN::Optional<Battlenet::Attribute::ArtId>& value);

    /* 0000 */ BSN::Optional<Battlenet::Attribute::LocalizedString> m_text;
    /* 0010 */ BSN::Optional<Battlenet::Attribute::LocalizedString> m_tip;
    /* 0020 */ BSN::Optional<Battlenet::Attribute::ArtId> m_art;
};
}}

namespace Battlenet { namespace Attribute {
struct Entry {
    public:
    static const BSN::FourCC s_default_code;

    static void SetDefault_code(BSN::FourCC& value);
    static void SetDefault_visual(Battlenet::Attribute::Visual& value);

    /* 0000 */ BSN::FourCC m_code;
    /* 0004 */ Battlenet::Attribute::Visual m_visual;
};
}}

namespace Battlenet { namespace Attribute {
struct Address {
    public:
    static void SetDefault_scope(Battlenet::Attribute::Scope::Enum& value);
    static void SetDefault_definitionHandle(Battlenet::Attribute::DefinitionHandle& value);

    /* 0000 */ Battlenet::Attribute::Scope::Enum m_scope;
    /* 0004 */ Battlenet::Attribute::DefinitionHandle m_definitionHandle;
};
}}

namespace Battlenet { namespace Attribute {
struct Parent {
    public:
    static void SetDefault_address(Battlenet::Attribute::Address& value);
    static void SetDefault_set(BSN::Array<BSN::FourCC, 32>& value);

    /* 0000 */ Battlenet::Attribute::Address m_address;
    /* 000C */ BSN::Array<BSN::FourCC, 32> m_set;
};
}}

namespace Battlenet { namespace Attribute {
struct NearInstance {
    public:
    static void SetDefault_index(uint8_t& value);
    static void SetDefault_baked(bool& value);

    /* 0000 */ uint8_t m_index;
    /* 0001 */ bool m_baked;
};
}}

namespace Battlenet { namespace Attribute {
struct NearInstanceList : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[36];

    NearInstanceList();
};
}}

namespace Battlenet { namespace Attribute {
struct Instance {
    public:
    static void SetDefault_definitionHandle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_data(Battlenet::Attribute::NearInstanceList& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_definitionHandle;
    /* 0008 */ Battlenet::Attribute::NearInstanceList m_data;
};
}}

namespace Battlenet { namespace Attribute {
struct Definition {
    public:
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_table(BSN::Array<Battlenet::Attribute::Entry, 32>& value);
    static void SetDefault_visual(Battlenet::Attribute::Visual& value);
    static void SetDefault_parents(BSN::Array<Battlenet::Attribute::Parent, 4>& value);
    static void SetDefault_arbitration(Battlenet::Attribute::Arbitration::Enum& value);
    static void SetDefault_visibility(Battlenet::Attribute::Visibility::Enum& value);
    static void SetDefault_access(Battlenet::Attribute::Access::Enum& value);
    static void SetDefault_flags(uint32_t& value);
    static void SetDefault_defaults(Battlenet::Attribute::NearInstanceList& value);
    static void SetDefault_sortOrder(uint8_t& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ BSN::Array<Battlenet::Attribute::Entry, 32> m_table;
    /* 060C */ Battlenet::Attribute::Visual m_visual;
    /* 0638 */ BSN::Array<Battlenet::Attribute::Parent, 4> m_parents;
    /* 087C */ Battlenet::Attribute::Arbitration::Enum m_arbitration;
    /* 0880 */ Battlenet::Attribute::Visibility::Enum m_visibility;
    /* 0884 */ Battlenet::Attribute::Access::Enum m_access;
    /* 0888 */ uint32_t m_flags;
    /* 088C */ Battlenet::Attribute::NearInstanceList m_defaults;
    /* 08B4 */ uint8_t m_sortOrder;
};
}}

namespace Battlenet { namespace Attribute {
struct DefinitionLock {
    public:
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_lockedScopes(BSN::Optional<BSN::FixedBitArray<16>>& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ BSN::Optional<BSN::FixedBitArray<16>> m_lockedScopes;
};
}}

namespace Battlenet { namespace Attribute {
struct WorkingSet {
    public:
    static void SetDefault_name(Battlenet::Attribute::LocalizedString& value);
    static void SetDefault_description(Battlenet::Attribute::LocalizedString& value);
    static void SetDefault_thumbnail(BSN::Optional<Battlenet::Attribute::ArtId>& value);
    static void SetDefault_bigMap(BSN::Optional<Battlenet::Attribute::ArtId>& value);
    static void SetDefault_maxPlayers(uint8_t& value);
    static void SetDefault_specialScope(Battlenet::Attribute::Scope::Enum& value);
    static void SetDefault_instances(BSN::Array<Battlenet::Attribute::Instance, 64>& value);
    static void SetDefault_visualFiles(BSN::Array<BSN::FixedBlob<40>, 16>& value);
    static void SetDefault_locales(BSN::Array<Battlenet::Attribute::LocaleTableEntry, 16>& value);
    static void SetDefault_visibilityOverrides(BSN::Array<Battlenet::Attribute::VisibilityOverride, 63>& value);
    static void SetDefault_indexRestrictions(BSN::Array<Battlenet::Attribute::IndexRestriction, 63>& value);
    static void SetDefault_permissionRestrictions(BSN::Array<Battlenet::Attribute::PermissionRestriction, 64>& value);

    /* 0000 */ Battlenet::Attribute::LocalizedString m_name;
    /* 000C */ Battlenet::Attribute::LocalizedString m_description;
    /* 0018 */ BSN::Optional<Battlenet::Attribute::ArtId> m_thumbnail;
    /* 0024 */ BSN::Optional<Battlenet::Attribute::ArtId> m_bigMap;
    /* 0030 */ uint8_t m_maxPlayers;
    /* 0034 */ Battlenet::Attribute::Scope::Enum m_specialScope;
    /* 0038 */ BSN::Array<Battlenet::Attribute::Instance, 64> m_instances;
    /* 0C3C */ BSN::Array<BSN::FixedBlob<40>, 16> m_visualFiles;
    /* 0EC0 */ BSN::Array<Battlenet::Attribute::LocaleTableEntry, 16> m_locales;
    /* 2844 */ BSN::Array<Battlenet::Attribute::VisibilityOverride, 63> m_visibilityOverrides;
    /* 2B3C */ BSN::Array<Battlenet::Attribute::IndexRestriction, 63> m_indexRestrictions;
    /* 4EB0 */ BSN::Array<Battlenet::Attribute::PermissionRestriction, 64> m_permissionRestrictions;
};
}}

namespace Battlenet { namespace Cache {
struct StreamDescription {
    public:
    static const BSN::FourCC s_default_channel;
    static const BSN::FourCC s_default_itemName;

    static void SetDefault_channel(BSN::FourCC& value);
    static void SetDefault_itemName(BSN::FourCC& value);

    /* 0000 */ BSN::FourCC m_channel;
    /* 0004 */ BSN::FourCC m_itemName;
};
}}

namespace Battlenet { namespace Cache {
struct ResolvedStreamItem {
    public:
    static const BSN::String<248> s_default_path;
    static const BSN::FixedBlob<40> s_default_handle;

    static void SetDefault_commandId(uint32_t& value);
    static void SetDefault_path(BSN::String<248>& value);
    static void SetDefault_handle(BSN::FixedBlob<40>& value);
    static void SetDefault_publicationTime(int32_t& value);
    static void SetDefault_index(uint16_t& value);
    static void SetDefault_totalNumItems(uint16_t& value);

    /* 0000 */ uint32_t m_commandId;
    /* 0004 */ BSN::String<248> m_path;
    /* 03F0 */ BSN::FixedBlob<40> m_handle;
    /* 0418 */ int32_t m_publicationTime;
    /* 041C */ uint16_t m_index;
    /* 041E */ uint16_t m_totalNumItems;
};
}}

namespace Battlenet { namespace Toon {
struct FullName {
    public:
    static const BSN::FourCC s_default_programId;
    static const BSN::String<25> s_default_name;

    static void SetDefault_region(uint8_t& value);
    static void SetDefault_programId(BSN::FourCC& value);
    static void SetDefault_realm(uint32_t& value);
    static void SetDefault_name(BSN::String<25>& value);

    /* 0000 */ uint8_t m_region;
    /* 0004 */ BSN::FourCC m_programId;
    /* 0008 */ uint32_t m_realm;
    /* 000C */ BSN::String<25> m_name;
};
}}

namespace Battlenet { namespace Profile {
struct RecordAddress {
    public:
    static void SetDefault_label(uint32_t& value);
    static void SetDefault_id(uint64_t& value);

    /* 0000 */ uint32_t m_label;
    /* 0008 */ uint64_t m_id;
};
}}

namespace Battlenet { namespace Conference {
struct ConferenceConfiguration {
    public:
    static const BSN::String<255> s_default_welcomeMessage;

    static void SetDefault_maxMembers(uint16_t& value);
    static void SetDefault_allowedPrograms(BSN::Array<BSN::FourCC, 4>& value);
    static void SetDefault_welcomeMessage(BSN::String<255>& value);
    static void SetDefault_allowedRealms(BSN::Array<uint32_t, 4>& value);
    static void SetDefault_flags(uint32_t& value);

    /* 0000 */ uint16_t m_maxMembers;
    /* 0004 */ BSN::Array<BSN::FourCC, 4> m_allowedPrograms;
    /* 0018 */ BSN::String<255> m_welcomeMessage;
    /* 0420 */ BSN::Array<uint32_t, 4> m_allowedRealms;
    /* 0434 */ uint32_t m_flags;
};
}}

namespace Battlenet { namespace Conference {
struct PublicName {
    public:
    static const BSN::FourCC s_default_locale;

    static void SetDefault_locale(BSN::FourCC& value);
    static void SetDefault_name(uint16_t& value);
    static void SetDefault_index(uint16_t& value);

    /* 0000 */ BSN::FourCC m_locale;
    /* 0004 */ uint16_t m_name;
    /* 0006 */ uint16_t m_index;
};
}}

namespace Battlenet { namespace Toon {
struct Handle {
    public:
    static const BSN::FourCC s_default_programId;

    static void SetDefault_region(uint8_t& value);
    static void SetDefault_programId(BSN::FourCC& value);
    static void SetDefault_realm(uint32_t& value);
    static void SetDefault_id(uint64_t& value);

    /* 0000 */ uint8_t m_region;
    /* 0004 */ BSN::FourCC m_programId;
    /* 0008 */ uint32_t m_realm;
    /* 0010 */ uint64_t m_id;
};
}}

namespace Battlenet { namespace S2Master {
struct InstanceHandle {
    public:
    static void SetDefault_instanceId(uint32_t& value);
    static void SetDefault_serverLabel(uint32_t& value);
    static void SetDefault_serverEpoch(int32_t& value);

    /* 0000 */ uint32_t m_instanceId;
    /* 0004 */ uint32_t m_serverLabel;
    /* 0008 */ int32_t m_serverEpoch;
};
}}

namespace Battlenet { namespace Conference {
struct PublicPartialName {
    public:
    static const BSN::FourCC s_default_locale;

    static void SetDefault_locale(BSN::FourCC& value);
    static void SetDefault_name(uint16_t& value);

    /* 0000 */ BSN::FourCC m_locale;
    /* 0004 */ uint16_t m_name;
};
}}

namespace Battlenet { namespace Conference {
struct ConferenceDescription {
    public:
    static void SetDefault_parentCategory(uint8_t& value);
    static void SetDefault_name(Battlenet::Conference::PublicName& value);
    static void SetDefault_sortOrder(uint16_t& value);
    static void SetDefault_configuration(Battlenet::Conference::ConferenceConfiguration& value);

    /* 0000 */ uint8_t m_parentCategory;
    /* 0004 */ Battlenet::Conference::PublicName m_name;
    /* 000C */ uint16_t m_sortOrder;
    /* 0010 */ Battlenet::Conference::ConferenceConfiguration m_configuration;
};
}}

namespace Battlenet { namespace Conference {
struct CategoryDescription {
    public:
    static void SetDefault_id(uint8_t& value);
    static void SetDefault_name(uint16_t& value);
    static void SetDefault_sortOrder(uint16_t& value);

    /* 0000 */ uint8_t m_id;
    /* 0002 */ uint16_t m_name;
    /* 0004 */ uint16_t m_sortOrder;
};
}}

namespace Battlenet { namespace S2Map {
struct Handle {
    public:
    static void SetDefault_id(uint32_t& value);
    static void SetDefault_version(uint32_t& value);

    /* 0000 */ uint32_t m_id;
    /* 0004 */ uint32_t m_version;
};
}}

namespace Battlenet { namespace S2Map {
struct LinkEntry {
    public:
    static void SetDefault_handle(Battlenet::S2Map::Handle& value);
    static void SetDefault_type(Battlenet::S2Map::LinkEntryType::Enum& value);

    /* 0000 */ Battlenet::S2Map::Handle m_handle;
    /* 0008 */ Battlenet::S2Map::LinkEntryType::Enum m_type;
};
}}

namespace Battlenet { namespace S2Map {
struct Link {
    public:
    static const BSN::FourCC s_default_speed;

    static void SetDefault_variantIndex(uint8_t& value);
    static void SetDefault_speed(BSN::FourCC& value);
    static void SetDefault_entriesDEPRECATED(BSN::Array<Battlenet::S2Map::LinkEntry, 10>& value);
    static void SetDefault_extraEntries(BSN::Array<Battlenet::S2Map::LinkEntry, 32>& value);

    /* 0000 */ uint8_t m_variantIndex;
    /* 0004 */ BSN::FourCC m_speed;
    /* 0008 */ BSN::Array<Battlenet::S2Map::LinkEntry, 10> m_entriesDEPRECATED;
    /* 0084 */ BSN::Array<Battlenet::S2Map::LinkEntry, 32> m_extraEntries;
};
}}

namespace Battlenet { namespace Result {
struct Container {
    public:
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_orderHint(uint32_t& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ uint32_t m_orderHint;
};
}}

namespace Battlenet { namespace Result {
struct Definition {
    public:
    static const BSN::String<31> s_default_name;

    static void SetDefault_name(BSN::String<31>& value);
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_visual(Battlenet::Attribute::Visual& value);
    static void SetDefault_containers(BSN::Array<Battlenet::Result::Container, 8>& value);
    static void SetDefault_form(Battlenet::Result::Form::Enum& value);
    static void SetDefault_flags(uint8_t& value);

    /* 0000 */ BSN::String<31> m_name;
    /* 0088 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0090 */ Battlenet::Attribute::Visual m_visual;
    /* 00BC */ BSN::Array<Battlenet::Result::Container, 8> m_containers;
    /* 0120 */ Battlenet::Result::Form::Enum m_form;
    /* 0124 */ uint8_t m_flags;
};
}}

namespace Battlenet { namespace S2Map {
struct MapSize {
    public:
    static void SetDefault_horizontal(uint16_t& value);
    static void SetDefault_vertical(uint16_t& value);

    /* 0000 */ uint16_t m_horizontal;
    /* 0002 */ uint16_t m_vertical;
};
}}

namespace Battlenet { namespace S2Map {
struct VariantId {
    public:
    static void SetDefault_category(uint8_t& value);
    static void SetDefault_mode(uint8_t& value);

    /* 0000 */ uint8_t m_category;
    /* 0001 */ uint8_t m_mode;
};
}}

namespace Battlenet { namespace S2Map {
struct VariantInfo {
    public:
    static void SetDefault_variantId(Battlenet::S2Map::VariantId& value);
    static void SetDefault_categoryName(Battlenet::Attribute::LocalizedString& value);
    static void SetDefault_modeName(Battlenet::Attribute::LocalizedString& value);
    static void SetDefault_categoryDescription(BSN::Optional<Battlenet::Attribute::LocalizedString>& value);
    static void SetDefault_modeDescription(BSN::Optional<Battlenet::Attribute::LocalizedString>& value);
    static void SetDefault_visual(Battlenet::Attribute::Visual& value);
    static void SetDefault_instances(BSN::Array<Battlenet::Attribute::Instance, 64>& value);
    static void SetDefault_lockedAttributes(BSN::Array<Battlenet::Attribute::DefinitionLock, 64>& value);
    static void SetDefault_maxTeamSize(uint8_t& value);
    static void SetDefault_visibilityOverrides(BSN::Array<Battlenet::Attribute::VisibilityOverride, 63>& value);
    static void SetDefault_instanceRestrictions(BSN::Array<Battlenet::Attribute::IndexRestriction, 63>& value);
    static void SetDefault_achievementTags(BSN::Array<BSN::FourCC, 63>& value);

    /* 0000 */ Battlenet::S2Map::VariantId m_variantId;
    /* 0004 */ Battlenet::Attribute::LocalizedString m_categoryName;
    /* 0010 */ Battlenet::Attribute::LocalizedString m_modeName;
    /* 001C */ BSN::Optional<Battlenet::Attribute::LocalizedString> m_categoryDescription;
    /* 002C */ BSN::Optional<Battlenet::Attribute::LocalizedString> m_modeDescription;
    /* 003C */ Battlenet::Attribute::Visual m_visual;
    /* 0068 */ BSN::Array<Battlenet::Attribute::Instance, 64> m_instances;
    /* 0C6C */ BSN::Array<Battlenet::Attribute::DefinitionLock, 64> m_lockedAttributes;
    /* 0F70 */ uint8_t m_maxTeamSize;
    /* 0F74 */ BSN::Array<Battlenet::Attribute::VisibilityOverride, 63> m_visibilityOverrides;
    /* 126C */ BSN::Array<Battlenet::Attribute::IndexRestriction, 63> m_instanceRestrictions;
    /* 35E0 */ BSN::Array<BSN::FourCC, 63> m_achievementTags;
};
}}

namespace Battlenet { namespace S2Map {
struct BnetHeader {
    public:
    static const BSN::String<80> s_default_name;
    static const BSN::FixedBlob<40> s_default_archiveCacheHandle;

    static void SetDefault_handle(Battlenet::S2Map::Handle& value);
    static void SetDefault_name(BSN::String<80>& value);
    static void SetDefault_archiveCacheHandle(BSN::FixedBlob<40>& value);
    static void SetDefault_mapNamespace(uint32_t& value);
    static void SetDefault_workingSet(Battlenet::Attribute::WorkingSet& value);
    static void SetDefault_definitions(BSN::Array<Battlenet::Attribute::Definition, 64>& value);
    static void SetDefault_dependenciesDEPRECATED(BSN::Array<Battlenet::S2Map::Handle, 4>& value);
    static void SetDefault_resultDefinitions(BSN::Array<Battlenet::Result::Definition, 512>& value);
    static void SetDefault_locales(BSN::Array<Battlenet::Attribute::LocaleTableEntry, 16>& value);
    static void SetDefault_size(BSN::Optional<Battlenet::S2Map::MapSize>& value);
    static void SetDefault_tileset(BSN::Optional<Battlenet::Attribute::LocalizedString>& value);
    static void SetDefault_labelDeprecated(BSN::Optional<BSN::FourCC>& value);
    static void SetDefault_defaultVariantIndex(uint8_t& value);
    static void SetDefault_variants(BSN::Array<Battlenet::S2Map::VariantInfo, 32>& value);
    static void SetDefault_extraDependencies(BSN::Array<Battlenet::S2Map::Handle, 32>& value);
    static void SetDefault_addDefaultPermissions(bool& value);
    static void SetDefault_relevantPermissions(BSN::Array<Battlenet::Permission::Handle, 100>& value);
    static void SetDefault_requiredPermissions(BSN::Array<Battlenet::Permission::Handle, 100>& value);
    static void SetDefault_labels(BSN::Array<BSN::FourCC, 16>& value);

    /* 0000 */ Battlenet::S2Map::Handle m_handle;
    /* 0008 */ BSN::String<80> m_name;
    /* 0154 */ BSN::FixedBlob<40> m_archiveCacheHandle;
    /* 017C */ uint32_t m_mapNamespace;
    /* 0180 */ Battlenet::Attribute::WorkingSet m_workingSet;
    /* 9638 */ BSN::Array<Battlenet::Attribute::Definition, 64> m_definitions;
    /* 2C43C */ BSN::Array<Battlenet::S2Map::Handle, 4> m_dependenciesDEPRECATED;
    /* 2C460 */ BSN::Array<Battlenet::Result::Definition, 512> m_resultDefinitions;
    /* 51464 */ BSN::Array<Battlenet::Attribute::LocaleTableEntry, 16> m_locales;
    /* 52DE8 */ BSN::Optional<Battlenet::S2Map::MapSize> m_size;
    /* 52DF0 */ BSN::Optional<Battlenet::Attribute::LocalizedString> m_tileset;
    /* 52E00 */ BSN::Optional<BSN::FourCC> m_labelDeprecated;
    /* 52E08 */ uint8_t m_defaultVariantIndex;
    /* 52E0C */ BSN::Array<Battlenet::S2Map::VariantInfo, 32> m_variants;
    /* C0A10 */ BSN::Array<Battlenet::S2Map::Handle, 32> m_extraDependencies;
    /* C0B14 */ bool m_addDefaultPermissions;
    /* C0B18 */ BSN::Array<Battlenet::Permission::Handle, 100> m_relevantPermissions;
    /* C1160 */ BSN::Array<Battlenet::Permission::Handle, 100> m_requiredPermissions;
    /* C17A8 */ BSN::Array<BSN::FourCC, 16> m_labels;
};
}}

namespace Battlenet { namespace S2Map {
struct GameHeader {
    public:
    static void SetDefault_dummy(bool& value);

    /* 0000 */ bool m_dummy;
};
}}

namespace Battlenet { namespace S2Map {
struct Header {
    public:
    static void SetDefault_bnet(Battlenet::S2Map::BnetHeader& value);
    static void SetDefault_game(Battlenet::S2Map::GameHeader& value);

    /* 0000 */ Battlenet::S2Map::BnetHeader m_bnet;
    /* C17F0 */ Battlenet::S2Map::GameHeader m_game;
};
}}

namespace Battlenet { namespace Result {
struct PlayerObjectType : public BSN::Choice {
    public:
    struct Player {
        public:
        static void SetDefault_toonHandle(Battlenet::Toon::Handle& value);
        static void SetDefault_toonProfileAddress(BSN::Optional<Battlenet::Profile::RecordAddress>& value);

        /* 0000 */ Battlenet::Toon::Handle m_toonHandle;
        /* 0018 */ BSN::Optional<Battlenet::Profile::RecordAddress> m_toonProfileAddress;
    };

    /* 0004 */ uint8_t m_value[48];

    PlayerObjectType();
};
}}

namespace Battlenet { namespace Result {
struct PlayerObject {
    public:
    static void SetDefault_scope(Battlenet::Attribute::Scope::Enum& value);
    static void SetDefault_playerObjectType(Battlenet::Result::PlayerObjectType& value);

    /* 0000 */ Battlenet::Attribute::Scope::Enum m_scope;
    /* 0004 */ Battlenet::Result::PlayerObjectType m_playerObjectType;
};
}}

namespace Battlenet { namespace S2Game { namespace Summary {
struct ParticipantRole {
    public:
    static void SetDefault_resultCode(Battlenet::S2Game::PlayerResultCode::Enum& value);

    /* 0000 */ Battlenet::S2Game::PlayerResultCode::Enum m_resultCode;
};
}}}

namespace Battlenet { namespace S2Map {
struct InfoBase {
    public:
    static const BSN::FixedBlob<40> s_default_headerCacheHandle;
    static const BSN::String<80> s_default_name;

    static void SetDefault_handle(Battlenet::S2Map::Handle& value);
    static void SetDefault_headerCacheHandle(BSN::FixedBlob<40>& value);
    static void SetDefault_uploadTime(int32_t& value);
    static void SetDefault_isLinked(bool& value);
    static void SetDefault_isLocked(bool& value);
    static void SetDefault_isPrivate(bool& value);
    static void SetDefault_mapSize(uint32_t& value);
    static void SetDefault_name(BSN::String<80>& value);
    static void SetDefault_profileRecordAddress(Battlenet::Profile::RecordAddress& value);
    static void SetDefault_isMod(bool& value);
    static void SetDefault_labelDeprecated(BSN::Optional<BSN::FourCC>& value);
    static void SetDefault_authorToonName(Battlenet::Toon::FullName& value);
    static void SetDefault_isLatestVersion(bool& value);
    static void SetDefault_mainLocale(BSN::Optional<BSN::FourCC>& value);
    static void SetDefault_authorToonHandle(Battlenet::Toon::Handle& value);
    static void SetDefault_isSkipInitialDownload(bool& value);
    static void SetDefault_createdTime(int32_t& value);
    static void SetDefault_labels(BSN::Array<BSN::FourCC, 16>& value);

    /* 0000 */ Battlenet::S2Map::Handle m_handle;
    /* 0008 */ BSN::FixedBlob<40> m_headerCacheHandle;
    /* 0030 */ int32_t m_uploadTime;
    /* 0034 */ bool m_isLinked;
    /* 0035 */ bool m_isLocked;
    /* 0036 */ bool m_isPrivate;
    /* 0038 */ uint32_t m_mapSize;
    /* 003C */ BSN::String<80> m_name;
    /* 0188 */ Battlenet::Profile::RecordAddress m_profileRecordAddress;
    /* 0198 */ bool m_isMod;
    /* 019C */ BSN::Optional<BSN::FourCC> m_labelDeprecated;
    /* 01A4 */ Battlenet::Toon::FullName m_authorToonName;
    /* 0220 */ bool m_isLatestVersion;
    /* 0224 */ BSN::Optional<BSN::FourCC> m_mainLocale;
    /* 0230 */ Battlenet::Toon::Handle m_authorToonHandle;
    /* 0248 */ bool m_isSkipInitialDownload;
    /* 024C */ int32_t m_createdTime;
    /* 0250 */ BSN::Array<BSN::FourCC, 16> m_labels;
};
}}

namespace Battlenet { namespace S2Map {
struct Info : public Battlenet::S2Map::InfoBase {
    public:
    static void SetDefault_InfoBase(Battlenet::S2Map::InfoBase& value);
    static void SetDefault_authorRealName(BSN::Optional<Battlenet::Account::FullName>& value);

    /* 0298 */ BSN::Optional<Battlenet::Account::FullName> m_authorRealName;
};
}}

namespace Battlenet { namespace Event { namespace OfflineDataStore {
struct Email {
    public:
    static void SetDefault_hash(BSN::Optional<uint64_t>& value);

    /* 0000 */ BSN::Optional<uint64_t> m_hash;
};
}}}

namespace Battlenet { namespace Event { namespace OfflineDataStore {
struct TagIdx {
    public:
    static const BSN::FourCC s_default_tag;

    static void SetDefault_tag(BSN::FourCC& value);
    static void SetDefault_idx(uint32_t& value);

    /* 0000 */ BSN::FourCC m_tag;
    /* 0004 */ uint32_t m_idx;
};
}}}

namespace Battlenet { namespace Event { namespace OfflineDataStore {
struct StoredData {
    public:
    static const BSN::AsciiString<32> s_default_gameAccountName;
    static const BSN::FourCC s_default_program;

    static void SetDefault_toon(Battlenet::Toon::FullName& value);
    static void SetDefault_fullName(Battlenet::Account::FullName& value);
    static void SetDefault_accountRegion(uint8_t& value);
    static void SetDefault_gameAccountRegion(uint8_t& value);
    static void SetDefault_gameAccountName(BSN::AsciiString<32>& value);
    static void SetDefault_gameAccountFlags(uint64_t& value);
    static void SetDefault_program(BSN::FourCC& value);
    static void SetDefault_tagidx(BSN::Optional<BSN::Array<Battlenet::Event::OfflineDataStore::TagIdx, 16>>& value);
    static void SetDefault_lastOnline(uint32_t& value);
    static void SetDefault_toonHandle(Battlenet::Toon::Handle& value);
    static void SetDefault_accountId(uint32_t& value);

    /* 0000 */ Battlenet::Toon::FullName m_toon;
    /* 007C */ Battlenet::Account::FullName m_fullName;
    /* 0194 */ uint8_t m_accountRegion;
    /* 0195 */ uint8_t m_gameAccountRegion;
    /* 0198 */ BSN::AsciiString<32> m_gameAccountName;
    /* 01C0 */ uint64_t m_gameAccountFlags;
    /* 01C8 */ BSN::FourCC m_program;
    /* 01CC */ BSN::Optional<BSN::Array<Battlenet::Event::OfflineDataStore::TagIdx, 16>> m_tagidx;
    /* 0254 */ uint32_t m_lastOnline;
    /* 0258 */ Battlenet::Toon::Handle m_toonHandle;
    /* 0270 */ uint32_t m_accountId;
};
}}}

namespace Battlenet { namespace Event { namespace OfflineDataStore {
struct OfflineData {
    public:
    static void SetDefault_info(BSN::Optional<BSN::Array<Battlenet::Event::OfflineDataStore::StoredData, 3>>& value);

    /* 0000 */ BSN::Optional<BSN::Array<Battlenet::Event::OfflineDataStore::StoredData, 3>> m_info;
};
}}}

namespace Battlenet { namespace Toon {
struct UnlockDefinitionFile {
    public:
    static const BSN::FixedBlob<40> s_default_unlockDefinitionFileCacheHandle;

    static void SetDefault_unlockDefinitionFileCacheHandle(BSN::FixedBlob<40>& value);

    /* 0000 */ BSN::FixedBlob<40> m_unlockDefinitionFileCacheHandle;
};
}}

namespace Battlenet { namespace Result {
struct Value {
    public:
    static void SetDefault_value(uint32_t& value);
    static void SetDefault_maxValue(BSN::Optional<uint32_t>& value);
    static void SetDefault_time(int32_t& value);

    /* 0000 */ uint32_t m_value;
    /* 0004 */ BSN::Optional<uint32_t> m_maxValue;
    /* 000C */ int32_t m_time;
};
}}

namespace Battlenet { namespace Result {
struct Instance {
    public:
    static void SetDefault_handle(Battlenet::Attribute::DefinitionHandle& value);
    static void SetDefault_values(BSN::Array<BSN::Array<Battlenet::Result::Value, 32>, 15>& value);

    /* 0000 */ Battlenet::Attribute::DefinitionHandle m_handle;
    /* 0008 */ BSN::Array<BSN::Array<Battlenet::Result::Value, 32>, 15> m_values;
};
}}

namespace Battlenet { namespace Result {
struct DefinitionListContainer {
    public:
    static void SetDefault_definitionList(BSN::Array<Battlenet::Result::Definition, 512>& value);

    /* 0000 */ BSN::Array<Battlenet::Result::Definition, 512> m_definitionList;
};
}}

namespace Battlenet { namespace Result {
struct InstanceListContainer {
    public:
    static void SetDefault_instanceList(BSN::Array<Battlenet::Result::Instance, 10>& value);

    /* 0000 */ BSN::Array<Battlenet::Result::Instance, 10> m_instanceList;
};
}}

namespace Battlenet { namespace Result {
struct PlayerListContainer {
    public:
    static void SetDefault_playerList(BSN::Array<Battlenet::Result::PlayerObject, 16>& value);

    /* 0000 */ BSN::Array<Battlenet::Result::PlayerObject, 16> m_playerList;
};
}}

namespace Battlenet { namespace S2Game { namespace Summary {
struct Role : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[4];

    Role();
};
}}}

namespace Battlenet { namespace S2Game { namespace Summary {
struct PlayerData {
    public:
    static void SetDefault_playerObject(Battlenet::Result::PlayerObject& value);
    static void SetDefault_role(Battlenet::S2Game::Summary::Role& value);
    static void SetDefault_actualRace(BSN::Optional<BSN::FourCC>& value);

    /* 0000 */ Battlenet::Result::PlayerObject m_playerObject;
    /* 0038 */ Battlenet::S2Game::Summary::Role m_role;
    /* 0040 */ BSN::Optional<BSN::FourCC> m_actualRace;
};
}}}

namespace Battlenet { namespace S2Game { namespace Summary {
struct Data {
    public:
    static const BSN::FixedBlob<4> s_default_seed;

    static void SetDefault_mapLink(Battlenet::S2Map::Link& value);
    static void SetDefault_replayCacheHandle(BSN::Optional<BSN::FixedBlob<40>>& value);
    static void SetDefault_instanceHandle(Battlenet::S2Master::InstanceHandle& value);
    static void SetDefault_playerList(BSN::Array<Battlenet::S2Game::Summary::PlayerData, 16>& value);
    static void SetDefault_seed(BSN::FixedBlob<4>& value);
    static void SetDefault_definitionList(BSN::Array<Battlenet::Attribute::Definition, 64>& value);
    static void SetDefault_workingSet(Battlenet::Attribute::WorkingSet& value);
    static void SetDefault_simulationTime(int32_t& value);
    static void SetDefault_endTime(int32_t& value);
    static void SetDefault_resultsCacheHandle(BSN::Optional<BSN::FixedBlob<40>>& value);

    /* 0000 */ Battlenet::S2Map::Link m_mapLink;
    /* 0208 */ BSN::Optional<BSN::FixedBlob<40>> m_replayCacheHandle;
    /* 0234 */ Battlenet::S2Master::InstanceHandle m_instanceHandle;
    /* 0240 */ BSN::Array<Battlenet::S2Game::Summary::PlayerData, 16> m_playerList;
    /* 06C4 */ BSN::FixedBlob<4> m_seed;
    /* 06C8 */ BSN::Array<Battlenet::Attribute::Definition, 64> m_definitionList;
    /* 234D0 */ Battlenet::Attribute::WorkingSet m_workingSet;
    /* 2C988 */ int32_t m_simulationTime;
    /* 2C98C */ int32_t m_endTime;
    /* 2C990 */ BSN::Optional<BSN::FixedBlob<40>> m_resultsCacheHandle;
};
}}}

namespace Battlenet { namespace Event {
struct Destination : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[4];

    Destination();
};
}}

namespace Battlenet { namespace Event {
struct Header {
    public:
    /* 0000 */ uint8_t m_id;
    /* 0004 */ BSN::Optional<uint32_t> m_sourceAddress;
    /* 000C */ BSN::FourCC m_sourceName;
    /* 0010 */ Battlenet::Event::Destination m_destination;

    Header();
};
}}

namespace Battlenet { namespace Event { namespace AuthModule {
struct Parameter : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[1036];

    Parameter();
};
}}}

namespace Battlenet { namespace Registry {
struct FriendDataVersion1 {
    public:
    /* 0000 */ uint16_t m_volume;
};
}}

namespace Battlenet { namespace S2Master {
struct AdvertHandle {
    public:
    /* 0000 */ uint32_t m_serverLabel;
    /* 0004 */ int32_t m_serverEpoch;
    /* 0008 */ uint32_t m_advertId;
};
}}

namespace Battlenet { namespace S2Map {
struct ShortLink {
    public:
    /* 0000 */ uint8_t m_variantIndex;
    /* 0004 */ BSN::FourCC m_speed;
    /* 0008 */ BSN::Array<Battlenet::S2Map::LinkEntry, 5> m_entries;
};
}}

namespace Battlenet { namespace Client { namespace Defines {
struct PlayerTarget : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[328];

    PlayerTarget();
};
}}}

namespace Battlenet { namespace MatchMaker {
struct Filter {
    public:
    /* 0000 */ BSN::Optional<BSN::Array<BSN::FourCC, 8>> m_tags;
    /* 0028 */ BSN::Optional<bool> m_active;
    /* 002C */ BSN::Optional<uint32_t> m_mmqId;
};
}}

namespace Battlenet { namespace MatchMaker {
struct Handle {
    public:
    /* 0000 */ uint8_t m_region;
    /* 0004 */ BSN::FourCC m_programId;
    /* 0008 */ uint32_t m_realm;
    /* 000C */ uint32_t m_id;
    /* 0010 */ uint16_t m_version;
};
}}

namespace Battlenet { namespace MatchMaker {
struct MapPreferences {
    public:
    /* 0000 */ BSN::Array<uint32_t, 3> m_vetoedMapIds;
};
}}

namespace Battlenet { namespace S2Master {
struct OGLFilterValue {
    public:
    /* 0000 */ BSN::Optional<uint8_t> m_intValue;
    /* 0004 */ BSN::Optional<BSN::FourCC> m_fourccValue;
};
}}

namespace Battlenet { namespace S2Master {
struct OGLFilter {
    public:
    /* 0000 */ Battlenet::S2Master::OGLFilterType::Enum m_type;
    /* 0004 */ Battlenet::S2Master::OGLFilterValue m_value;
};
}}

namespace Battlenet { namespace Party {
struct ReadyProcess : public BSN::Choice {
    public:
    struct JoinAmm {
        public:
        /* 0000 */ Battlenet::MatchMaker::Handle m_mmqHandle;
        /* 0014 */ BSN::FixedBlob<40> m_cacheHandle;
        /* 0040 */ Battlenet::Profile::RecordAddress m_profileAddress;
        /* 0050 */ BSN::Array<Battlenet::S2Map::Handle, 100> m_vetoes;
    };

    struct JoinCustomGame {
        public:
        /* 0000 */ Battlenet::S2Map::ShortLink m_map;
        /* 0048 */ Battlenet::S2Master::AdvertPostMode::Enum m_mode;
    };

    /* 0004 */ uint8_t m_value[888];

    ReadyProcess();
};
}}

namespace Battlenet { namespace WoW {
struct RealmHandle {
    public:
    /* 0000 */ uint8_t m_region;
    /* 0001 */ uint8_t m_site;
    /* 0004 */ uint32_t m_realm;
};
}}

namespace Battlenet { namespace Profile {
struct ReadSelection : public BSN::Choice {
    public:
    struct All {
        public:
        /* 0000 */ BSN::Blob<32> m_prefixPath;
    };

    struct Slice {
        public:
        /* 0000 */ BSN::Blob<32> m_prefixPath;
        /* 0028 */ BSN::Optional<int64_t> m_sliceStart;
        /* 0038 */ BSN::Optional<int64_t> m_sliceEnd;
    };

    struct Random {
        public:
        /* 0000 */ BSN::Blob<32> m_prefixPath;
        /* 0028 */ BSN::Array<uint64_t, 100> m_indices;
    };

    struct MultiPath {
        public:
        /* 0000 */ BSN::Array<BSN::Blob<32>, 31> m_paths;
    };

    /* 0004 */ uint8_t m_value[1120];

    ReadSelection();
};
}}

namespace Battlenet { namespace Client { namespace S2GameDefines {
struct SinglePlayerEvent {
    public:
    struct Event : public BSN::Choice {
        public:
        struct CampaignCompleted {
            public:
            /* 0000 */ int32_t m_date;
        };

        struct CampaignMissionCompleted {
            public:
            /* 0000 */ uint32_t m_id;
            /* 0004 */ int32_t m_date;
        };

        struct SkirmishCompleted {
            public:
            /* 0000 */ Battlenet::S2Game::PlayerResultCode::Enum m_decision;
        };

        struct ChallengeCompleted {
            public:
            /* 0000 */ uint32_t m_id;
            /* 0004 */ int32_t m_date;
        };

        struct BonusObjectiveCompleted {
            public:
            /* 0000 */ uint32_t m_id;
            /* 0004 */ int32_t m_date;
        };

        /* 0004 */ uint8_t m_value[8];

        Event();
    };

    /* 0000 */ BSN::FourCC m_difficulty;
    /* 0004 */ Battlenet::Client::S2GameDefines::SinglePlayerEvent::Event m_event;
};
}}}

namespace Battlenet { namespace Ladder {
struct Membership {
    public:
    /* 0000 */ Battlenet::Profile::RecordAddress m_memberId;
    /* 0010 */ uint32_t m_ladderId;
};
}}

namespace Battlenet { namespace S2Map {
struct MapSearch {
    public:
    /* 0000 */ uint32_t m_startIndex;
    /* 0004 */ uint32_t m_pageSize;
    /* 0008 */ BSN::Optional<BSN::String<80>> m_search;
    /* 0158 */ BSN::Optional<BSN::FourCC> m_locale;
    /* 0160 */ Battlenet::S2Map::MapFilterType::Enum m_filterType;
    /* 0164 */ int32_t m_filterValue;
};
}}

namespace Battlenet { namespace S2Map {
struct MapListChoice : public BSN::Choice {
    public:
    struct Favorites {
        public:
    };

    struct RecentPlayed {
        public:
    };

    /* 0004 */ uint8_t m_value[360];

    MapListChoice();
};
}}

namespace Battlenet { namespace Event { namespace Creep {
struct LocaleTableEntry {
    public:
    /* 0000 */ BSN::FourCC m_locale;
    /* 0004 */ BSN::Array<BSN::String<248>, 10> m_files;
};
}}}

namespace Battlenet { namespace S2Map {
struct LocalizedName {
    public:
    /* 0000 */ BSN::FourCC m_locale;
    /* 0004 */ BSN::String<80> m_name;
};
}}

namespace Battlenet { namespace S2Map {
struct AuthorPrefs {
    public:
    /* 0000 */ BSN::Optional<bool> m_showRID;
    /* 0002 */ BSN::Optional<bool> m_accountLinked;
    /* 0004 */ BSN::Optional<bool> m_isPrivate;
};
}}

namespace Battlenet { namespace Toon {
struct ToonCreationData {
    public:
    /* 0000 */ BSN::String<12> m_displayName;
};
}}

namespace Battlenet { namespace Event { namespace Creep {
struct Invoke : public BSN::Choice {
    public:
    struct Authentication : public BSN::Choice {
        public:
        struct Hostname {
            public:
            /* 0000 */ BSN::AsciiString<255> m_hostname;
        };

        struct Logon {
            public:
            /* 0000 */ BSN::AsciiString<320> m_mail;
        };

        struct Module {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0004 */ BSN::AsciiString<63> m_function;
            /* 0048 */ BSN::Array<Battlenet::Event::AuthModule::Parameter, 4> m_parameters;
        };

        struct CachedAccountRequest {
            public:
        };

        struct CachedGameAccountRequest {
            public:
        };

        struct CachedToonRequest {
            public:
        };

        struct SetRememberAccount {
            public:
            /* 0000 */ bool m_remember;
        };

        /* 0004 */ uint8_t m_value[4236];

        Authentication();
    };

    struct Friend : public BSN::Choice {
        public:
        struct SaveRegistryData {
            public:
            /* 0000 */ BSN::String<255> m_key;
            /* 0408 */ BSN::String<255> m_name;
            /* 0810 */ Battlenet::Registry::FriendDataVersion1 m_value;
        };

        struct RetrieveRegistryData {
            public:
            /* 0000 */ BSN::String<255> m_key;
            /* 0408 */ BSN::String<255> m_name;
        };

        struct DeleteRegistryData {
            public:
            /* 0000 */ BSN::String<255> m_key;
            /* 0408 */ BSN::String<255> m_name;
        };

        /* 0004 */ uint8_t m_value[2068];

        Friend();
    };

    struct Advert : public BSN::Choice {
        public:
        struct Join {
            public:
            /* 0000 */ Battlenet::S2Master::AdvertHandle m_advertHandle;
        };

        struct Post {
            public:
            /* 0000 */ BSN::Optional<Battlenet::S2Map::ShortLink> m_mapLink;
            /* 004C */ Battlenet::S2Master::AdvertPostMode::Enum m_mode;
        };

        struct Invite {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0004 */ Battlenet::Client::Defines::PlayerTarget m_target;
        };

        struct AcceptInvite {
            public:
        };

        struct DeclineInvite {
            public:
        };

        struct OfflinePost {
            public:
            /* 0000 */ Battlenet::S2Map::ShortLink m_mapLink;
        };

        /* 0004 */ uint8_t m_value[336];

        Advert();
    };

    struct MatchMaker : public BSN::Choice {
        public:
        struct Subscribe {
            public:
            /* 0000 */ bool m_state;
            /* 0004 */ BSN::Optional<Battlenet::MatchMaker::Filter> m_filter;
        };

        struct ClientRegister {
            public:
            /* 0000 */ Battlenet::MatchMaker::Handle m_handle;
            /* 0014 */ uint32_t m_token;
            /* 0018 */ Battlenet::MatchMaker::MapPreferences m_mapPreferences;
        };

        struct ClientLeaveQueue {
            public:
            /* 0000 */ Battlenet::MatchMaker::Handle m_handle;
        };

        struct OGLPageRequest {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Master::OGLCriticId::Enum m_criticId;
            /* 0008 */ uint16_t m_startIndex;
            /* 000A */ uint8_t m_pageSize;
            /* 000C */ BSN::Array<Battlenet::S2Master::OGLFilter, 5> m_filters;
        };

        struct SubmitSelfEvaluation {
            public:
            /* 0000 */ Battlenet::League::SelfEvaluation::Enum m_selfEvaluation;
        };

        struct ExitPracticeLeague {
            public:
        };

        struct GetLeagueStatus {
            public:
        };

        struct SaveMapPreferences {
            public:
            /* 0000 */ uint32_t m_matchmakerId;
            /* 0004 */ Battlenet::MatchMaker::MapPreferences m_mapPreferences;
        };

        struct GamePopularityRequest {
            public:
            /* 0000 */ Battlenet::S2Map::ShortLink m_mapLink;
        };

        struct MapPopularityRequest {
            public:
            /* 0000 */ Battlenet::S2Map::Handle m_handle;
        };

        /* 0004 */ uint8_t m_value[96];

        MatchMaker();
    };

    struct Presence : public BSN::Choice {
        public:
        struct RequestStatistics {
            public:
            /* 0000 */ bool m_on;
        };

        /* 0004 */ uint8_t m_value[4];

        Presence();
    };

    struct Party : public BSN::Choice {
        public:
        struct ModifyNonLobbyAttribute {
            public:
            /* 0000 */ Battlenet::Attribute::DefinitionHandle m_definitionHandle;
            /* 0008 */ uint8_t m_index;
        };

        struct ReadyCheckRequest {
            public:
            /* 0000 */ bool m_ready;
        };

        struct PartyLeaveGame {
            public:
        };

        struct BeginReadyProcess {
            public:
            /* 0000 */ Battlenet::Party::ReadyProcess m_process;
        };

        struct ReadyProcessUpdate {
            public:
            /* 0000 */ uint16_t m_reason;
        };

        struct DownloadUpdate {
            public:
            /* 0000 */ float m_progress;
            /* 0004 */ uint16_t m_currentFile;
            /* 0006 */ uint16_t m_totalFiles;
            /* 0008 */ BSN::String<255> m_label;
            /* 0410 */ bool m_isFinished;
        };

        /* 0004 */ uint8_t m_value[1044];

        Party();
    };

    struct WoW : public BSN::Choice {
        public:
        struct RequestRealmList {
            public:
            /* 0000 */ bool m_on;
        };

        struct JoinRealm {
            public:
            /* 0000 */ BSN::Optional<Battlenet::WoW::RealmHandle> m_realmId;
        };

        /* 0004 */ uint8_t m_value[12];

        WoW();
    };

    struct Profile : public BSN::Choice {
        public:
        struct RequestGameResults {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::FixedBlob<40> m_summaryCacheHandle;
        };

        struct RequestLocalGameResults {
            public:
            /* 0000 */ uint32_t m_commandId;
        };

        struct ReadRequest {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0008 */ Battlenet::Profile::RecordAddress m_recordAddress;
            /* 0018 */ bool m_allowRemoteResolve;
            /* 001C */ int32_t m_maxAge;
            /* 0020 */ Battlenet::Profile::ReadSelection m_selection;
        };

        struct QueryAddress {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Client::Defines::PlayerTarget m_target;
        };

        struct SubmitS2SinglePlayerStats {
            public:
            /* 0000 */ BSN::Array<Battlenet::Client::S2GameDefines::SinglePlayerEvent, 15> m_events;
        };

        /* 0004 */ uint8_t m_value[1160];

        Profile();
    };

    struct Ladder : public BSN::Choice {
        public:
        struct GetRankingsRequest {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0008 */ BSN::Array<Battlenet::Ladder::Membership, 50> m_requests;
        };

        struct GetMembersRequest {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0004 */ uint32_t m_ladderId;
            /* 0008 */ uint32_t m_startRank;
            /* 000C */ uint32_t m_numMembers;
        };

        /* 0004 */ uint8_t m_value[1216];

        Ladder();
    };

    struct Service : public BSN::Choice {
        public:
        struct Disconnect {
            public:
        };

        struct DebugDisconnect {
            public:
            /* 0000 */ uint16_t m_error;
            /* 0004 */ uint32_t m_timeout;
        };

        /* 0004 */ uint8_t m_value[8];

        Service();
    };

    struct S2Map : public BSN::Choice {
        public:
        struct Info {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::Array<Battlenet::S2Map::Handle, 100> m_handles;
        };

        struct List {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::MapListChoice m_section;
            /* 0170 */ bool m_excludeMods;
        };

        struct VersionList {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint32_t m_mapId;
            /* 0008 */ BSN::Optional<uint32_t> m_maxVersion;
        };

        struct DownloadMap {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::Link m_shortLink;
        };

        struct ListFileDependencies {
            public:
            struct Link : public BSN::Choice {
                public:
                /* 0004 */ uint8_t m_value[520];

                Link();
            };

            /* 0000 */ Battlenet::Event::Creep::Invoke::S2Map::ListFileDependencies::Link m_link;
            /* 020C */ bool m_restrictToCurrentLocale;
            /* 0210 */ uint32_t m_commandId;
        };

        struct PrePublishMap {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::Array<BSN::String<80>, 16> m_localizedMapNames;
        };

        struct PublishMap {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::String<248> m_archiveFile;
            /* 03F0 */ BSN::Array<BSN::String<248>, 16> m_visualFiles;
            /* 42B4 */ BSN::Array<Battlenet::Event::Creep::LocaleTableEntry, 16> m_localeTable;
            /* 2B6B8 */ BSN::Array<Battlenet::Event::Creep::LocaleTableEntry, 16> m_headerLocaleTable;
            /* 52ABC */ BSN::Array<Battlenet::S2Map::LocalizedName, 16> m_localizedMapNames;
            /* 53FC0 */ bool m_isMod;
            /* 53FC1 */ bool m_isUndependable;
            /* 53FC2 */ bool m_isLocked;
            /* 53FC3 */ bool m_isSkipInitialDownload;
            /* 53FC4 */ bool m_isMajorRevision;
            /* 53FC5 */ Battlenet::S2Map::AuthorPrefs m_authorPrefs;
            /* 53FD0 */ Battlenet::S2Map::Header m_mapHeader;
        };

        struct SetAuthorPrefs {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::Handle m_handle;
            /* 000C */ Battlenet::S2Map::AuthorPrefs m_authorPrefs;
        };

        struct UnlinkMap {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::Handle m_handle;
        };

        struct GetStorageInfo {
            public:
            /* 0000 */ uint32_t m_commandId;
        };

        /* 0004 */ uint8_t m_value[1136584];

        S2Map();
    };

    struct Cache : public BSN::Choice {
        public:
        struct Resolve {
            public:
            /* 0000 */ BSN::FixedBlob<40> m_handle;
            /* 0028 */ BSN::FourCC m_invokeSource;
            /* 002C */ uint32_t m_commandId;
            /* 0030 */ bool m_doNotDownload;
            /* 0031 */ bool m_blocking;
        };

        struct GetStreamItems {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint8_t m_maxItems;
            /* 0008 */ BSN::Optional<int32_t> m_referenceTime;
            /* 0010 */ Battlenet::Cache::StreamDirection::Enum m_direction;
            /* 0014 */ BSN::FourCC m_channel;
            /* 0018 */ BSN::FourCC m_itemName;
        };

        struct DownloadURL {
            public:
            /* 0000 */ BSN::FourCC m_invokeSource;
            /* 0004 */ uint32_t m_commandId;
            /* 0008 */ BSN::String<255> m_url;
            /* 0410 */ BSN::String<255> m_localFileName;
        };

        /* 0004 */ uint8_t m_value[2072];

        Cache();
    };

    struct DarkPortal : public BSN::Choice {
        public:
        struct Select {
            public:
            /* 0000 */ BSN::AsciiString<31> m_name;
        };

        /* 0004 */ uint8_t m_value[36];

        DarkPortal();
    };

    struct Toon : public BSN::Choice {
        public:
        struct CreateStart {
            public:
        };

        struct CreateFinish {
            public:
            /* 0000 */ Battlenet::Toon::ToonCreationData m_toonCreateData;
        };

        struct Select {
            public:
            /* 0000 */ BSN::String<25> m_toonName;
            /* 0070 */ uint32_t m_realm;
        };

        struct Cancel {
            public:
        };

        struct SetProfileMotto {
            public:
            /* 0000 */ BSN::String<1023> m_motto;
        };

        struct ToonRename {
            public:
            /* 0000 */ BSN::String<25> m_newName;
            /* 0070 */ BSN::String<25> m_oldName;
            /* 00E0 */ uint32_t m_realm;
        };

        /* 0004 */ uint8_t m_value[4104];

        Toon();
    };

    /* 0004 */ uint8_t m_value[1136588];

    Invoke();
};
}}}

namespace Battlenet { namespace GameAccount {
struct CachedAccountList {
    public:
    /* 0000 */ BSN::Array<BSN::AsciiString<32>, 31> m_accounts;
};
}}

namespace Battlenet { namespace Event { namespace Larva {
struct PartyReadyProcessNotify : public BSN::Choice {
    public:
    struct JoinAmm {
        public:
        /* 0000 */ Battlenet::MatchMaker::Handle m_mmqHandle;
        /* 0014 */ BSN::Array<Battlenet::S2Map::Handle, 100> m_vetoes;
    };

    struct JoinCustomGame {
        public:
        /* 0000 */ Battlenet::S2Map::ShortLink m_map;
        /* 0048 */ Battlenet::S2Master::AdvertPostMode::Enum m_mode;
    };

    /* 0004 */ uint8_t m_value[824];

    PartyReadyProcessNotify();
};
}}}

namespace Battlenet { namespace Ladder {
struct Ranking {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ uint32_t m_rank;
    /* 0008 */ bool m_exact;
};
}}

namespace Battlenet { namespace Ladder {
struct KeyValue {
    public:
    /* 0000 */ uint32_t m_key;
    /* 0004 */ uint32_t m_value;
};
}}

namespace Battlenet { namespace Ladder {
struct GameData {
    public:
    /* 0000 */ BSN::Optional<BSN::String<256>> m_name;
    /* 0410 */ BSN::Optional<BSN::Array<Battlenet::Ladder::KeyValue, 64>> m_values;
};
}}

namespace Battlenet { namespace Ladder {
struct RankingResponse : public BSN::Choice {
    public:
    struct Success {
        public:
        /* 0000 */ Battlenet::Ladder::Membership m_membership;
        /* 0018 */ Battlenet::Ladder::Ranking m_ranking;
        /* 0024 */ Battlenet::Ladder::GameData m_gameData;
    };

    /* 0004 */ uint8_t m_value[1600];

    RankingResponse();
};
}}

namespace Battlenet { namespace Ladder {
struct Member {
    public:
    /* 0000 */ Battlenet::Profile::RecordAddress m_id;
    /* 0010 */ double m_rating;
    /* 0018 */ Battlenet::Ladder::GameData m_gameData;
    /* 0630 */ int32_t m_joinTimestamp;
    /* 0634 */ uint32_t m_highestRank;
    /* 0638 */ uint32_t m_previousRank;
    /* 063C */ int32_t m_previousRankTimestamp;
};
}}

namespace Battlenet { namespace Ladder {
struct GetMembersResult : public BSN::Choice {
    public:
    struct Success {
        public:
        /* 0000 */ uint32_t m_count;
        /* 0004 */ uint32_t m_cutoffRank;
        /* 0008 */ uint32_t m_startRank;
        /* 0010 */ BSN::Array<Battlenet::Ladder::Member, 200> m_members;
    };

    /* 0004 */ uint8_t m_value[320024];

    GetMembersResult();
};
}}

namespace Battlenet { namespace IP4 {
struct AddressPort {
    public:
    /* 0000 */ BSN::FixedBlob<4> m_address;
    /* 0004 */ BSN::FixedBlob<2> m_port;
};
}}

namespace Battlenet { namespace S2Game {
struct GameLocation {
    public:
    /* 0000 */ Battlenet::IP4::AddressPort m_addressPort;
    /* 0008 */ uint32_t m_instanceId;
};
}}

namespace Battlenet { namespace S2Game {
struct PlayToken {
    public:
    /* 0000 */ uint32_t m_clientId;
    /* 0004 */ BSN::FixedBlob<16> m_serverSalt;
};
}}

namespace Battlenet { namespace S2Master {
struct AdvertResponseData {
    public:
    /* 0000 */ Battlenet::S2Game::GameLocation m_gameLocation;
    /* 000C */ Battlenet::S2Game::PlayToken m_playToken;
    /* 0020 */ Battlenet::S2Map::Link m_mapLink;
};
}}

namespace Battlenet { namespace S2Master {
struct AdvertPostResponseData : public Battlenet::S2Master::AdvertResponseData {
    public:
    /* 0228 */ Battlenet::S2Master::AdvertPostJoinedOrCreated::Enum m_joinedOrCreated;
};
}}

namespace Battlenet { namespace S2Master {
struct AdvertJoinResponseData : public Battlenet::S2Master::AdvertResponseData {
    public:
};
}}

namespace Battlenet { namespace MatchMaker {
struct HistogramSet {
    public:
    /* 0000 */ BSN::Array<BSN::Optional<uint32_t>, 7> m_avgWaitTime;
    /* 003C */ BSN::Array<BSN::Optional<uint32_t>, 7> m_playerMatchRate;
};
}}

namespace Battlenet { namespace MatchMaker {
struct Update {
    public:
    /* 0000 */ Battlenet::MatchMaker::Handle m_handle;
    /* 0014 */ Battlenet::MatchMaker::HistogramSet m_stats;
};
}}

namespace Battlenet { namespace MatchMaker {
struct Remove {
    public:
    /* 0000 */ Battlenet::MatchMaker::Handle m_handle;
};
}}

namespace Battlenet { namespace MatchMaker {
struct ClientRegisterResponseSuccess {
    public:
    /* 0000 */ Battlenet::S2Game::GameLocation m_gameLocation;
    /* 000C */ Battlenet::S2Game::PlayToken m_playToken;
    /* 0020 */ Battlenet::S2Map::Link m_mapLink;
    /* 0228 */ Battlenet::S2Master::InstanceHandle m_instanceHandle;
    /* 0234 */ Battlenet::S2Master::AdvertHandle m_advertHandle;
};
}}

namespace Battlenet { namespace S2Master {
struct ScoredGameInfo {
    public:
    /* 0000 */ Battlenet::S2Map::ShortLink m_map;
    /* 0048 */ Battlenet::S2Map::VariantId m_variantId;
    /* 004C */ BSN::Optional<uint32_t> m_promotionOrder;
    /* 0054 */ float m_score;
    /* 0058 */ uint8_t m_scorePercent;
};
}}

namespace Battlenet { namespace League {
struct LeagueStatus {
    public:
    /* 0000 */ BSN::Optional<uint32_t> m_practiceGamesRemaining;
    /* 0008 */ BSN::Optional<Battlenet::League::SelfEvaluation::Enum> m_selfEvaluation;
};
}}

namespace Battlenet { namespace League {
struct Team {
    public:
    /* 0000 */ BSN::Array<Battlenet::Toon::FullName, 4> m_members;
};
}}

namespace Battlenet { namespace League {
struct LeagueAssignment {
    public:
    /* 0000 */ Battlenet::League::League::Enum m_league;
    /* 0004 */ uint32_t m_division;
    /* 0008 */ uint32_t m_ladderId;
    /* 000C */ uint32_t m_state;
};
}}

namespace Battlenet { namespace S2Master {
struct CGLMetaData {
    public:
    /* 0000 */ uint32_t m_throttleNumRequests;
    /* 0004 */ uint32_t m_throttlePerHowLong;
};
}}

namespace Battlenet { namespace S2Master {
struct GameStats {
    public:
    /* 0000 */ int32_t m_gameFrequency;
    /* 0004 */ float m_dGameFrequency;
    /* 0008 */ uint32_t m_bookmarkedCount;
    /* 000C */ float m_dBookmarkedCount;
    /* 0010 */ BSN::Optional<uint32_t> m_promotionOrder;
    /* 0018 */ BSN::Optional<uint32_t> m_featured;
    /* 0020 */ uint32_t m_mapCreation;
    /* 0024 */ uint32_t m_mapVersionCreation;
    /* 0028 */ bool m_isBlizzardMap;
    /* 0029 */ bool m_isTrialMap;
    /* 002C */ uint32_t m_timesPlayed;
    /* 0030 */ uint32_t m_totalPlayers;
    /* 0034 */ uint32_t m_hotOrNotPlayersTotal;
    /* 0038 */ uint32_t m_hotOrNotGamesTotal;
    /* 003C */ uint32_t m_hotCount;
    /* 0040 */ uint32_t m_notCount;
    /* 0044 */ uint32_t m_brokenCount;
};
}}

namespace Battlenet { namespace WoWRealm {
struct RealmInfo {
    public:
    struct __subtypeof_m_privilegedData {
        public:
        /* 0000 */ BSN::AsciiString<23> m_version;
        /* 001C */ uint32_t m_configId;
        /* 0020 */ Battlenet::IP4::AddressPort m_address;
    };

    /* 0000 */ uint8_t m_infoFlags;
    /* 0004 */ BSN::String<255> m_name;
    /* 040C */ int32_t m_type;
    /* 0410 */ uint32_t m_category;
    /* 0414 */ uint8_t m_stateFlags;
    /* 0418 */ float m_population;
    /* 041C */ BSN::Optional<Battlenet::WoWRealm::RealmInfo::__subtypeof_m_privilegedData> m_privilegedData;
};
}}

namespace Battlenet { namespace WoWRealm {
struct ToonCountEntry {
    public:
    /* 0000 */ Battlenet::WoW::RealmHandle m_id;
    /* 0008 */ uint16_t m_count;
};
}}

namespace Battlenet { namespace IP6 {
struct AddressPort {
    public:
    /* 0000 */ BSN::FixedBlob<16> m_address;
    /* 0010 */ BSN::FixedBlob<2> m_port;
};
}}

namespace Battlenet { namespace WoW {
struct RealmAddressList {
    public:
    /* 0000 */ BSN::Array<Battlenet::IP4::AddressPort, 16> m_v4;
    /* 0064 */ BSN::Array<Battlenet::IP6::AddressPort, 16> m_v6;
};
}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldStub {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldInt {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
    /* 0028 */ int64_t m_data;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldFloat {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
    /* 0028 */ double m_data;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldBinary {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
    /* 0024 */ BSN::Blob<4096> m_data;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldString {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
    /* 0024 */ BSN::String<1023> m_data;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldCacheHandle {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
    /* 0024 */ BSN::FixedBlob<40> m_data;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldPointer {
    public:
    /* 0000 */ BSN::Blob<32> m_id;
    /* 0028 */ Battlenet::Profile::RecordAddress m_data;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptFieldBlock {
    public:
    /* 0000 */ BSN::Array<Battlenet::Event::Script::ScriptFieldStub, 100> m_stubData;
    /* 0E18 */ BSN::Array<Battlenet::Event::Script::ScriptFieldInt, 100> m_intData;
    /* 20E0 */ BSN::Array<Battlenet::Event::Script::ScriptFieldFloat, 100> m_floatData;
    /* 33A8 */ BSN::Array<Battlenet::Event::Script::ScriptFieldBinary, 10> m_binaryData;
    /* D53C */ BSN::Array<Battlenet::Event::Script::ScriptFieldString, 10> m_stringData;
    /* 176F8 */ BSN::Array<Battlenet::Event::Script::ScriptFieldCacheHandle, 10> m_cacheHandleData;
    /* 179F8 */ BSN::Array<Battlenet::Event::Script::ScriptFieldPointer, 10> m_pointerData;
};
}}}

namespace Battlenet { namespace Event { namespace Script {
struct ScriptProfileResponse : public BSN::Choice {
    public:
    struct Start {
        public:
        /* 0000 */ uint32_t m_numPackets;
        /* 0004 */ uint32_t m_type;
    };

    /* 0004 */ uint8_t m_value[97328];

    ScriptProfileResponse();
};
}}}

namespace Battlenet { namespace Event { namespace Larva {
struct PlayerScoreScreenInfo {
    public:
    /* 0000 */ BSN::FourCC m_team;
    /* 0004 */ uint32_t m_color;
    /* 0008 */ BSN::FourCC m_race;
    /* 000C */ Battlenet::S2Game::PlayerResultCode::Enum m_result;
    /* 0010 */ Battlenet::S2Game::Summary::Role m_role;
    /* 0018 */ Battlenet::Result::PlayerObject m_playerObject;
};
}}}

namespace Battlenet { namespace Client { namespace S2GameDefines {
struct AMMInfo {
    public:
    /* 0000 */ BSN::Optional<double> m_deltaSeasonPoints;
    /* 0010 */ BSN::Optional<double> m_deltaBonusPoints;
    /* 0020 */ BSN::Optional<double> m_seasonRating;
    /* 0030 */ BSN::Optional<Battlenet::League::LeagueAssignment> m_leagueAssignment;
    /* 0044 */ Battlenet::Ladder::Ranking m_teamRanking;
    /* 0050 */ BSN::Optional<uint32_t> m_winExpectancy;
    /* 0058 */ uint32_t m_queueId;
};
}}}

namespace Battlenet { namespace League {
struct LeagueRanking {
    public:
    /* 0000 */ uint32_t m_rank;
    /* 0004 */ Battlenet::League::League::Enum m_leagueId;
};
}}

namespace Battlenet { namespace Client { namespace S2GameDefines {
struct LeagueStatus {
    public:
    /* 0000 */ Battlenet::League::LeagueRanking m_ranking;
    /* 0008 */ Battlenet::Profile::RecordAddress m_teamAddress;
};
}}}

namespace Battlenet { namespace S2Map {
struct PriorRevisionInfo {
    public:
    /* 0000 */ uint32_t m_mapId;
    /* 0004 */ BSN::String<80> m_mapName;
    /* 0150 */ uint32_t m_attributeNs;
    /* 0154 */ bool m_showRID;
    /* 0158 */ uint32_t m_version;
    /* 015C */ bool m_isPrivate;
    /* 015D */ bool m_isLocked;
    /* 0160 */ uint32_t m_mapSize;
};
}}

namespace Battlenet { namespace Friends {
struct PriorFriendId : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[24];

    PriorFriendId();
};
}}

namespace Battlenet { namespace Friends {
struct ToonOfFriend {
    public:
    /* 0000 */ uint32_t m_friendAccountId;
    /* 0004 */ Battlenet::Toon::FullName m_toon;
    /* 0080 */ Battlenet::Profile::RecordAddress m_profile;
};
}}

namespace Battlenet { namespace Toon {
struct Display {
    public:
    /* 0000 */ BSN::String<25> m_name;
    /* 0070 */ uint32_t m_realm;
    /* 0078 */ Battlenet::Profile::RecordAddress m_profile;
    /* 0088 */ int32_t m_lastOnline;
    /* 008C */ uint32_t m_flags;
};
}}

namespace Battlenet { namespace Toon {
struct ToonCreationInput {
    public:
    /* 0000 */ uint32_t m_creationUnlockable;
};
}}

namespace Battlenet { namespace Error {
struct GenericResult : public BSN::Choice {
    public:
    struct Success {
        public:
    };

    /* 0004 */ uint8_t m_value[4];

    GenericResult();
};
}}

namespace Battlenet { namespace Presence {
struct S2GameAdvert : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[12];

    S2GameAdvert();
};
}}

namespace Battlenet { namespace Presence {
struct S2GameInfo {
    public:
    /* 0000 */ Battlenet::S2Map::ShortLink m_shortLink;
    /* 0048 */ Battlenet::Presence::S2GameAdvert m_advert;
};
}}

namespace Battlenet { namespace Statistics {
struct SingleStatistic {
    public:
    /* 0000 */ Battlenet::Statistics::CategoryEnum::Enum m_id;
    /* 0008 */ uint64_t m_number;
};
}}

namespace Battlenet { namespace Event { namespace Larva {
struct Invoke : public BSN::Choice {
    public:
    struct Authentication : public BSN::Choice {
        public:
        struct Module {
            public:
            /* 0000 */ BSN::AsciiString<63> m_function;
            /* 0044 */ uint32_t m_token;
            /* 0048 */ BSN::Array<Battlenet::Event::AuthModule::Parameter, 4> m_parameters;
        };

        struct Success {
            public:
            /* 0000 */ Battlenet::Account::FullName m_fullName;
            /* 0118 */ uint8_t m_accountRegion;
            /* 0119 */ uint8_t m_gameAccountRegion;
            /* 011C */ BSN::AsciiString<32> m_gameAccountName;
            /* 0148 */ uint64_t m_gameAccountFlags;
            /* 0150 */ BSN::FourCC m_program;
        };

        struct Failure {
            public:
            /* 0000 */ uint16_t m_error;
            /* 0004 */ BSN::Array<Battlenet::Event::AuthModule::Parameter, 4> m_parameters;
        };

        struct CachedAccount {
            public:
            /* 0000 */ BSN::String<320> m_account;
        };

        struct CachedGameAccount {
            public:
            /* 0000 */ BSN::Optional<BSN::AsciiString<32>> m_selectedGameAccount;
            /* 002C */ Battlenet::GameAccount::CachedAccountList m_gameAccounts;
        };

        struct CachedToon {
            public:
            /* 0000 */ BSN::String<25> m_toonName;
        };

        struct ModuleBegin {
            public:
        };

        struct ModuleEnd {
            public:
        };

        /* 0004 */ uint8_t m_value[4236];

        Authentication();
    };

    struct Service : public BSN::Choice {
        public:
        struct Error {
            public:
            /* 0000 */ uint16_t m_error;
        };

        struct Warning {
            public:
            /* 0000 */ uint16_t m_error;
        };

        struct Lost {
            public:
        };

        struct FloodingError {
            public:
            /* 0000 */ uint32_t m_depth;
        };

        struct Disconnected {
            public:
        };

        struct Interrupted {
            public:
        };

        struct Recovered {
            public:
        };

        struct Established {
            public:
        };

        /* 0004 */ uint8_t m_value[4];

        Service();
    };

    struct Connection : public BSN::Choice {
        public:
        struct Boom {
            public:
            /* 0000 */ uint16_t m_error;
        };

        /* 0004 */ uint8_t m_value[4];

        Connection();
    };

    struct Block : public BSN::Choice {
        public:
        struct AddAccount {
            public:
            /* 0000 */ uint32_t m_accountId;
            /* 0004 */ Battlenet::Account::FullName m_fullName;
        };

        struct AddCharacter {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_toonName;
        };

        struct RemoveAccount {
            public:
            /* 0000 */ uint32_t m_accountId;
        };

        struct RemoveCharacter {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_toonName;
        };

        /* 0004 */ uint8_t m_value[284];

        Block();
    };

    struct Game : public BSN::Choice {
        public:
        struct Invite {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_toon;
            /* 007C */ uint32_t m_token;
        };

        /* 0004 */ uint8_t m_value[128];

        Game();
    };

    struct Party : public BSN::Choice {
        public:
        struct NonLobbyAttributeChange {
            public:
            /* 0000 */ Battlenet::Attribute::DefinitionHandle m_definitionHandle;
            /* 0008 */ uint8_t m_index;
            /* 000C */ uint32_t m_presenceId;
        };

        struct ReadyCheckNotify {
            public:
            /* 0000 */ uint32_t m_presenceId;
            /* 0004 */ bool m_ready;
        };

        struct BeginReadyProcessNotify {
            public:
            /* 0000 */ Battlenet::Event::Larva::PartyReadyProcessNotify m_process;
        };

        struct ReadyProcessUpdateNotify {
            public:
            /* 0000 */ uint32_t m_presenceId;
            /* 0004 */ uint16_t m_reason;
        };

        struct DownloadUpdateNotify {
            public:
            /* 0000 */ uint32_t m_presenceId;
            /* 0004 */ float m_progress;
            /* 0008 */ uint16_t m_currentFile;
            /* 000A */ uint16_t m_totalFiles;
            /* 000C */ BSN::String<255> m_label;
            /* 0414 */ bool m_isFinished;
        };

        /* 0004 */ uint8_t m_value[1048];

        Party();
    };

    struct Ladder : public BSN::Choice {
        public:
        struct GetRankingsResponse {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0004 */ BSN::Array<Battlenet::Ladder::RankingResponse, 50> m_rankings;
        };

        struct GetMembersResponse {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0004 */ Battlenet::Ladder::GetMembersResult m_result;
        };

        /* 0004 */ uint8_t m_value[320032];

        Ladder();
    };

    struct Advert : public BSN::Choice {
        public:
        struct Post {
            public:
            /* 0000 */ Battlenet::S2Master::AdvertPostResponseData m_data;
        };

        struct PostFailure {
            public:
            /* 0000 */ uint16_t m_reason;
        };

        struct Join {
            public:
            /* 0000 */ Battlenet::S2Master::AdvertJoinResponseData m_data;
        };

        struct JoinFailure {
            public:
            /* 0000 */ uint16_t m_reason;
        };

        struct InviteNotify {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_inviterToon;
            /* 007C */ Battlenet::S2Map::ShortLink m_mapLink;
            /* 00C4 */ Battlenet::S2Master::InstanceHandle m_instanceHandle;
        };

        struct InviteRescindNotify {
            public:
            /* 0000 */ Battlenet::S2Master::InstanceHandle m_instanceHandle;
        };

        struct InviteResult {
            public:
            /* 0000 */ uint32_t m_token;
            /* 0004 */ uint16_t m_failure;
        };

        struct AnnounceNotify {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_inviterToon;
            /* 007C */ Battlenet::S2Map::ShortLink m_mapLink;
            /* 00C4 */ Battlenet::S2Master::AdvertHandle m_advertHandle;
        };

        struct OfflinePost {
            public:
        };

        /* 0004 */ uint8_t m_value[556];

        Advert();
    };

    struct MatchMaker : public BSN::Choice {
        public:
        struct QueueAnnounce {
            public:
            /* 0000 */ BSN::Array<Battlenet::MatchMaker::Handle, 16> m_announcements;
        };

        struct QueueUpdate {
            public:
            /* 0000 */ BSN::Array<Battlenet::MatchMaker::Update, 16> m_updates;
        };

        struct QueueRemove {
            public:
            /* 0000 */ BSN::Array<Battlenet::MatchMaker::Remove, 16> m_removals;
        };

        struct QueueSuccess {
            public:
            /* 0000 */ Battlenet::MatchMaker::Handle m_handle;
        };

        struct MatchingCancelled {
            public:
            /* 0000 */ Battlenet::MatchMaker::Handle m_handle;
            /* 0014 */ BSN::Optional<uint32_t> m_token;
            /* 001C */ uint16_t m_reason;
            /* 0020 */ BSN::Optional<Battlenet::Toon::FullName> m_instigator;
        };

        struct ClientRegisterSuccess {
            public:
            /* 0000 */ BSN::Optional<uint32_t> m_token;
            /* 0008 */ Battlenet::Toon::Handle m_toon;
            /* 0020 */ Battlenet::MatchMaker::Handle m_handle;
            /* 0034 */ Battlenet::MatchMaker::ClientRegisterResponseSuccess m_result;
        };

        struct OpenGames {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint16_t m_startIndex;
            /* 0006 */ uint16_t m_totalNumGames;
            /* 0008 */ BSN::Array<Battlenet::S2Master::ScoredGameInfo, 100> m_games;
            /* 23FC */ bool m_isLastPage;
        };

        struct SubmitSelfEvaluationSuccess {
            public:
            /* 0000 */ Battlenet::League::LeagueStatus m_status;
        };

        struct SubmitSelfEvaluationFailure {
            public:
            /* 0000 */ uint16_t m_reason;
        };

        struct ExitPracticeLeagueSuccess {
            public:
            /* 0000 */ Battlenet::League::LeagueStatus m_status;
        };

        struct ExitPracticeLeagueFailure {
            public:
            /* 0000 */ uint16_t m_reason;
        };

        struct GetLeagueStatusResponse {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                    /* 0000 */ Battlenet::League::LeagueStatus m_status;
                };

                /* 0004 */ uint8_t m_value[16];

                Result();
            };

            /* 0000 */ Battlenet::Event::Larva::Invoke::MatchMaker::GetLeagueStatusResponse::Result m_result;
        };

        struct LeagueAssignNotify {
            public:
            /* 0000 */ Battlenet::MatchMaker::Handle m_mmqHandle;
            /* 0014 */ Battlenet::League::Team m_team;
            /* 0208 */ Battlenet::League::LeagueAssignment m_leagueAssignment;
            /* 0218 */ Battlenet::League::LeagueAssignmentReason::Enum m_reason;
        };

        struct CGLMetaDataInform {
            public:
            /* 0000 */ Battlenet::S2Master::CGLMetaData m_data;
        };

        struct OpenGamesError {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint16_t m_reason;
        };

        struct GamePopularityResponse {
            public:
            /* 0000 */ Battlenet::S2Map::ShortLink m_mapLink;
            /* 0048 */ Battlenet::S2Master::GameStats m_stats;
        };

        struct MapPopularityResponse {
            public:
            /* 0000 */ Battlenet::S2Map::Handle m_handle;
            /* 0008 */ Battlenet::S2Master::GameStats m_stats;
        };

        struct ExitPracticeLeagueNotify {
            public:
        };

        /* 0004 */ uint8_t m_value[9216];

        MatchMaker();
    };

    struct WoW : public BSN::Choice {
        public:
        struct RealmUpdate {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Update {
                    public:
                    /* 0000 */ Battlenet::WoWRealm::RealmInfo m_info;
                };

                struct Delete {
                    public:
                };

                /* 0004 */ uint8_t m_value[1096];

                Result();
            };

            /* 0000 */ Battlenet::WoW::RealmHandle m_id;
            /* 0008 */ Battlenet::Event::Larva::Invoke::WoW::RealmUpdate::Result m_result;
        };

        struct RealmList {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Start {
                    public:
                    /* 0000 */ BSN::Array<Battlenet::WoWRealm::ToonCountEntry, 64> m_toonCounts;
                };

                struct Complete {
                    public:
                };

                struct Failure {
                    public:
                    /* 0000 */ Battlenet::WoW::AuthResult::Enum m_reasonCode;
                };

                /* 0004 */ uint8_t m_value[772];

                Result();
            };

            /* 0000 */ Battlenet::Event::Larva::Invoke::WoW::RealmList::Result m_result;
        };

        struct JoinResponse {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                    /* 0000 */ BSN::FixedBlob<40> m_sessionKey;
                    /* 0028 */ Battlenet::WoW::RealmAddressList m_addresses;
                };

                struct Failure {
                    public:
                    /* 0000 */ Battlenet::WoW::AuthResult::Enum m_reasonCode;
                };

                /* 0004 */ uint8_t m_value[432];

                Result();
            };

            /* 0000 */ Battlenet::Event::Larva::Invoke::WoW::JoinResponse::Result m_result;
        };

        /* 0004 */ uint8_t m_value[1108];

        WoW();
    };

    struct DarkPortal : public BSN::Choice {
        public:
        struct ListStart {
            public:
        };

        struct Add {
            public:
            /* 0000 */ BSN::AsciiString<31> m_name;
        };

        struct ListComplete {
            public:
        };

        /* 0004 */ uint8_t m_value[36];

        DarkPortal();
    };

    struct Profile : public BSN::Choice {
        public:
        struct GameResultsFailure {
            public:
            /* 0000 */ uint16_t m_reason;
            /* 0004 */ uint32_t m_commandId;
        };

        struct GameResults {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Script::ScriptProfileResponse m_result;
        };

        struct ReadResult {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Script::ScriptProfileResponse m_result;
        };

        struct PlayerScoreScreenData {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::Array<Battlenet::Event::Larva::PlayerScoreScreenInfo, 16> m_info;
            /* 0508 */ BSN::Array<Battlenet::Attribute::LocaleTableEntry, 16> m_localeTable;
            /* 1E8C */ BSN::Array<BSN::FixedBlob<40>, 16> m_visualTable;
            /* 2110 */ uint32_t m_matchLength;
        };

        struct AddressNotify {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                    /* 0000 */ Battlenet::Profile::RecordAddress m_address;
                };

                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_error;
                };

                /* 0004 */ uint8_t m_value[16];

                Result();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Larva::Invoke::Profile::AddressNotify::Result m_result;
        };

        struct EOGScoreScreenData {
            public:
            /* 0000 */ Battlenet::S2Game::PlayerResultCode::Enum m_playerResultCode;
            /* 0004 */ uint32_t m_matchLength;
            /* 0008 */ BSN::Optional<Battlenet::Client::S2GameDefines::AMMInfo> m_ammInfo;
            /* 0070 */ Battlenet::S2Map::Link m_mapLink;
            /* 0278 */ BSN::Array<BSN::Optional<Battlenet::Client::S2GameDefines::LeagueStatus>, 16> m_playerLeagues;
        };

        struct ProfileScoreScreenData {
            public:
            /* 0000 */ Battlenet::S2Game::PlayerResultCode::Enum m_gameResultCode;
            /* 0004 */ uint32_t m_matchLength;
            /* 0008 */ Battlenet::S2Map::Link m_mapLink;
        };

        /* 0004 */ uint8_t m_value[97336];

        Profile();
    };

    struct S2Map : public BSN::Choice {
        public:
        struct Info {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0008 */ BSN::Array<Battlenet::S2Map::Info, 100> m_info;
        };

        struct InfoFailure {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::Array<Battlenet::S2Map::Handle, 100> m_failedMaps;
        };

        struct List {
            public:
            struct Type : public BSN::Choice {
                public:
                struct Links {
                    public:
                    /* 0000 */ BSN::Array<Battlenet::S2Map::ShortLink, 100> m_links;
                };

                struct Games {
                    public:
                    /* 0000 */ BSN::Array<Battlenet::S2Master::ScoredGameInfo, 100> m_games;
                };

                /* 0004 */ uint8_t m_value[9204];

                Type();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::MapListChoice m_section;
            /* 0170 */ Battlenet::Event::Larva::Invoke::S2Map::List::Type m_type;
            /* 2568 */ bool m_endOfList;
        };

        struct ListFailure {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::MapListChoice m_section;
            /* 0170 */ uint16_t m_reason;
        };

        struct VersionList {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                    /* 0000 */ BSN::Array<Battlenet::S2Map::Handle, 100> m_handleList;
                };

                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_reason;
                };

                /* 0004 */ uint8_t m_value[804];

                Result();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Larva::Invoke::S2Map::VersionList::Result m_result;
        };

        struct DownloadMapSuccess {
            public:
            /* 0000 */ uint32_t m_commandId;
        };

        struct DownloadMapFailure {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint16_t m_reason;
        };

        struct ListFileDependenciesSuccess {
            public:
            /* 0000 */ BSN::Array<BSN::FixedBlob<40>, 32> m_cacheHandles;
            /* 0504 */ uint32_t m_commandId;
            /* 0508 */ bool m_isLast;
        };

        struct ListFileDependenciesFailure {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint16_t m_reason;
        };

        struct PrePublish {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                    /* 0000 */ uint32_t m_attributeNs;
                    /* 0004 */ BSN::Optional<Battlenet::S2Map::PriorRevisionInfo> m_priorRevision;
                };

                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_reason;
                    /* 0004 */ BSN::Optional<Battlenet::S2Map::PriorRevisionInfo> m_priorRevision;
                    /* 016C */ BSN::Optional<BSN::Array<BSN::String<80>, 16>> m_unavailableNames;
                    /* 1634 */ BSN::Optional<BSN::Array<Battlenet::S2Map::LocalizedName, 16>> m_requiredNames;
                };

                /* 0004 */ uint8_t m_value[11068];

                Result();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Larva::Invoke::S2Map::PrePublish::Result m_result;
        };

        struct PublishMapSuccess {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::S2Map::Handle m_handle;
        };

        struct PublishMapFailure {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ uint16_t m_reason;
        };

        struct PublishMapUpdate {
            public:
            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ BSN::AsciiString<63> m_state;
            /* 0048 */ BSN::String<248> m_currFileHint;
            /* 0438 */ uint64_t m_currFileBytesDone;
            /* 0440 */ uint64_t m_currFileBytesTotal;
            /* 0448 */ uint64_t m_publishBytesDone;
            /* 0450 */ uint64_t m_publishBytesTotal;
        };

        struct SetAuthorPrefs {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                };

                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_reason;
                };

                /* 0004 */ uint8_t m_value[4];

                Result();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Larva::Invoke::S2Map::SetAuthorPrefs::Result m_result;
        };

        struct UnlinkMap {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                };

                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_reason;
                };

                /* 0004 */ uint8_t m_value[4];

                Result();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Larva::Invoke::S2Map::UnlinkMap::Result m_result;
        };

        struct GetStorageInfo {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Success {
                    public:
                    /* 0000 */ uint32_t m_usedBytes;
                    /* 0004 */ uint32_t m_usedSlots;
                    /* 0008 */ uint32_t m_maxBytes;
                    /* 000C */ uint32_t m_maxSlots;
                    /* 0010 */ uint32_t m_maxBytesPerSlot;
                };

                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_reason;
                };

                /* 0004 */ uint8_t m_value[20];

                Result();
            };

            /* 0000 */ uint32_t m_commandId;
            /* 0004 */ Battlenet::Event::Larva::Invoke::S2Map::GetStorageInfo::Result m_result;
        };

        /* 0004 */ uint8_t m_value[95216];

        S2Map();
    };

    struct Cache : public BSN::Choice {
        public:
        struct ResolveResponse {
            public:
            struct Result : public BSN::Choice {
                public:
                struct Failure {
                    public:
                    /* 0000 */ uint16_t m_reason;
                };

                struct Success {
                    public:
                    /* 0000 */ BSN::String<248> m_localPath;
                    /* 03F0 */ uint64_t m_bytesTotal;
                };

                struct Update {
                    public:
                    /* 0000 */ uint64_t m_bytesTotal;
                    /* 0008 */ uint64_t m_bytesPresent;
                    /* 0010 */ uint64_t m_bytesVerified;
                };

                /* 0004 */ uint8_t m_value[1016];

                Result();
            };

            /* 0000 */ BSN::FourCC m_invokeSource;
            /* 0004 */ uint32_t m_commandId;
            /* 0008 */ Battlenet::Event::Larva::Invoke::Cache::ResolveResponse::Result m_result;
        };

        /* 0004 */ uint8_t m_value[1056];

        Cache();
    };

    struct Friend : public BSN::Choice {
        public:
        struct AddStalkedFriend {
            public:
            /* 0000 */ uint32_t m_id;
            /* 0004 */ Battlenet::Toon::FullName m_name;
            /* 0080 */ Battlenet::Profile::RecordAddress m_profile;
            /* 0090 */ BSN::Optional<BSN::String<127>> m_note;
        };

        struct AddGameAccountFriend {
            public:
            /* 0000 */ uint32_t m_id;
            /* 0004 */ uint32_t m_accountId;
        };

        struct AddRIDFriend {
            public:
            /* 0000 */ uint32_t m_accountId;
            /* 0004 */ Battlenet::Account::FullName m_fullName;
            /* 011C */ uint8_t m_region;
            /* 0120 */ Battlenet::Profile::RecordAddress m_profile;
            /* 0130 */ BSN::Optional<BSN::String<127>> m_customMsg;
            /* 033C */ BSN::Optional<BSN::String<127>> m_note;
            /* 0548 */ int32_t m_lastOnline;
        };

        struct RemoveFriend {
            public:
            /* 0000 */ Battlenet::Friends::PriorFriendId m_id;
        };

        struct AddFriendInvite {
            public:
            /* 0000 */ uint32_t m_accountId;
            /* 0004 */ uint8_t m_region;
            /* 0008 */ BSN::FourCC m_locale;
            /* 000C */ Battlenet::Account::FullName m_fullName;
            /* 0128 */ Battlenet::Profile::RecordAddress m_profile;
            /* 0138 */ BSN::Optional<BSN::String<127>> m_msg;
            /* 0344 */ bool m_popupToast;
            /* 0348 */ int32_t m_createdTime;
            /* 034C */ BSN::Optional<bool> m_isEndOfList;
        };

        struct RemoveFriendInvite {
            public:
            /* 0000 */ uint32_t m_accountId;
        };

        struct InviteFailed {
            public:
            /* 0000 */ BSN::Optional<Battlenet::Account::FullName> m_fullName;
            /* 011C */ uint16_t m_reason;
        };

        struct RegistryData {
            public:
            /* 0000 */ BSN::String<255> m_key;
            /* 0408 */ BSN::String<255> m_name;
            /* 0810 */ Battlenet::Registry::FriendDataVersion1 m_value;
        };

        struct UpdateFriendsList {
            public:
            /* 0000 */ BSN::Optional<bool> m_isEndOfList;
        };

        struct ToonOfFriend {
            public:
            /* 0000 */ Battlenet::Friends::ToonOfFriend m_toon;
            /* 0090 */ bool m_isEndOfList;
        };

        /* 0004 */ uint8_t m_value[2068];

        Friend();
    };

    struct Toon : public BSN::Choice {
        public:
        struct List {
            public:
            /* 0000 */ BSN::Array<Battlenet::Toon::Display, 50> m_list;
        };

        struct CreationData {
            public:
            /* 0000 */ Battlenet::Toon::ToonCreationInput m_input;
        };

        struct Created {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_toonName;
            /* 0080 */ Battlenet::Profile::RecordAddress m_profileLocation;
        };

        struct Failure {
            public:
            /* 0000 */ uint16_t m_error;
        };

        struct Success {
            public:
            /* 0000 */ Battlenet::Toon::FullName m_toonName;
            /* 0080 */ Battlenet::Toon::Handle m_toonHandle;
            /* 0098 */ Battlenet::Profile::RecordAddress m_profileLocation;
        };

        struct InitialNotifiesComplete {
            public:
        };

        struct SetProfileResponse {
            public:
            /* 0000 */ Battlenet::Error::GenericResult m_result;
        };

        /* 0004 */ uint8_t m_value[7208];

        Toon();
    };

    struct Presence : public BSN::Choice {
        public:
        struct TriggerUpdate {
            public:
            /* 0000 */ uint32_t m_id;
            /* 0004 */ BSN::Optional<bool> m_online;
            /* 0008 */ BSN::Optional<Battlenet::Toon::FullName> m_name;
            /* 0088 */ BSN::Optional<uint32_t> m_accountId;
            /* 0090 */ BSN::Optional<uint8_t> m_region;
            /* 0094 */ BSN::Optional<Battlenet::Account::FullName> m_fullName;
            /* 01B0 */ BSN::Optional<bool> m_hasVoice;
            /* 01B2 */ BSN::Optional<bool> m_afk;
            /* 01B4 */ BSN::Optional<bool> m_dnd;
            /* 01B8 */ BSN::Optional<BSN::String<127>> m_msg;
            /* 03C4 */ BSN::Optional<int32_t> m_time;
            /* 03CC */ BSN::Optional<uint32_t> m_visVersion;
            /* 03D4 */ BSN::Optional<bool> m_inGameS2;
            /* 03D6 */ bool m_s2GameInfoUpdated;
            /* 03D8 */ BSN::Optional<Battlenet::Presence::S2GameInfo> m_s2GameInfo;
            /* 0438 */ BSN::Optional<Battlenet::Profile::RecordAddress> m_record;
            /* 0450 */ BSN::Optional<int32_t> m_focus;
            /* 0458 */ BSN::Optional<BSN::FourCC> m_locale;
            /* 0460 */ BSN::Optional<BSN::FourCC> m_gameProgram;
        };

        struct StatisticsUpdate {
            public:
            /* 0000 */ BSN::Array<Battlenet::Statistics::SingleStatistic, 15> m_statistics;
        };

        /* 0004 */ uint8_t m_value[1128];

        Presence();
    };

    struct Support : public BSN::Choice {
        public:
        struct FeedbackInfo {
            public:
            /* 0000 */ BSN::AsciiString<640> m_token;
        };

        /* 0004 */ uint8_t m_value[648];

        Support();
    };

    /* 0004 */ uint8_t m_value[320036];

    Invoke();
};
}}}

namespace Battlenet { namespace Version {
struct Patch {
    public:
    /* 0000 */ BSN::FourCC m_programId;
    /* 0004 */ BSN::FourCC m_component;
    /* 0008 */ BSN::AsciiString<255> m_instructions;
};
}}

namespace Battlenet { namespace Event { namespace Update {
struct Invoke {
    public:
    /* 0000 */ Battlenet::Version::Patch m_patch;
    /* 010C */ bool m_more;
};
}}}

namespace Battlenet { namespace Chat {
struct MemberList {
    public:
    /* 0000 */ BSN::Array<uint32_t, 12> m_list;
};
}}

namespace Battlenet { namespace Presence {
struct StringTableEntry {
    public:
    /* 0000 */ uint16_t m_tableId;
    /* 0002 */ uint16_t m_offset;
};
}}

namespace Battlenet { namespace Presence {
struct ImageTableEntry {
    public:
    /* 0000 */ uint16_t m_tableId;
    /* 0002 */ uint16_t m_offset;
};
}}

namespace Battlenet { namespace Presence {
struct AccountInfo {
    public:
    /* 0000 */ uint32_t m_accountId;
    /* 0004 */ uint8_t m_region;
    /* 0008 */ Battlenet::Account::FullName m_fullName;
};
}}

namespace Battlenet { namespace GameAccount {
struct Handle {
    public:
    /* 0000 */ uint8_t m_region;
    /* 0004 */ BSN::FourCC m_programId;
    /* 0008 */ uint32_t m_id;
};
}}

namespace Battlenet { namespace Presence {
struct FieldVal : public BSN::Choice {
    public:
    /* 0004 */ uint8_t m_value[520];

    FieldVal();
};
}}

namespace Battlenet { namespace Presence {
struct BlockList {
    public:
    /* 0000 */ BSN::Array<uint32_t, 100> m_ridBlocks;
    /* 0194 */ BSN::Array<uint32_t, 100> m_cidBlocks;
};
}}

namespace Battlenet { namespace Presence {
struct FieldUpdate {
    public:
    /* 0000 */ uint32_t m_id;
    /* 0004 */ uint32_t m_handle;
    /* 0008 */ Battlenet::Presence::FieldVal m_value;
};
}}

namespace Battlenet { namespace Friends {
struct FriendsOfFriendList {
    public:
    /* 0000 */ uint32_t m_friendId;
    /* 0004 */ BSN::Array<uint32_t, 32> m_friends;
};
}}

namespace Battlenet { namespace Friends {
struct FriendInviteContainer {
    public:
    /* 0000 */ uint32_t m_presenceId;
    /* 0004 */ BSN::Optional<BSN::String<127>> m_msg;
    /* 0210 */ bool m_popupToast;
    /* 0214 */ int32_t m_createdTime;
};
}}

namespace Battlenet { namespace Friends {
struct FriendList {
    public:
    /* 0000 */ BSN::Array<uint32_t, 32> m_friends;
};
}}

namespace Battlenet { namespace Version {
struct Record {
    public:
    /* 0000 */ BSN::FourCC m_programId;
    /* 0004 */ BSN::FourCC m_component;
    /* 0008 */ uint32_t m_version;
};
}}

namespace Battlenet { namespace Authentication {
struct Settings {
    public:
    /* 0000 */ BSN::Blob<1023> m_seed;
    /* 0404 */ BSN::FourCC m_program;
    /* 0408 */ BSN::FourCC m_platform;
    /* 040C */ BSN::FourCC m_productLocale;
    /* 0410 */ BSN::FourCC m_textLocale;
    /* 0414 */ BSN::Optional<BSN::String<248>> m_cacheOverridePath;
    /* 0804 */ BSN::Array<Battlenet::Version::Record, 63> m_versions;

    Settings();
};
}}

namespace Battlenet { namespace Client {
struct DNSInfo {
    public:
    /* 0000 */ BSN::Array<BSN::FixedBlob<4>, 127> m_addresses;
    /* 0200 */ BSN::AsciiString<255> m_canonicalName;
};
}}

#include "WoW/Source/Battlenet/BattlenetProtocolTypes.inl"

#endif
