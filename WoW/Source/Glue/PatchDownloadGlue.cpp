#include "PatchDownloadGlue.h"
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FileDownload.h"
#include "Engine/Source/Event/FrameTime.h"

// STATIC: 0x00BD1460
static const uint8_t s_patchExponent[4] = {
    0x01, 0x00, 0x01, 0x00,
};

// STATIC: 0x00D07D08
static uint8_t s_patchModulus[256] = {
    0x4F, 0x6F, 0xD4, 0x65, 0x56, 0x85, 0x88, 0xD9, 0xAA, 0xE6, 0xF7, 0x05, 0x72, 0xBB, 0x2A, 0x20,
    0xE0, 0x24, 0x38, 0x2B, 0x77, 0xB3, 0x3D, 0xC2, 0xD3, 0x2D, 0x52, 0x5C, 0xB7, 0xA3, 0x0F, 0xE6,
    0x0F, 0x80, 0x93, 0x8D, 0x5C, 0xC4, 0x81, 0x5E, 0x9D, 0x58, 0x8E, 0xCA, 0xA6, 0x6F, 0x27, 0x36,
    0x52, 0x58, 0x0D, 0xCE, 0x65, 0xE2, 0xB5, 0x7A, 0xD2, 0x39, 0x74, 0x8A, 0xAC, 0xAA, 0xC1, 0x0E,
    0xE5, 0x67, 0x2B, 0x79, 0xC1, 0xFE, 0x08, 0x96, 0x54, 0x18, 0x2A, 0x34, 0x31, 0xA4, 0x06, 0x20,
    0x4B, 0x12, 0xD2, 0x47, 0x63, 0x53, 0x36, 0x38, 0x19, 0x88, 0x8E, 0x50, 0x2F, 0x2F, 0xD1, 0x68,
    0xC8, 0x5C, 0x9D, 0x3B, 0xDE, 0xFE, 0x56, 0x21, 0xD1, 0x59, 0x4D, 0x1C, 0x32, 0x1B, 0x70, 0x4B,
    0x40, 0xE2, 0x5E, 0x0C, 0x0E, 0x4B, 0x84, 0xD5, 0xD7, 0x52, 0x00, 0x50, 0x65, 0x81, 0xCE, 0x17,
    0x53, 0x25, 0x49, 0x9E, 0x40, 0xE9, 0x04, 0x94, 0x2C, 0xA7, 0x93, 0x5C, 0x5D, 0x54, 0xC8, 0xF9,
    0x6E, 0xD0, 0x69, 0xFC, 0xE5, 0x8F, 0xC0, 0xB6, 0x76, 0xF2, 0xEB, 0x34, 0x67, 0x9F, 0xF8, 0xD0,
    0x34, 0xB9, 0xEA, 0x9E, 0x80, 0xD0, 0x29, 0x8D, 0xB4, 0xB2, 0xEB, 0x3E, 0xF4, 0x4F, 0xD0, 0x84,
    0x43, 0x15, 0xF4, 0x19, 0xE3, 0xC6, 0x52, 0xA2, 0x18, 0x40, 0x08, 0x07, 0x21, 0xF4, 0x20, 0xEC,
    0xE6, 0x5A, 0x9C, 0x2B, 0x08, 0xF6, 0x17, 0x97, 0x72, 0xA6, 0xD7, 0xAB, 0x64, 0x0B, 0x04, 0x95,
    0x93, 0x0D, 0xCC, 0x8F, 0x02, 0x30, 0x91, 0xA2, 0xEE, 0x30, 0xE6, 0x5F, 0x2F, 0xD4, 0x13, 0xB5,
    0x5D, 0xCA, 0x5D, 0xCF, 0x7D, 0xC3, 0x6F, 0x6B, 0x9D, 0xBC, 0x6D, 0x86, 0x4C, 0x91, 0xC5, 0x51,
    0xE8, 0xFD, 0xA6, 0x33, 0x10, 0xD1, 0x12, 0x8F, 0x05, 0x80, 0xDA, 0x42, 0x57, 0x0C, 0xEB, 0xC0,
};

