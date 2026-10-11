#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMGLOBAL_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMGLOBAL_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

struct JamGuildMemberSetNoteData {
    /* 0000 */ uint8_t m_memberGUID[8];
    /* 0008 */ char m_note[128];
    /* 0088 */ bool m_isPublic;

    JamGuildMemberSetNoteData(uint64_t memberGUID, const char* note, bool isPublic);
    JamGuildMemberSetNoteData(const JamGuildMemberSetNoteData& other);
    ~JamGuildMemberSetNoteData();
};

class GlobalGuildEventLogQuery : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalGuildEventLogQuery();
    ~GlobalGuildEventLogQuery();
};

class GlobalGuildBankRemainingWithdrawMoneyQuery : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalGuildBankRemainingWithdrawMoneyQuery();
    ~GlobalGuildBankRemainingWithdrawMoneyQuery();
};

class GlobalGuildGetRoster : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];
    /* 0018 */ uint8_t m_playerGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildGetRoster();
    ~GlobalGuildGetRoster();
};

class GlobalGuildSetRankPermissions : public JamClientMessage {
    public:
    /* 0010 */ int32_t m_rankOrder;
    /* 0014 */ int32_t m_rankID;
    /* 0018 */ uint32_t m_oldFlags;
    /* 001C */ uint32_t m_newFlags;
    /* 0020 */ char m_rankName[64];
    /* 0060 */ int32_t m_withdrawGoldLimit;
    /* 0064 */ int32_t m_tabWithdrawItemLimit[8];
    /* 0084 */ uint32_t m_tabFlags[8];

    void Put(CDataStore* msg) override;

    GlobalGuildSetRankPermissions();
    ~GlobalGuildSetRankPermissions();
};

class GlobalGuildNewsUpdateSticky : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];
    /* 0018 */ bool m_sticky;
    /* 001C */ int32_t m_newsID;

    void Put(CDataStore* msg) override;

    GlobalGuildNewsUpdateSticky(uint64_t guildGUID, int32_t newsID, bool sticky);
    ~GlobalGuildNewsUpdateSticky();
};

class GlobalGuildBankTextQuery : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_tab;

    void Put(CDataStore* msg) override;

    GlobalGuildBankTextQuery(uint32_t tab);
    ~GlobalGuildBankTextQuery();
};

class GlobalGuildChangeNameRequest : public JamClientMessage {
    public:
    /* 0010 */ char m_newName[96];

    void Put(CDataStore* msg) override;

    GlobalGuildChangeNameRequest();
    ~GlobalGuildChangeNameRequest();
};

class GlobalGuildAutoDeclineInvitation : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalGuildAutoDeclineInvitation();
    ~GlobalGuildAutoDeclineInvitation();
};

class GlobalGuildShiftRank : public JamClientMessage {
    public:
    /* 0010 */ bool m_shiftUp;
    /* 0014 */ int32_t m_rankOrder;

    void Put(CDataStore* msg) override;

    GlobalGuildShiftRank(int32_t rankOrder, bool shiftUp);
    ~GlobalGuildShiftRank();
};

class GlobalGuildUpdateInfoText : public JamClientMessage {
    public:
    /* 0010 */ char m_infoText[2004];

    void Put(CDataStore* msg) override;

    GlobalGuildUpdateInfoText(const char* infoText);
    ~GlobalGuildUpdateInfoText();
};

class GlobalGuildQueryRecipes : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildQueryRecipes(uint64_t guildGUID);
    ~GlobalGuildQueryRecipes();
};

class GlobalGuildBankLogQuery : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_tab;

    void Put(CDataStore* msg) override;

    GlobalGuildBankLogQuery(uint32_t tab);
    ~GlobalGuildBankLogQuery();
};

class GlobalGuildDeleteRank : public JamClientMessage {
    public:
    /* 0010 */ int32_t m_rankOrder;

    void Put(CDataStore* msg) override;

    GlobalGuildDeleteRank(int32_t rankOrder);
    ~GlobalGuildDeleteRank();
};

class GlobalGuildAddRank : public JamClientMessage {
    public:
    /* 0010 */ char m_name[64];
    /* 0050 */ int32_t m_rankOrder;

    void Put(CDataStore* msg) override;

    GlobalGuildAddRank(const char* name, int32_t rankOrder);
    ~GlobalGuildAddRank();
};

class GlobalGuildChallengeUpdateRequest : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalGuildChallengeUpdateRequest();
    ~GlobalGuildChallengeUpdateRequest();
};

class GlobalGuildPermissionsQuery : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalGuildPermissionsQuery();
    ~GlobalGuildPermissionsQuery();
};

