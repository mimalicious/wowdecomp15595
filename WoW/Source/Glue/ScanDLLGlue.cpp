#include "ScanDLLGlue.h"
#include <intrin.h>
#include <new>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/OsURLDownload.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/CheckExecutableSignature.h"

// STATIC: 0x00EBC248
char CGlueMgr::m_scandllHackName[512];

// STATIC: 0x00EBC448
static volatile long s_downloadComplete;

// STATIC: 0x00EBC44C
volatile long CGlueMgr::m_scandllStatus;

// STATIC: 0x00EBC450
const char* CGlueMgr::m_scandllResult;

// STATIC: 0x00EBC454
uint8_t CGlueMgr::m_scandllOkayToLogIn;

// STATIC: 0x00EBC455
uint8_t CGlueMgr::m_scandllShouldBlockPlaying;

// STATIC: 0x00EBC458
static CDataStore* s_versionData;

// STATIC: 0x00EBC45C
static char* s_dllURL;

// STATIC: 0x00EBC460
static char* s_localVersion;

// STATIC: 0x00EBC464
SThread CGlueMgr::m_scandllThread;

// STATIC: 0x00BD1334
static ScanDLLThreadParams s_scanThreadParams = { ".\\Scan.dll", &CGlueMgr::m_scandllStatus, &CGlueMgr::m_scandllResult, CGlueMgr::m_scandllHackName, sizeof(((CGlueMgr*)0)->m_scandllHackName), &CGlueMgr::m_scandllOkayToLogIn, &CGlueMgr::m_scandllShouldBlockPlaying };

// OFFSET: 0x0080F810/0x140526AB0
void CGlueMgr::ScanDLLContinueAnyway() {
    if (CGlueMgr::m_scandllShouldBlockPlaying) {
        return;
    }

    int32_t status = CGlueMgr::m_scandllStatus;

    if (status == 2 || status == 3) {
        CGlueMgr::m_scandllOkayToLogIn = 1;
    }
}

// OFFSET: 0x0080F830/0x140526AE0
bool CGlueMgr::IsScanDLLFinished() {
    return CGlueMgr::m_scandllOkayToLogIn;
}

// PROVEN: 03dda93c IDENTICAL/NONE
// OFFSET: 0x0080F880
static uint32_t STORMAPI ScanThreadProc(void* param) {
    ScanDLLThreadParams* params = static_cast<ScanDLLThreadParams*>(param);

    uint8_t hackFound;
    uint8_t shouldBlockPlaying;
    const char* result;

    if (!ScanFileForHack(params->m_dllName, &hackFound, &shouldBlockPlaying, &result, params->m_hackName, params->m_hackNameSize)) {
        _InterlockedExchange(params->m_status, 2);
        return 0;
    }

    *params->m_result = result;
    *params->m_okayToLogIn = hackFound == 0;
    *params->m_shouldBlockPlaying = shouldBlockPlaying;
    _InterlockedExchange(params->m_status, 3);
    return 0;
}

// OFFSET: 0x0080F8F0/0x140526BD0
void CGlueMgr::ScanDLLFinished() {
    CGlueMgr::SetIdleState(IDLE_NONE);

    if (CGlueMgr::m_scandllStatus == 2) {
        CGlueMgr::SetIdleState(IDLE_NONE);
        _InterlockedExchange(&CGlueMgr::m_scandllStatus, 2);
        FrameScript_SignalEvent(GLUE_EVENT_SCANDLL_ERROR, "%s", FrameScript_GetText("SCANDLL_MESSAGE_ERROR", -1, GENDER_NOT_APPLICABLE));
        return;
    }

    FrameScript_SignalEvent(GLUE_EVENT_SCANDLL_FINISHED, "%s%s%d", CGlueMgr::m_scandllResult, CGlueMgr::m_scandllHackName, CGlueMgr::m_scandllShouldBlockPlaying != 0);
}

// OFFSET: 0x0080F970
static bool DownloadDllCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    HOSFILE file = static_cast<HOSFILE>(param);

    if (data && size > 0) {
        uint32_t written;

        if (!OsWriteFile(file, data, size, &written) || written != size) {
            OsCloseFile(file);
            OsDeleteFile(".\\Scan.dll");
            _InterlockedExchange(&s_downloadComplete, 1);
            return false;
        }
    }

    if (done) {
        OsCloseFile(file);

        if (error) {
            OsDeleteFile(".\\Scan.dll");
        }

        _InterlockedExchange(&s_downloadComplete, 1);
    }

    return true;
}