// STATIC: 0x00EBB938
float CGlueMgr::m_patchDownloadProgress;

// STATIC: 0x00EBB93C
uint32_t CGlueMgr::m_patchLastUpdate;

// STATIC: 0x00EBB940
uint32_t CGlueMgr::m_patchStartTime;

// STATIC: 0x00EBC47C
FileDownload* CGlueMgr::m_patchDownload;

// STATIC: 0x00EBC480
bool CGlueMgr::m_deleteLocalPatch;

// OFFSET: 0x00810680/0x140527C70
bool CGlueMgr::PatchDownloadExtractCommand(SArchive* archive, const char* name) {
    void* data;
    uint32_t size;

    if (!SFileLoadFileEx(archive, name, &data, &size, 0, 0, NULL) || !data) {
        return false;
    }

    HOSFILE file = OsCreateFile(name, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        SFileUnloadFile(data);
        return false;
    }

    uint32_t written;
    OsWriteFile(file, data, size, &written);
    SFileUnloadFile(data);
    OsCloseFile(file);

    return true;
}

// OFFSET: 0x00810710/0x140527D20
bool CGlueMgr::PatchDownloadDeleteCommand(const char* path) {
    if (OsFileExists(path)) {
        if (SStrCmpI(path, "wow-patch.mpq", STORM_MAX_STR)) {
            OsDeleteFile(path);
            return true;
        }

        CGlueMgr::m_deleteLocalPatch = true;
        return true;
    }

    if (OsDirectoryExists(path)) {
        OsRemoveDirectory(path);
    }

    return true;
}

// OFFSET: 0x00810780/0x140527D90
bool CGlueMgr::PatchDownloadExecutePrepatch(SArchive* archive) {
    bool result = true;
    void* data = NULL;
    uint32_t size;

    SFileLoadFileEx(archive, "prepatch.lst", &data, &size, 1, 0, NULL);

    if (!data) {
        return false;
    }

    const char* cursor = static_cast<const char*>(data);

    while (*cursor && result) {
        char line[260];
        SStrTokenize(&cursor, line, sizeof(line), "\r\n", NULL);

        if (!SStrCmp(line, "extract ", SStrLen("extract "))) {
            result = CGlueMgr::PatchDownloadExtractCommand(archive, &line[SStrLen("extract ")]);
        } else if (!SStrCmp(line, "execute ", SStrLen("execute "))) {
            result = CGlueMgr::PatchDownloadExecuteCommand(&line[SStrLen("execute ")], "");
        } else if (!SStrCmp(line, "delete ", SStrLen("delete "))) {
            result = CGlueMgr::PatchDownloadDeleteCommand(&line[SStrLen("delete ")]);
        }

        if (!cursor) {
            break;
        }
    }

    SFileUnloadFile(data);
    return result;
}

// PROVEN: ab06e423 IDENTICAL/NONE
// OFFSET: 0x00810940/0x140527FA0
void CGlueMgr::PatchDownloadFeed(const uint8_t* data, uint32_t size) {
    if (CGlueMgr::m_patchDownload) {
        CGlueMgr::m_patchDownload->Proceed(data, size);
    }
}

// OFFSET: 0x00810960/0x140527FD0
size_t CGlueMgr::GetPatchFolder(char* buffer, uint32_t size) {
    return SStrCopy(buffer, OsFileGetDownloadFolder(), size);
}

