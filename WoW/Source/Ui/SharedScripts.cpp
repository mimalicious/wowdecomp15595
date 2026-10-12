#include "SharedScripts.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D3AFD0/0x140BF6350
char s_visitableURLs[4][1024];

// OFFSET: 0x004814D0/0x1400A0050
const char* GetVisitableURL(int32_t index) {
    return s_visitableURLs[index];
}

// OFFSET: 0x004814E0/0x1400A0070
void FormatVisitableURL(int32_t index, char* buffer, uint32_t size) {
    if (index != 2) {
        SStrPrintf(buffer, size, s_visitableURLs[0], index);
        return;
    }

    Login* login = ClientServices::LoginConnection();
    SStrPrintf(buffer, size, s_visitableURLs[2], login ? login->GetAccountName() : ClientServices::GetAccountName());
}

// OFFSET: 0x00481880/0x1400A04F0
void SetVisitableURL(int32_t index, const char* url) {
    SStrCopy(s_visitableURLs[index], url, sizeof(s_visitableURLs[index]));
}

// OFFSET: 0x008C1490/0x1406309C0
int32_t GetPortalLocales(int32_t* locales, int32_t* count) {
    *count = 0;
    const char* portal = CVar::Lookup("portal")->m_stringValue.m_str;

    if (!portal) {
        portal = " ";
    }

    switch (*portal) {
        case 'C':
        case 'c':
            locales[0] = 6;
            locales[1] = 8;
            *count = 2;
            return 2;

        case 'E':
        case 'e':
            locales[0] = 0;
            locales[1] = 1;
            locales[2] = 3;
            locales[3] = 4;
            locales[4] = 11;
            locales[5] = 14;
            *count = 6;
            return 6;

        case 'K':
        case 'k':
            locales[0] = 5;
            *count = 1;
            return 1;

        case 'T':
        case 't':
            locales[0] = 7;
            locales[1] = 9;
            *count = 2;
            return 2;

        case 'U':
        case 'u':
            locales[0] = 2;
            locales[1] = 10;
            locales[2] = 13;
            *count = 3;
            return 3;

        default:
            locales[0] = 0;
            locales[1] = 1;
            locales[2] = 2;
            locales[3] = 3;
            locales[4] = 4;
            locales[5] = 5;
            locales[6] = 6;
            locales[7] = 7;
            locales[8] = 8;
            locales[9] = 9;
            locales[10] = 10;
            locales[11] = 11;
            locales[12] = 13;
            locales[13] = 14;
            *count = 14;
            return 14;
    }
}
