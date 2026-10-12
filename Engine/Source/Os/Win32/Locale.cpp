#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <cstring>
#include <cwchar>
#include <windows.h>
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Os/Error.h"
#include "Engine/Source/Os/Locale.h"
#include "Engine/Source/Os/System.h"

typedef int32_t (WINAPI* GETUSERDEFAULTLOCALENAME)(LPWSTR localeName, int32_t size);

// STATIC: 0x00EC6DBC
static GETUSERDEFAULTLOCALENAME s_getUserDefaultLocaleName;

// OFFSET: 0x00876AD0/0x1405ADCF0
static int32_t WINAPI DownlevelGetUserDefaultLocaleName(LPWSTR localeName, int32_t size) {
    uint32_t lang = GetUserDefaultLangID();

    switch (lang & 0x3FF) {
        case 0x01:
            wcscpy(localeName, L"ar-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"SA");
                    return size;
                case 2:
                    wcscat(localeName, L"IQ");
                    return size;
                case 3:
                    wcscat(localeName, L"EG");
                    return size;
                case 4:
                    wcscat(localeName, L"LY");
                    return size;
                case 5:
                    wcscat(localeName, L"DZ");
                    return size;
                case 6:
                    wcscat(localeName, L"MA");
                    return size;
                case 7:
                    wcscat(localeName, L"TN");
                    return size;
                case 8:
                    wcscat(localeName, L"OM");
                    return size;
                case 9:
                    wcscat(localeName, L"YE");
                    return size;
                case 10:
                    wcscat(localeName, L"SY");
                    return size;
                case 11:
                    wcscat(localeName, L"JO");
                    return size;
                case 12:
                    wcscat(localeName, L"LB");
                    return size;
                case 13:
                    wcscat(localeName, L"KW");
                    return size;
                case 14:
                    wcscat(localeName, L"AE");
                    return size;
                case 15:
                    wcscat(localeName, L"BH");
                    return size;
                case 16:
                    wcscat(localeName, L"QA");
                    return size;
            }

            return size;
        case 0x02:
            wcscpy(localeName, L"bg-BG");
            return size;
        case 0x03:
            wcscpy(localeName, L"ca-ES");
            return size;
        case 0x04:
            wcscpy(localeName, L"zh-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"TW");
                    return size;
                case 2:
                    wcscat(localeName, L"CN");
                    return size;
                case 3:
                    wcscat(localeName, L"HK");
                    return size;
                case 4:
                    wcscat(localeName, L"SG");
                    return size;
                case 5:
                    wcscat(localeName, L"MO");
                    return size;
            }

            return size;
        case 0x05:
            wcscpy(localeName, L"cs-CZ");
            return size;
        case 0x06:
            wcscpy(localeName, L"da-DK");
            return size;
        case 0x07:
            wcscpy(localeName, L"de-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"DE");
                    return size;
                case 2:
                    wcscat(localeName, L"CH");
                    return size;
                case 3:
                    wcscat(localeName, L"AT");
                    return size;
                case 4:
                    wcscat(localeName, L"LU");
                    return size;
                case 5:
                    wcscat(localeName, L"LI");
                    return size;
            }

            return size;
        case 0x08:
            wcscpy(localeName, L"el-GR");
            return size;
        case 0x09:
            wcscpy(localeName, L"en-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"US");
                    return size;
                case 2:
                    wcscat(localeName, L"GB");
                    return size;
                case 3:
                    wcscat(localeName, L"AU");
                    return size;
                case 4:
                    wcscat(localeName, L"CA");
                    return size;
                case 5:
                    wcscat(localeName, L"NZ");
                    return size;
                case 6:
                    wcscat(localeName, L"IE");
                    return size;
                case 7:
                    wcscat(localeName, L"ZA");
                    return size;
                case 8:
                    wcscat(localeName, L"JM");
                    return size;
                case 9:
                    wcscat(localeName, L"029");
                    return size;
                case 10:
                    wcscat(localeName, L"BZ");
                    return size;
                case 11:
                    wcscat(localeName, L"TT");
                    return size;
                case 12:
                    wcscat(localeName, L"ZW");
                    return size;
                case 13:
                    wcscat(localeName, L"PH");
                    return size;
                case 16:
                    wcscat(localeName, L"IN");
                    return size;
                case 17:
                    wcscat(localeName, L"MY");
                    return size;
                case 18:
                    wcscat(localeName, L"SG");
                    return size;
            }

            return size;
        case 0x0A:
            wcscpy(localeName, L"es-");

            switch (lang >> 10) {
                case 1:
                case 3:
                    wcscat(localeName, L"ES");
                    return size;
                case 2:
                    wcscat(localeName, L"MX");
                    return size;
                case 4:
                    wcscat(localeName, L"GT");
                    return size;
                case 5:
                    wcscat(localeName, L"CR");
                    return size;
                case 6:
                    wcscat(localeName, L"PA");
                    return size;
                case 7:
                    wcscat(localeName, L"DO");
                    return size;
                case 8:
                    wcscat(localeName, L"VE");
                    return size;
                case 9:
                    wcscat(localeName, L"CO");
                    return size;
                case 10:
                    wcscat(localeName, L"PE");
                    return size;
                case 11:
                    wcscat(localeName, L"AR");
                    return size;
                case 12:
                    wcscat(localeName, L"EC");
                    return size;
                case 13:
                    wcscat(localeName, L"CL");
                    return size;
                case 14:
                    wcscat(localeName, L"UY");
                    return size;
                case 15:
                    wcscat(localeName, L"PY");
                    return size;
                case 16:
                    wcscat(localeName, L"BO");
                    return size;
                case 17:
                    wcscat(localeName, L"SV");
                    return size;
                case 18:
                    wcscat(localeName, L"HN");
                    return size;
                case 19:
                    wcscat(localeName, L"NI");
                    return size;
                case 20:
                    wcscat(localeName, L"PR");
                    return size;
                case 21:
                    wcscat(localeName, L"US");
                    return size;
            }

            return size;
        case 0x0B:
            wcscpy(localeName, L"fi-FI");
            return size;
        case 0x0C:
            wcscpy(localeName, L"fr-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"FR");
                    return size;
                case 2:
                    wcscat(localeName, L"BE");
                    return size;
                case 3:
                    wcscat(localeName, L"CA");
                    return size;
                case 4:
                    wcscat(localeName, L"CH");
                    return size;
                case 5:
                    wcscat(localeName, L"LU");
                    return size;
                case 6:
                    wcscat(localeName, L"MC");
                    return size;
            }

            return size;
        case 0x0D:
            wcscpy(localeName, L"he-IL");
            return size;
        case 0x0E:
            wcscpy(localeName, L"hu-HU");
            return size;
        case 0x0F:
            wcscpy(localeName, L"is-IS");
            return size;
        case 0x10:
            wcscpy(localeName, L"it-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"IT");
                    return size;
                case 2:
                    wcscat(localeName, L"CH");
                    return size;
            }

            return size;
        case 0x11:
            wcscpy(localeName, L"ja-JP");
            return size;
        case 0x12:
            wcscpy(localeName, L"ko-KR");
            return size;
        case 0x13:
            wcscpy(localeName, L"nl-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"NL");
                    return size;
                case 2:
                    wcscat(localeName, L"BE");
                    return size;
            }

            return size;
        case 0x14:
            wcscpy(localeName, L"no-NO");
            return size;
        case 0x15:
            wcscpy(localeName, L"pl-PL");
            return size;
        case 0x16:
            wcscpy(localeName, L"pt-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"BR");
                    return size;
                case 2:
                    wcscat(localeName, L"PT");
                    return size;
            }

            return size;
        case 0x17:
            wcscpy(localeName, L"rm-CH");
            return size;
        case 0x18:
            wcscpy(localeName, L"ro-RO");
            return size;
        case 0x19:
            wcscpy(localeName, L"ru-RU");
            return size;
        case 0x1A:
            switch (lang >> 10) {
                case 1:
                    wcscpy(localeName, L"hr-HR");
                    return size;
                case 2:
                case 3:
                    wcscpy(localeName, L"sr-CS");
                    return size;
                case 4:
                    wcscpy(localeName, L"hr-BA");
                    return size;
                case 5:
                case 8:
                    wcscpy(localeName, L"bs-BA");
                    return size;
                case 6:
                case 7:
                    wcscpy(localeName, L"sr-BA");
                    return size;
            }

            return size;
        case 0x1B:
            wcscpy(localeName, L"sk-SK");
            return size;
        case 0x1C:
            wcscpy(localeName, L"sq-AL");
            return size;
        case 0x1D:
            wcscpy(localeName, L"sv-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"SE");
                    return size;
                case 2:
                    wcscat(localeName, L"FI");
                    return size;
            }

            return size;
        case 0x1E:
            wcscpy(localeName, L"th-TH");
            return size;
        case 0x1F:
            wcscpy(localeName, L"tr-TR");
            return size;
        case 0x20:
            wcscpy(localeName, L"ur-PK");
            return size;
        case 0x21:
            wcscpy(localeName, L"id-ID");
            return size;
        case 0x22:
            wcscpy(localeName, L"uk-UA");
            return size;
        case 0x23:
            wcscpy(localeName, L"be-BY");
            return size;
        case 0x24:
            wcscpy(localeName, L"sl-SI");
            return size;
        case 0x25:
            wcscpy(localeName, L"et-EE");
            return size;
        case 0x26:
            wcscpy(localeName, L"lv-LV");
            return size;
        case 0x27:
            wcscpy(localeName, L"lt-LT");
            return size;
        case 0x28:
            wcscpy(localeName, L"tg-TJ");
            return size;
        case 0x29:
            wcscpy(localeName, L"fa-IR");
            return size;
        case 0x2A:
            wcscpy(localeName, L"vi-VN");
            return size;
        case 0x2B:
            wcscpy(localeName, L"hy-AM");
            return size;
        case 0x2C:
            wcscpy(localeName, L"az-AZ");
            return size;
        case 0x2D:
            wcscpy(localeName, L"eu-ES");
            return size;
        case 0x2E:
            switch (lang >> 10) {
                case 1:
                    wcscpy(localeName, L"hsb-DE");
                    return size;
                case 2:
                    wcscpy(localeName, L"dsb-DE");
                    return size;
            }

            return size;
        case 0x2F:
            wcscpy(localeName, L"mk-MK");
            return size;
        case 0x32:
            wcscpy(localeName, L"tn-ZA");
            return size;
        case 0x34:
            wcscpy(localeName, L"xh-ZA");
            return size;
        case 0x35:
            wcscpy(localeName, L"zu-ZA");
            return size;
        case 0x36:
            wcscpy(localeName, L"af-ZA");
            return size;
        case 0x37:
            wcscpy(localeName, L"ka-GE");
            return size;
        case 0x38:
            wcscpy(localeName, L"fo-FO");
            return size;
        case 0x39:
            wcscpy(localeName, L"hi-IN");
            return size;
        case 0x3A:
            wcscpy(localeName, L"mt-MT");
            return size;
        case 0x3B:
            switch (lang >> 10) {
                case 1:
                    wcscpy(localeName, L"se-NO");
                    return size;
                case 2:
                    wcscpy(localeName, L"se-SE");
                    return size;
                case 3:
                    wcscpy(localeName, L"se-FI");
                    return size;
                case 4:
                    wcscpy(localeName, L"smj-NO");
                    return size;
                case 5:
                    wcscpy(localeName, L"smj-SE");
                    return size;
                case 6:
                    wcscpy(localeName, L"sma-NO");
                    return size;
                case 7:
                    wcscpy(localeName, L"sma-SE");
                    return size;
                case 8:
                    wcscpy(localeName, L"sms-FI");
                    return size;
                case 9:
                    wcscpy(localeName, L"smn-FI");
                    return size;
            }

            return size;
        case 0x3C:
            wcscpy(localeName, L"ga-IE");
            return size;
        case 0x3E:
            wcscpy(localeName, L"ms-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"MY");
                    return size;
                case 2:
                    wcscat(localeName, L"BN");
                    return size;
            }

            return size;
        case 0x3F:
            wcscpy(localeName, L"kk-KZ");
            return size;
        case 0x40:
            wcscpy(localeName, L"ky-KG");
            return size;
        case 0x41:
            wcscpy(localeName, L"sw-KE");
            return size;
        case 0x42:
            wcscpy(localeName, L"tk-TM");
            return size;
        case 0x43:
            wcscpy(localeName, L"uz-UZ");
            return size;
        case 0x44:
            wcscpy(localeName, L"tt-RU");
            return size;
        case 0x45:
            wcscpy(localeName, L"bn-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"IN");
                    return size;
                case 2:
                    wcscat(localeName, L"BD");
                    return size;
            }

            return size;
        case 0x46:
            wcscpy(localeName, L"pa-IN");
            return size;
        case 0x47:
            wcscpy(localeName, L"gu-IN");
            return size;
        case 0x48:
            wcscpy(localeName, L"or-IN");
            return size;
        case 0x49:
            wcscpy(localeName, L"ta-IN");
            return size;
        case 0x4A:
            wcscpy(localeName, L"te-IN");
            return size;
        case 0x4B:
            wcscpy(localeName, L"kn-IN");
            return size;
        case 0x4C:
            wcscpy(localeName, L"ml-IN");
            return size;
        case 0x4D:
            wcscpy(localeName, L"as-IN");
            return size;
        case 0x4E:
            wcscpy(localeName, L"mr-IN");
            return size;
        case 0x4F:
            wcscpy(localeName, L"sa-IN");
            return size;
        case 0x50:
            wcscpy(localeName, L"mn-MN");
            return size;
        case 0x51:
            wcscpy(localeName, L"bo-CN");
            return size;
        case 0x52:
            wcscpy(localeName, L"cy-GB");
            return size;
        case 0x53:
            wcscpy(localeName, L"kh-KH");
            return size;
        case 0x54:
            wcscpy(localeName, L"lo-LA");
            return size;
        case 0x56:
            wcscpy(localeName, L"gl-ES");
            return size;
        case 0x57:
            wcscpy(localeName, L"kok-IN");
            return size;
        case 0x5A:
            wcscpy(localeName, L"syr-SY");
            return size;
        case 0x5B:
            wcscpy(localeName, L"si-LK");
            return size;
        case 0x5D:
            wcscpy(localeName, L"iu-CA");
            return size;
        case 0x5E:
            wcscpy(localeName, L"am-ET");
            return size;
        case 0x5F:
            wcscpy(localeName, L"tzm-DZ");
            return size;
        case 0x61:
            wcscpy(localeName, L"ne-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"NP");
                    return size;
                case 2:
                    wcscat(localeName, L"IN");
                    return size;
            }

            return size;
        case 0x62:
            wcscpy(localeName, L"fy-NL");
            return size;
        case 0x63:
            wcscpy(localeName, L"ps-AF");
            return size;
        case 0x64:
            wcscpy(localeName, L"fil-PH");
            return size;
        case 0x65:
            wcscpy(localeName, L"dv-MV");
            return size;
        case 0x68:
            wcscpy(localeName, L"ha-NG");
            return size;
        case 0x6A:
            wcscpy(localeName, L"yo-NG");
            return size;
        case 0x6B:
            wcscpy(localeName, L"quz-");

            switch (lang >> 10) {
                case 1:
                    wcscat(localeName, L"BO");
                    return size;
                case 2:
                    wcscat(localeName, L"EC");
                    return size;
                case 3:
                    wcscat(localeName, L"PE");
                    return size;
            }

            return size;
        case 0x6C:
            wcscpy(localeName, L"nso-ZA");
            return size;
        case 0x6D:
            wcscpy(localeName, L"ba-RU");
            return size;
        case 0x6E:
            wcscpy(localeName, L"lb-LU");
            return size;
        case 0x6F:
            wcscpy(localeName, L"kl-GL");
            return size;
        case 0x70:
            wcscpy(localeName, L"ig-NG");
            return size;
        case 0x78:
            wcscpy(localeName, L"ii-CN");
            return size;
        case 0x7A:
            wcscpy(localeName, L"arn-CL");
            return size;
        case 0x7C:
            wcscpy(localeName, L"moh-CA");
            return size;
        case 0x7E:
            wcscpy(localeName, L"br-FR");
            return size;
        case 0x80:
            wcscpy(localeName, L"ug-CN");
            return size;
        case 0x81:
            wcscpy(localeName, L"mi-NZ");
            return size;
        case 0x82:
            wcscpy(localeName, L"oc-FR");
            return size;
        case 0x83:
            wcscpy(localeName, L"co-FR");
            return size;
        case 0x84:
            wcscpy(localeName, L"gsw-FR");
            return size;
        case 0x85:
            wcscpy(localeName, L"sah-RU");
            return size;
        case 0x86:
            wcscpy(localeName, L"qut-GT");
            return size;
        case 0x87:
            wcscpy(localeName, L"rw-RW");
            return size;
        case 0x88:
            wcscpy(localeName, L"wo-SN");
            return size;
        case 0x8C:
            wcscpy(localeName, L"prs-AF");
            return size;
    }

    return size;
}

