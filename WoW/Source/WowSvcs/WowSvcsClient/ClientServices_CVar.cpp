#include "ClientServices.h"
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WowServices/InstallLocation.h"

// STATIC: 0x00DBE7F8/0x140CA4318
CVar* ClientServices::s_realmListBNVar;

// STATIC: 0x00DBE7FC/0x140CA4320
CVar* ClientServices::s_portalVar;

// STATIC: 0x00DBE800/0x140CA4328
CVar* ClientServices::s_realmListVar;

// STATIC: 0x00DBE804/0x140CA4330
CVar* ClientServices::s_serverAlertVar;

// STATIC: 0x00DBE808/0x140CA4338
CVar* ClientServices::s_realmNameVar;

// STATIC: 0x00DBE80C/0x140CA4340
CVar* ClientServices::s_decorateAccountName;

// STATIC: 0x00DBE810/0x140CA4348
CVar* ClientServices::s_patchListVar;

// OFFSET: 0x004CEBF0/0x14010AC70
const char* ClientServices::GetDefaultPortalString() {
    const char* portal = "";

    switch (LocaleNameToIndex(s_localeCode)) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 11:
        case 14:
            return "eu";

        case 2:
        case 10:
            portal = "us";
            break;

        case 5:
            return "kr";

        case 6:
        case 8:
            return "cn";

        case 7:
        case 9:
            return "tw";

        default:
            break;
    }

    return portal;
}

// OFFSET: 0x004CEC70/0x14010AD30
const char* ClientServices::GetDefaultRealmlistString() {
    const char* portal = ClientServices::s_portalVar->m_stringValue.m_str;

    if (!portal) {
        return "us.logon.worldofwarcraft.com:3724";
    }

    if (!_stricmp(portal, "eu")) {
        return "eu.logon.worldofwarcraft.com";
    }

    if (!_stricmp(portal, "kr")) {
        return "kr.logon.worldofwarcraft.com";
    }

    if (!_stricmp(portal, "cn")) {
        return "cn.logon.warcraftchina.com";
    }

    if (!_stricmp(portal, "tw")) {
        return "tw.logon.worldofwarcraft.com";
    }

    if (!_stricmp(portal, "us")) {
        return "us.logon.worldofwarcraft.com";
    }

    return "us.logon.worldofwarcraft.com:3724";
}

// OFFSET: 0x004CED20/0x14010AE30
const char* ClientServices::GetDefaultPatchListString() {
    if (!_stricmp(ClientServices::s_portalVar->m_stringValue.m_str, "public-test") || GetInstallLocation() == INSTALL_LOCATION_TEST) {
        return "public-test.patch.battle.net:1119/patch";
    }

    const char* portal = ClientServices::s_portalVar->m_stringValue.m_str;

    if (!_stricmp(portal, "eu")) {
        return "deDE.patch.battle.net:1119/patch";
    }

    if (!_stricmp(portal, "kr")) {
        return "koKR.patch.battle.net:1119/patch";
    }

    if (!_stricmp(portal, "cn")) {
        return "zhCN.patch.battle.net:1119/patch";
    }

    if (!_stricmp(portal, "tw")) {
        return "zhTW.patch.battle.net:1119/patch";
    }

    if (!_stricmp(portal, "us")) {
        return "enUS.patch.battle.net:1119/patch";
    }

    return "streaming.corp.blizzard.net/patchlist/ldap.php";
}

// OFFSET: 0x004CEDF0/0x14010AF50
void ClientServices::InitLoginServerCVars() {
    ClientServices::s_decorateAccountName = CVar::Register("decorateAccountName", "", 0, "0", NULL, CATEGORY_NET, false, NULL, false);

    char localeDir[260];

    if (s_localeCode[0]) {
        SStrPrintf(localeDir, sizeof(localeDir), "data\\%s\\", s_localeCode);
    } else {
        localeDir[0] = '\0';
    }

    char path[260];

    ClientServices::s_realmListBNVar = CVar::Register("realmListbn", "Address of Battle.net server", 0, "", NULL, CATEGORY_NET, false, NULL, false);
    SStrPrintf(path, sizeof(path), "%srealmlistbn.wtf", localeDir);

    if (!CVar::Load(path, NULL, NULL)) {
        CVar::Load("realmlistbn.wtf", NULL, NULL);
    }

    SStrPrintf(path, sizeof(path), "%srealmlist.wtf", localeDir);

    if (!CVar::Load(path, NULL, NULL)) {
        CVar::Load("realmlist.wtf", NULL, NULL);
    }

    ClientServices::s_portalVar = CVar::Register("portal", "Name of Battle.net portal to use", 0, ClientServices::GetDefaultPortalString(), NULL, CATEGORY_NET, false, NULL, false);
    ClientServices::s_serverAlertVar = CVar::Register("serverAlert", "Get the glue-string tag for the URL", 0, "SERVER_ALERT_URL", NULL, CATEGORY_NET, false, NULL, false);
    ClientServices::s_realmListVar = CVar::Register("realmList", "Address of realm list server", 0, ClientServices::GetDefaultRealmlistString(), NULL, CATEGORY_NET, false, NULL, false);
    ClientServices::s_patchListVar = CVar::Register("patchList", "Address of patch server", 0, ClientServices::GetDefaultPatchListString(), NULL, CATEGORY_NET, false, NULL, false);
}
