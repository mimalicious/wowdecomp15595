#include "CGTabardCreationFrame.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F71070
WOWGUID CGTabardCreationFrame::m_vendor;

// PROVEN: b2cf592f IDENTICAL/NONE
// OFFSET: 0x009DFF70/0x140776AD0
void CGTabardCreationFrame::EnterWorld() {
    CGTabardCreationFrame::m_vendor = WOWGUID();
}

// OFFSET: 0x009E0010
void CGTabardCreationFrame::Open(const WOWGUID& vendor) {
    CGTabardCreationFrame::m_vendor = vendor;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TurnOnGuildTabardPurchase();
    }

    CGGossipInfo::ClearGossip();
    CGGameUI::SetInteractTarget(vendor);
    FrameScript_SignalEvent(SCRIPT_EVENT_OPEN_TABARD_FRAME, NULL);
}

// OFFSET: 0x009E0070/0x140776C70
void CGTabardCreationFrame::Close() {
    if (!CGTabardCreationFrame::m_vendor) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->TurnOffGuildTabardPurchase();
    }

    CGGameUI::ClearInteractTarget(CGTabardCreationFrame::m_vendor, 0);
    CGTabardCreationFrame::m_vendor = WOWGUID();
    FrameScript_SignalEvent(SCRIPT_EVENT_CLOSE_TABARD_FRAME, NULL);
}

// OFFSET: 0x009E00F0
void CGTabardCreationFrame::LeaveWorld() {
    CGTabardCreationFrame::Close();
}

// OFFSET: 0x00B0F1A0/0x1408D44F0
bool GuildGetGuildTabard(WOWGUID requester, WOWGUID guild, void (*callback)(const uint64_t& key, const WOWGUID& guid, void* param, bool success), void* param, int32_t& emblemStyle, int32_t& emblemColor, int32_t& borderStyle, int32_t& borderColor, int32_t& background) {
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(guild.m_guid, requester, callback, param, false);
    emblemStyle = -1;
    emblemColor = -1;
    borderStyle = -1;
    borderColor = -1;
    background = -1;

    if (!stats || stats->m_emblemStyle == -1 || stats->m_emblemColor == -1 || stats->m_borderStyle == -1 || stats->m_borderColor == -1 || stats->m_background == -1) {
        return false;
    }

    emblemStyle = stats->m_emblemStyle;
    emblemColor = stats->m_emblemColor;
    borderStyle = stats->m_borderStyle;
    borderColor = stats->m_borderColor;
    background = stats->m_background;
    return true;
}

// PROVEN: 13228d94 IDENTICAL/NONE
// OFFSET: 0x00B0F260/0x1408D45D0
uint32_t GuildGetTabardCost() {
    return 100000;
}