// OFFSET: 0x008790C0/0x1405B0570
uint32_t OsGetLangIdFromLocale(uint32_t locale) {
    switch (locale) {
        case 'enGB':
            return 0x809;
        case 'deDE':
            return 0x407;
        case 'esES':
            return 0x40A;
        case 'esMX':
            return 0x80A;
        case 'frFR':
            return 0x40C;
        case 'koKR':
            return 0x412;
        case 'ptPT':
            return 0x816;
        case 'ptBR':
            return 0x416;
        case 'ruRU':
            return 0x419;
        case 'zhTW':
            return 0x404;
        case 'zhCN':
            return 0x804;
        default:
            return 0x409;
    }
}

// OFFSET: 0x00879170/0x1405B0620
uint32_t OsGetUserDefaultLangID() {
    return GetUserDefaultLangID();
}

// OFFSET: 0x00879180/0x1405B0640
uint32_t OsGetCurrentLocale() {
    GETUSERDEFAULTLOCALENAME getLocaleName = s_getUserDefaultLocaleName;

    if (!getLocaleName) {
        HMODULE kernel = GetModuleHandleA("Kernel32.dll");

        if (!kernel) {
            return 0x53556E65;
        }

        getLocaleName = reinterpret_cast<GETUSERDEFAULTLOCALENAME>(GetProcAddress(kernel, "GetUserDefaultLocaleName"));
        s_getUserDefaultLocaleName = getLocaleName;

        if (!getLocaleName) {
            getLocaleName = DownlevelGetUserDefaultLocaleName;
            s_getUserDefaultLocaleName = DownlevelGetUserDefaultLocaleName;
        }
    }

    WCHAR wideName[85];

    if (!getLocaleName(wideName, 85)) {
        char* error = OsGetLastErrorStr();
        OsOutputDebugString("Failed to get locale name: %s", error);
        OsFreeLastErrorStr(error);
        return 0x53556E65;
    }

    char name[85];
    ConvertUTF16toUTF8(name, 85, reinterpret_cast<const uint16_t*>(wideName), 85, NULL, NULL);

    if (strlen(name) == 5) {
        memmove(&name[2], &name[3], 3);
    } else {
        char* dash = strchr(name, '-');

        if (!dash || dash - name < 2) {
            return 0x53556E65;
        }

        memmove(&name[2], dash + 1, 2);
    }

    uint16_t high = static_cast<uint16_t>(static_cast<uint8_t>(name[3]) << 8) | static_cast<uint8_t>(name[2]);
    uint16_t low = static_cast<uint16_t>(static_cast<uint8_t>(name[1]) << 8);
    return (static_cast<uint32_t>(high) << 16) | low | static_cast<uint8_t>(name[0]);
}

#endif