// OFFSET: 0x0080FA00
static bool VersionDownloadCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    CDataStore* msg = static_cast<CDataStore*>(param);
    bool failed = true;

    if (data && size > 0) {
        msg->PutArray(reinterpret_cast<const uint8_t*>(data), size);
    }

    if (!done) {
        return true;
    }

    if (!error) {
        msg->Finalize();

        uint8_t ch;
        uint32_t lineStart = 0;
        msg->Get(ch);

        if (msg->m_read <= msg->m_size) {
            bool found = false;

            while (true) {
                if (ch == '\n' || ch == '\r') {
                    uint32_t pos = msg->m_read;

                    if (pos - lineStart == 5) {
                        char tag[5];
                        msg->m_read = lineStart;
                        msg->GetArray(reinterpret_cast<uint8_t*>(tag), 4);
                        bool valid = msg->m_read <= msg->m_size;
                        msg->m_read = pos;
                        tag[4] = '\0';

                        if (valid && !SStrCmp(tag, "Scan", STORM_MAX_STR)) {
                            found = true;
                            break;
                        }
                    }

                    lineStart = pos;
                }

                msg->Get(ch);

                if (msg->m_read > msg->m_size) {
                    break;
                }
            }

            if (found) {
                uint32_t versionStart = msg->m_read;
                bool crlf = ch == '\r';
                uint32_t versionEnd = msg->m_read;
                bool first = true;
                msg->Get(ch);

                while (msg->m_read <= msg->m_size) {
                    if (first && crlf && ch == '\n') {
                        versionStart++;
                    } else if (ch == '\n' || ch == '\r') {
                        break;
                    }

                    versionEnd = msg->m_read;
                    first = false;
                    msg->Get(ch);
                }

                uint32_t length = versionEnd - versionStart;
                msg->m_read = versionEnd;

                if (length < 24) {
                    char version[24];
                    msg->m_read = versionStart;
                    msg->GetArray(reinterpret_cast<uint8_t*>(version), length);

                    if (msg->m_read <= msg->m_size) {
                        version[length] = '\0';

                        if (s_localVersion && !SStrCmp(s_localVersion, version, STORM_MAX_STR)) {
                            _InterlockedExchange(&s_downloadComplete, 1);
                            failed = false;
                        } else {
                            HOSFILE file = OsCreateFile(".\\Scan.dll.new", OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

                            if (file != OS_INVALID_FILE) {
                                if (OsURLDownload(s_dllURL, DownloadDllCallback, file)) {
                                    failed = false;
                                } else {
                                    OsCloseFile(file);
                                    OsDeleteFile(".\\Scan.dll");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    delete s_versionData;
    s_versionData = NULL;

    if (s_dllURL) {
        SMemFree(s_dllURL, __FILE__, __LINE__, 0);
    }

    s_dllURL = NULL;

    if (s_localVersion) {
        SMemFree(s_localVersion, "delete[]", -1, 0);
    }

    s_localVersion = NULL;

    if (failed) {
        _InterlockedExchange(&s_downloadComplete, 1);
    }

    return true;
}

// OFFSET: 0x0080FCA0/0x140527070
void CGlueMgr::ScanDLLStart(const char* versionURL, const char* dllURL) {
    if (CGlueMgr::m_scandllStatus || s_downloadComplete) {
        return;
    }

    _InterlockedExchange(&s_downloadComplete, 0);
    s_localVersion = static_cast<char*>(SMemAlloc(24, __FILE__, __LINE__, 0));

    bool updated = false;

    if (OsFileExists(".\\Scan.dll.new")) {
        if (GetDllVersionString(".\\Scan.dll.new", s_localVersion, 24)) {
            OsDeleteFile(".\\Scan.dll");
            updated = OsMoveFile(".\\Scan.dll.new", ".\\Scan.dll") != 0;
        }

        if (!updated) {
            OsDeleteFile(".\\Scan.dll.new");
        }
    }

    if (!updated && !GetDllVersionString(".\\Scan.dll", s_localVersion, 24)) {
        delete[] s_localVersion;
        s_localVersion = NULL;
    }

    s_versionData = new (SMemAlloc(sizeof(CDataStore), __FILE__, __LINE__, 0)) CDataStore();
    s_dllURL = SStrDupA(dllURL, __FILE__, __LINE__);

    if (!OsURLDownload(versionURL, VersionDownloadCallback, s_versionData)) {
        delete s_versionData;
        s_versionData = NULL;

        if (s_dllURL) {
            SMemFree(s_dllURL, __FILE__, __LINE__, 0);
        }

        s_dllURL = NULL;
        delete[] s_localVersion;
        s_localVersion = NULL;
        _InterlockedExchange(&s_downloadComplete, 1);
    }

    _InterlockedExchange(&CGlueMgr::m_scandllStatus, 1);

    if (!SThread::Create(ScanThreadProc, &s_scanThreadParams, CGlueMgr::m_scandllThread, const_cast<char*>("ScanThread"), 0)) {
        CGlueMgr::SetIdleState(IDLE_NONE);
        _InterlockedExchange(&CGlueMgr::m_scandllStatus, 2);
        FrameScript_SignalEvent(GLUE_EVENT_SCANDLL_ERROR, "%s", FrameScript_GetText("SCANDLL_MESSAGE_ERROR", -1, GENDER_NOT_APPLICABLE));
        return;
    }

    FrameScript_SignalEvent(GLUE_EVENT_SCANDLL_DOWNLOADING, "%s", FrameScript_GetText("SCANDLL_MESSAGE_SCANNING", -1, GENDER_NOT_APPLICABLE));
    CGlueMgr::SetIdleState(IDLE_SCANDLL);
}
