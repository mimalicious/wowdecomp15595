// INLINED
inline JamGuildNewsEvent::JamGuildNewsEvent(const JamGuildNewsEvent& other) {
    *this = other;
}

// INLINED
inline void JamLFGuildSettings::operator=(const JamLFGuildSettings& source) {
    this->m_availability = source.m_availability;
    this->m_classRoles = source.m_classRoles;
    this->m_level = source.m_level;
    this->m_interests = source.m_interests;
    this->unk_0010 = source.unk_0010;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
}

// INLINED
inline void JamGuildBankSocketEnchant::operator=(const JamGuildBankSocketEnchant& source) {
    this->m_socketEnchantID = source.m_socketEnchantID;
    this->m_socketIndex = source.m_socketIndex;
}

// INLINED
inline void ClientLFGBlackListSlot::operator=(const ClientLFGBlackListSlot& source) {
    this->unk_0000 = source.unk_0000;
    this->unk_000C = source.unk_000C;
    this->unk_0008 = source.unk_0008;
    this->unk_0004 = source.unk_0004;
}

// INLINED
inline JamCliGuildRankData::JamCliGuildRankData() {
}

// INLINED
inline JamCliGuildRankData::JamCliGuildRankData(const JamCliGuildRankData& source) {
    *this = source;
}

// INLINED
inline JamClientCommentatorPartyMember::JamClientCommentatorPartyMember() {
}

// INLINED
inline JamClientCommentatorPartyMember::JamClientCommentatorPartyMember(const JamClientCommentatorPartyMember& source) {
    *this = source;
}

// INLINED
inline JamGuildBankTabInfo::JamGuildBankTabInfo() {
}

// INLINED
inline JamGuildBankTabInfo::JamGuildBankTabInfo(const JamGuildBankTabInfo& source) {
    *this = source;
}

// INLINED
inline JamCliServerInfoLine::JamCliServerInfoLine() {
}

// INLINED
inline JamCliServerInfoLine::JamCliServerInfoLine(const JamCliServerInfoLine& source) {
    *this = source;
}

// INLINED
inline JamLfgSearchResultParty::JamLfgSearchResultParty() {
}

// INLINED
inline JamLfgSearchResultParty::JamLfgSearchResultParty(const JamLfgSearchResultParty& source) {
    *this = source;
}

// INLINED
inline ClientGuildRosterMemberData::ClientGuildRosterMemberData() {
}

// INLINED
inline ClientGuildRosterMemberData::ClientGuildRosterMemberData(const ClientGuildRosterMemberData& source) {
    *this = source;
}

// INLINED
inline JamClientCharacterListEntry::JamClientCharacterListEntry() {
}

// INLINED
inline JamClientCharacterListEntry::JamClientCharacterListEntry(const JamClientCharacterListEntry& source) {
    *this = source;
}

// INLINED
inline JamLfgSearchResultPlayer::JamLfgSearchResultPlayer() {
}

// INLINED
inline JamLfgSearchResultPlayer::JamLfgSearchResultPlayer(const JamLfgSearchResultPlayer& source) {
    *this = source;
}

// INLINED
inline JamCliCUFProfile::JamCliCUFProfile(const JamCliCUFProfile& source) {
    *this = source;
}

// INLINED
inline JamGuildBankLogEntry::JamGuildBankLogEntry() {
    this->m_hasMoney = false;
    this->m_hasItemID = false;
    this->m_hasCount = false;
    this->m_hasOtherTab = false;
}

// INLINED
inline JamGuildBankLogEntry::JamGuildBankLogEntry(const JamGuildBankLogEntry& source) {
    *this = source;
}

// INLINED
inline ClientPVPLogData_Player::ClientPVPLogData_Player() {
    this->m_hasHonor = false;
    this->m_hasRating = false;
    this->m_hasRatingChange = false;
    this->m_hasPreMatchMMR = false;
    this->m_hasMmrChange = false;
}

// INLINED
inline ClientPVPLogData_Player::ClientPVPLogData_Player(const ClientPVPLogData_Player& source) {
    *this = source;
}

// INLINED
inline JamQuestCompletionNPC::JamQuestCompletionNPC() {
}

// INLINED
inline JamQuestCompletionNPC::JamQuestCompletionNPC(const JamQuestCompletionNPC& source) {
    *this = source;
}

// INLINED
inline JamGuildBankItemInfo::JamGuildBankItemInfo() {
}

// INLINED
inline JamGuildBankItemInfo::JamGuildBankItemInfo(const JamGuildBankItemInfo& source) {
    *this = source;
}

// INLINED
inline ClientLFGBlackList::ClientLFGBlackList() {
}

// INLINED
inline ClientLFGBlackList::ClientLFGBlackList(const ClientLFGBlackList& source) {
    *this = source;
}

// INLINED
inline LFGuildBrowseData::LFGuildBrowseData() {
}

// INLINED
inline LFGuildBrowseData::LFGuildBrowseData(const LFGuildBrowseData& source) {
    *this = source;
}

// INLINED
inline LFGuildRecruitData::LFGuildRecruitData() {
}

// INLINED
inline LFGuildRecruitData::LFGuildRecruitData(const LFGuildRecruitData& source) {
    *this = source;
}

// INLINED
inline LFGuildApplicationData::LFGuildApplicationData() {
}

// INLINED
inline LFGuildApplicationData::LFGuildApplicationData(const LFGuildApplicationData& source) {
    *this = source;
}

// INLINED
inline JamGuildRewardItem::JamGuildRewardItem() {
}

// INLINED
inline JamGuildRewardItem::JamGuildRewardItem(const JamGuildRewardItem& source) {
    *this = source;
}

// INLINED
inline JamCliSupportTicketChatLine::JamCliSupportTicketChatLine() {
}

// INLINED
inline JamCliSupportTicketChatLine::JamCliSupportTicketChatLine(const JamCliSupportTicketChatLine& source) {
    *this = source;
}

// INLINED
inline bool LFGuildBrowseData::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_comment))) {
                break;
            }

            terminated |= this->m_comment[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
                break;
            }

            terminated |= this->m_name[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// INLINED
inline bool LFGuildRecruitData::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
                break;
            }

            terminated |= this->m_name[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_comment))) {
                break;
            }

            terminated |= this->m_comment[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// INLINED
inline bool LFGuildApplicationData::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_guildName))) {
                break;
            }

            terminated |= this->m_guildName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_comment))) {
                break;
            }

            terminated |= this->m_comment[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// INLINED
inline bool JamCliServerInfoLine::IsValid() const {
    {
        bool terminated = false;

        for (uint32_t i = 0; !terminated; i++) {
            if (i >= sizeof(this->m_value)) {
                break;
            }

            terminated |= this->m_value[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (uint32_t i = 0; !terminated; i++) {
            if (i >= sizeof(this->m_name)) {
                break;
            }

            terminated |= this->m_name[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// INLINED
inline bool JamGuildBankTabInfo::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
                break;
            }

            terminated |= this->m_name[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_icon))) {
                break;
            }

            terminated |= this->m_icon[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}
