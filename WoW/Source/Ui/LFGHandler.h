#ifndef WOW_SOURCE_UI_LFGHANDLER_H
#define WOW_SOURCE_UI_LFGHANDLER_H

class LFGLIST;
class LFGListParty;
class LFGListPlayer;
struct JamLfgSearchResultParty;
struct JamLfgSearchResultPlayer;

namespace LFGHandler {
    void ReadGroupInfo(const JamLfgSearchResultParty* source, LFGListParty* party);
    void ReadPlayerInfo(const JamLfgSearchResultPlayer* source, LFGLIST* list, LFGListPlayer* player);
}

#endif
