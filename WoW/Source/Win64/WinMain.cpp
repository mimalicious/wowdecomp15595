#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include <commctrl.h>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Locale.h"
#include "Engine/Source/Os/OsVersionHash.h"
#include "Engine/Source/Os/Path.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00000000/0x140B33908
static const uint32_t s_db2LocaleIds[11] = { 'enUS', 'koKR', 'frFR', 'deDE', 'zhCN', 'zhTW', 'esES', 'esMX', 'ruRU', 'enUS', 'ptBR' };

// STATIC: 0x00D3651C
static float s_archiveProgressStages[3][2];

// STATIC: 0x00D36534
static uint32_t s_archiveProgressStage;

// STATIC: 0x00D36538
static HWND s_archiveProgressDialog;

// STATIC: 0x00D3653C
static HWND s_archiveProgressBar;

// STATIC: 0x00D36540
static HICON s_archiveProgressIcon;

static void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t reserved) {
}

// PROVEN: 5f3f6529 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
static bool ReturnFalse() {
    return false;
}

// OFFSET: 0x00000000/0x140008870
static bool CheckWowExeVersion() {
    char path[512];
    OsGetExePath(path, sizeof(path));

    if (sizeof(path) - strlen(path) < 8) {
        return true;
    }

    SStrPack(path, "WoW.exe", sizeof(path));
    OsFileVersionInfo info(path);

    if (!info.m_data.m_count || (info.m_fileVersion[0] == 4 && info.m_fileVersion[1] == 3 && info.m_fileVersion[2] == 4 && info.m_fileVersion[3] == 15595)) {
        return true;
    }

    uint32_t userLanguage = OsGetUserDefaultLangID();
    uint32_t localeLanguage = OsGetLangIdFromLocale(s_db2LocaleIds[GetCurrentDB2Locale()]);
    wchar_t message[512];
    wchar_t title[64];

    if (!SErrGetErrorStrW(0x85100096, message, 512, userLanguage) && !SErrGetErrorStrW(0x85100096, message, 512, localeLanguage)) {
        message[0] = 0;
    }

    if (!SErrGetErrorStrW(0x85100097, title, 64, userLanguage) && !SErrGetErrorStrW(0x85100097, title, 64, localeLanguage)) {
        title[0] = 0;
    }

    OsGuiMessageBoxW(NULL, 0x40001, reinterpret_cast<const uint16_t*>(message), reinterpret_cast<const uint16_t*>(title));
    return false;
}

extern "C" int WinMainCRTStartup();

// INLINED
extern "C" int start() {
    StormRtlInitialize();
    return WinMainCRTStartup();
}

// OFFSET: 0x00408430/0x14000A7B0
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    _set_invalid_parameter_handler(InvalidParameterHandler);
    ReturnFalse();

    if (CheckWowExeVersion()) {
        return CommonMain();
    }

    return 1;
}

// OFFSET: 0x00409090
static INT_PTR CALLBACK ArchiveProgressDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_INITDIALOG) {
        int32_t height = GetSystemMetrics(SM_CYSMICON);
        int32_t width = GetSystemMetrics(SM_CXSMICON);
        s_archiveProgressIcon = static_cast<HICON>(LoadImageW(GetModuleHandleW(NULL), L"BLIZZARDICON.ICO", IMAGE_ICON, width, height, 0));

        if (s_archiveProgressIcon) {
            SendMessageW(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(s_archiveProgressIcon));
        }

        return TRUE;
    }

    if (message == WM_COMMAND && LOWORD(wparam) == IDCANCEL) {
        PostQuitMessage(0);
        SetWindowLongW(window, DWLP_MSGRESULT, 0);
        return TRUE;
    }

    return FALSE;
}

// OFFSET: 0x00409120
void ArchiveProgressInitialize(int32_t sparse) {
    float start;
    float end;
    s_archiveProgressStages[0][0] = 0.0f;

    if (sparse) {
        start = 0.0f;
        end = 0.3f;
    } else {
        start = 0.3f;
        end = 0.6f;
    }

    s_archiveProgressStages[1][0] = start;
    s_archiveProgressStages[0][1] = start;
    s_archiveProgressStages[2][0] = end;
    s_archiveProgressStages[1][1] = end - start;
    s_archiveProgressStages[2][1] = 1.0f - end;
    s_archiveProgressStage = sparse != 0;
}

// OFFSET: 0x004091A0
void ArchiveProgressSetStage(uint32_t stage) {
    s_archiveProgressStage = stage;
}

// OFFSET: 0x004091B0/0x14000B870
static int32_t ArchiveProgressCreateDialog(int32_t locale) {
    HMODULE module = GetModuleHandleW(NULL);
    s_archiveProgressDialog = CreateDialogParamW(module, MAKEINTRESOURCEW(121), GetDesktopWindow(), ArchiveProgressDialogProc, 0);

    if (!s_archiveProgressDialog) {
        return 0;
    }

    s_archiveProgressBar = GetDlgItem(s_archiveProgressDialog, 1000);
    HWND text = GetDlgItem(s_archiveProgressDialog, 1001);
    SendMessageW(s_archiveProgressBar, PBM_SETSTEP, 1, 0);

    WCHAR buffer[4096];

    if (!LoadStringW(module, locale + 120, buffer, 4096) && !LoadStringW(module, 120, buffer, 4096)) {
        buffer[0] = 0;
    }

    SetWindowTextW(text, buffer);
    ShowWindow(s_archiveProgressDialog, SW_SHOW);
    return 1;
}

// OFFSET: 0x00409290/0x14000B990
int32_t ArchiveProgressPump() {
    if (!s_archiveProgressDialog && !ArchiveProgressCreateDialog(s_locale)) {
        return 0;
    }

    DWORD end = GetTickCount() + 10;

    if (static_cast<int32_t>(end - GetTickCount()) > 0) {
        MSG msg;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE)) {
            if (!GetMessageW(&msg, NULL, 0, 0)) {
                return 0;
            }

            IsDialogMessageW(s_archiveProgressDialog, &msg);

            if (static_cast<int32_t>(end - GetTickCount()) <= 0) {
                return 1;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00409330/0x14000BA60
void ArchiveProgressDestroy() {
    DestroyWindow(s_archiveProgressDialog);

    if (s_archiveProgressIcon) {
        DestroyIcon(s_archiveProgressIcon);
        s_archiveProgressIcon = NULL;
    }

    s_archiveProgressDialog = NULL;
    s_archiveProgressBar = NULL;
}

// OFFSET: 0x00409370/0x14000BAC0
void ArchiveProgressSetPosition(float position) {
    SendMessageW(s_archiveProgressBar, PBM_SETPOS, static_cast<WPARAM>(static_cast<int64_t>((s_archiveProgressStages[s_archiveProgressStage][1] * position + s_archiveProgressStages[s_archiveProgressStage][0]) * 100.0f)), 0);
}

#endif