// OFFSET: 0x008109A0/0x140528040
void CGlueMgr::PatchDownloadStart() {
    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
    CGlueMgr::SetIdleState(IDLE_PATCH_DOWNLOAD);
    CGlueMgr::SetScreen(GLUE_SCREEN_PATCHDOWNLOAD);
    CGlueMgr::m_patchStartTime = FrameTime::GetCurTimeMs();
    CGlueMgr::m_patchLastUpdate = FrameTime::GetCurTimeMs();

    if (CGlueMgr::m_patchDownload) {
        delete CGlueMgr::m_patchDownload;
    }

    if (ClientServices::LoginConnection()->GetLoginServerType() == 1) {
        CGlueMgr::m_patchDownload = new (__FILE__, __LINE__) BattlenetFileDownload;
    } else {
        CGlueMgr::m_patchDownload = new (__FILE__, __LINE__) GruntFileDownload;
    }

    Login* login = ClientServices::LoginConnection();
    uint64_t size = login->m_fileSize;
    const char* name = ClientServices::LoginConnection()->m_fileName;

    if (!SStrCmpI(name, "Patch", STORM_MAX_STR)) {
        name = "wow-patch.mpq";
    }

    char path[260];
    SStrCopy(path, OsFileGetDownloadFolder(), sizeof(path));
    OsCreateDirectory(path, 1);
    SStrPack(path, "/", sizeof(path));
    SStrPack(path, name, sizeof(path));

    ClientServices::StartDownload();

    uint64_t offset;
    bool accept = CGlueMgr::m_patchDownload->Initialize(path, ClientServices::LoginConnection()->m_fileMD5, size, offset);

    if (!ClientServices::LoginConnection()->GetLoginServerType()) {
        ClientServices::FileAccept(accept, offset);
    }
}

// OFFSET: 0x00810600/0x140527BE0
void CGlueMgr::PatchDownloadCancel() {
    CGlueMgr::m_patchDownload->StopDownload();

    if (CGlueMgr::m_patchDownload) {
        delete CGlueMgr::m_patchDownload;
    }

    CGlueMgr::m_patchDownload = NULL;

    if (!ClientServices::LoginConnection()->GetLoginServerType()) {
        ClientServices::LoginConnection()->FileComplete(false);
    }

    CGlueMgr::SetIdleState(IDLE_NONE);
    ClientServices::LoginConnection()->Logoff();
    CGlueMgr::SetScreen(GLUE_SCREEN_LOGIN);
}

// OFFSET: 0x008108A0/0x140527EE0
void CGlueMgr::PatchFailed(int32_t reason, uint32_t data) {
    ClientServices::LoginConnection()->Logoff();
    CGlueMgr::SetIdleState(IDLE_NONE);
    CGlueMgr::SetScreen(GLUE_SCREEN_LOGIN);

    char buffer[1024];
    const char* text;

    if (reason == 2) {
        uint32_t megabytes = data;

        if (megabytes < 2) {
            megabytes = 2;
        }

        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("PATCH_FAILED_DISK_FULL", -1, GENDER_NOT_APPLICABLE), megabytes);
        text = buffer;
    } else {
        text = FrameScript_GetText("PATCH_FAILED_MESSAGE", -1, GENDER_NOT_APPLICABLE);
    }

    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
}