class GlobalGuildQueryMemberRecipes : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];
    /* 0018 */ uint8_t m_memberGUID[8];
    /* 0020 */ int32_t m_skillLineID;

    void Put(CDataStore* msg) override;

    GlobalGuildQueryMemberRecipes(uint64_t memberGUID, uint64_t guildGUID, int32_t skillLineID);
    ~GlobalGuildQueryMemberRecipes();
};

class GlobalGuildRequestGuildXP : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildRequestGuildXP(uint64_t guildGUID);
    ~GlobalGuildRequestGuildXP();
};

class GlobalGuildAssignMemberRank : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_memberGUID[8];
    /* 0018 */ uint8_t m_setterGUID[8];
    /* 0020 */ int32_t m_rankOrder;

    void Put(CDataStore* msg) override;

    GlobalGuildAssignMemberRank(uint64_t setterGUID, uint64_t memberGUID, int32_t rankOrder);
    ~GlobalGuildAssignMemberRank();
};

class GlobalGuildQueryNews : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildQueryNews(uint64_t guildGUID);
    ~GlobalGuildQueryNews();
};

class GlobalGuildBankSetTabText : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_tab;
    /* 0014 */ char m_tabText[8192];

    void Put(CDataStore* msg) override;

    GlobalGuildBankSetTabText();
    ~GlobalGuildBankSetTabText();
};

class GlobalGuildSendSoRRequest : public JamClientMessage {
    public:
    /* 0010 */ char m_text[1024];
    /* 0410 */ uint8_t m_targetGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildSendSoRRequest();
    ~GlobalGuildSendSoRRequest();
};

class GlobalGuildRequestMaxDailyXP : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildRequestMaxDailyXP(uint64_t guildGUID);
    ~GlobalGuildRequestMaxDailyXP();
};

class GlobalGuildGetRanks : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    GlobalGuildGetRanks(uint64_t guildGUID);
    ~GlobalGuildGetRanks();
};

class GlobalGuildReplaceGuildMaster : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalGuildReplaceGuildMaster();
    ~GlobalGuildReplaceGuildMaster();
};

class GlobalGuildQueryMembersForRecipe : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];
    /* 0018 */ int32_t m_uniqueBit;
    /* 001C */ int32_t m_skillLineID;
    /* 0020 */ int32_t m_spellID;

    void Put(CDataStore* msg) override;

    GlobalGuildQueryMembersForRecipe(uint64_t guildGUID, int32_t skillLineID, int32_t spellID, int32_t uniqueBit);
    ~GlobalGuildQueryMembersForRecipe();
};

class GlobalGuildSetMemberNote : public JamClientMessage {
    public:
    /* 0010 */ JamGuildMemberSetNoteData m_noteData;

    void Put(CDataStore* msg) override;

    GlobalGuildSetMemberNote(const JamGuildMemberSetNoteData& noteData);
    ~GlobalGuildSetMemberNote();
};

class GlobalLFGuildGetGuildPost : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalLFGuildGetGuildPost();
    ~GlobalLFGuildGetGuildPost();
};

class GlobalLFGuildGetRecruits : public JamClientMessage {
    public:
    /* 0010 */ int32_t m_lastUpdate;

    void Put(CDataStore* msg) override;

    GlobalLFGuildGetRecruits(int32_t lastUpdate);
    ~GlobalLFGuildGetRecruits();
};

class GlobalLFGuildGetApplications : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    GlobalLFGuildGetApplications();
    ~GlobalLFGuildGetApplications();
};

class GlobalLFGuildDeclineRecruit : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_recruitGUID[8];

    void Put(CDataStore* msg) override;

    GlobalLFGuildDeclineRecruit(uint64_t recruitGUID);
    ~GlobalLFGuildDeclineRecruit();
};

class GlobalLFGuildRemoveRecruit : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    GlobalLFGuildRemoveRecruit();
    ~GlobalLFGuildRemoveRecruit();
};

class GlobalLFGuildSetGuildPost : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_classRoles;
    /* 0014 */ char m_comment[1024];
    /* 0414 */ bool m_listed;
    /* 0418 */ uint32_t m_availability;
    /* 041C */ uint32_t m_interests;
    /* 0420 */ uint32_t m_level;

    void Put(CDataStore* msg) override;

    GlobalLFGuildSetGuildPost();
    ~GlobalLFGuildSetGuildPost();
};

class GlobalLFGuildBrowse : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_availability;
    /* 0014 */ uint32_t m_interests;
    /* 0018 */ int32_t m_characterLevel;
    /* 001C */ uint32_t m_classRoles;

    void Put(CDataStore* msg) override;

    GlobalLFGuildBrowse();
    ~GlobalLFGuildBrowse();
};

#endif