// OFFSET: 0x00810D10/0x140528460
void CGlueMgr::PatchDownloadIdle() {
    switch (CGlueMgr::m_patchDownload->GetStatus()) {
        case 2: {
            CGlueMgr::m_patchDownloadProgress = 1.0f;
            FrameScript_SignalEvent(GLUE_EVENT_PATCH_DOWNLOADED, NULL);

            if (CGlueMgr::m_patchDownload->Accepted()) {
                ClientServices::LoginConnection()->FileComplete(true);
            }

            CGlueMgr::m_patchDownload->CompleteDownload();

            if (CGlueMgr::m_patchDownload->GetStatus() == 3) {
                uint32_t data;
                int32_t reason = CGlueMgr::m_patchDownload->GetReason(&data);
                CGlueMgr::PatchFailed(reason, data);
            } else {
                CGlueMgr::SetIdleState(IDLE_NONE);
            }

            if (CGlueMgr::m_patchDownload) {
                delete CGlueMgr::m_patchDownload;
            }

            CGlueMgr::m_patchDownload = NULL;
            break;
        }

        case 3: {
            if (CGlueMgr::m_patchDownload->Accepted()) {
                ClientServices::LoginConnection()->FileComplete(false);
            }

            CGlueMgr::m_patchDownload->Destroy(true);

            uint32_t data;
            int32_t reason = CGlueMgr::m_patchDownload->GetReason(&data);
            CGlueMgr::PatchFailed(reason, data);

            if (CGlueMgr::m_patchDownload) {
                delete CGlueMgr::m_patchDownload;
            }

            CGlueMgr::m_patchDownload = NULL;
            break;
        }

        case 4: {
            if (CGlueMgr::m_patchDownload) {
                delete CGlueMgr::m_patchDownload;
            }

            CGlueMgr::m_patchDownload = NULL;
            CGlueMgr::SetIdleState(IDLE_LOGIN_SERVER_LOGIN);

            Login* login = ClientServices::LoginConnection();
            const char* password = ClientServices::LoginConnection()->m_password;

            char username[1280];
            char passwordCopy[1280];
            SStrCopy(username, login->m_rawUsername, sizeof(username));
            SStrCopy(passwordCopy, password, sizeof(passwordCopy));

            BattlenetLogin::s_monolithicComponent = true;
            ClientServices::Logon(username, passwordCopy);
            break;
        }

        default: {
            uint32_t now = FrameTime::GetCurTimeMs();

            if (now - CGlueMgr::m_patchLastUpdate > 250) {
                CGlueMgr::m_patchLastUpdate = now;
                CGlueMgr::m_patchDownloadProgress = CGlueMgr::m_patchDownload->GetProgress();
                FrameScript_SignalEvent(GLUE_EVENT_PATCH_UPDATE_PROGRESS, NULL);

                if (!ClientServices::LoginConnection()->GetLoginServerType() && !ClientServices::LoginConnection()->IsLoggedOn()) {
                    CGlueMgr::PatchDownloadCancel();
                    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("DISCONNECTED", -1, GENDER_NOT_APPLICABLE));
                }
            }

            break;
        }
    }
}

// OFFSET: 0x00810BC0/0x1405282C0
void CGlueMgr::PatchDownloadApply() {
    char currentDirectory[260];
    OsGetCurrentDirectory(sizeof(currentDirectory), currentDirectory);

    char folder[260];
    SStrCopy(folder, OsFileGetDownloadFolder(), sizeof(folder));
    OsSetCurrentDirectory(folder);

    bool success = false;
    SArchive* archive = NULL;
    int32_t opened = SFileOpenArchive("wow-patch.mpq", 100, 0, &archive);
    int32_t reason = 0;
    CGlueMgr::m_deleteLocalPatch = false;

    if (opened) {
        uint32_t authResult;
        SFileAuthenticateArchiveEx(archive, &authResult, s_patchModulus, sizeof(s_patchModulus), s_patchExponent, sizeof(s_patchExponent));

        if (authResult >= 5) {
            success = CGlueMgr::PatchDownloadExecutePrepatch(archive);

            if (!success) {
                reason = 6;
            }
        } else {
            reason = 5;
        }

        SFileCloseArchive(archive);
        archive = NULL;

        if (success) {
            if (CGlueMgr::m_deleteLocalPatch) {
                OsDeleteFile("wow-patch.mpq");
            }

            OsSetCurrentDirectory(currentDirectory);
            CGlueMgr::QuitGame();
            return;
        }
    } else {
        reason = SErrGetLastError() == 2 ? 4 : 5;
    }

    CGlueMgr::PatchFailed(reason, 0);
    OsDeleteFile("wow-patch.mpq");
    OsSetCurrentDirectory(currentDirectory);
}
