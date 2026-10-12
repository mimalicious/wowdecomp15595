#include "AccountData.h"
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "ThirdParty/zlib/zlib.h"
#include "WoW/Source/Client.h"
#include "Engine/Source/Event/Event.h"

// STATIC: 0x00CA0C60
static AccountDataInfo s_accountData[] = {
    { false, true, NULL, "config-cache.wtf", "config-cache.old", "synchronizeConfig" },
    { false, false, NULL, "config-cache.wtf", "config-cache.old", "synchronizeConfig" },
    { false, true, NULL, "bindings-cache.wtf", "bindings-cache.old", "synchronizeBindings" },
    { false, false, NULL, "bindings-cache.wtf", "bindings-cache.old", "synchronizeBindings" },
    { false, true, NULL, "macros-cache.txt", "macros-cache.old", "synchronizeMacros" },
    { false, false, NULL, "macros-cache.txt", "macros-cache.old", "synchronizeMacros" },
    { false, false, "layout-local.txt", NULL, NULL, NULL, true },
    { false, false, NULL, "chat-cache.txt", "chat-cache.old", NULL }
};

// STATIC: 0x00DC0C58
static uint32_t s_accountDataTime;

// STATIC: 0x00DC0C60/0x140CA67B0
static int32_t unk_C7BFF8;

// STATIC: 0x00DC0C64/0x140CA67B4
static int32_t unk_C7BFFC;

// PROVEN: 7e58bad5 IDENTICAL/IDENTICAL
// OFFSET: 0x004D7890/0x140115DE0
static int32_t CopyAccountFile(bool global, const char* source, const char* dest) {
    char sourcePath[260];
    char destPath[260];

    const char* accountName = ClientServices::GetAccountName();

    if (!accountName || !source || !dest) {
        return 0;
    }

    if (global) {
        SStrPrintf(sourcePath, sizeof(sourcePath), "WTF\\Account\\%s\\%s", accountName, source);
        SStrPrintf(destPath, sizeof(destPath), "WTF\\Account\\%s\\%s", accountName, dest);
    } else {
        SStrPrintf(sourcePath, sizeof(sourcePath), "WTF\\Account\\%s\\%s\\%s\\%s", accountName, ClientServices::GetSelectedRealmName(), ClientServices::GetCharacterName(), source);
        SStrPrintf(destPath, sizeof(destPath), "WTF\\Account\\%s\\%s\\%s\\%s", accountName, ClientServices::GetSelectedRealmName(), ClientServices::GetCharacterName(), dest);
    }

    return OsCopyFile(sourcePath, destPath, 0);
}

// OFFSET: 0x004D79C0/0x140115F50
static HOSFILE OpenAccountFile(bool global, const char* fileName, bool write) {
    char path[260];

    const char* accountName = ClientServices::GetAccountName();

    if (!accountName || !fileName) {
        return OS_INVALID_FILE;
    }

    if (global) {
        SStrPrintf(path, sizeof(path), "WTF\\Account\\%s", accountName);
    } else {
        SStrPrintf(path, sizeof(path), "WTF\\Account\\%s\\%s\\%s", accountName, ClientServices::GetSelectedRealmName(), ClientServices::GetCharacterName());
    }

    if (write && !OsDirectoryExists(path)) {
        OsCreateDirectory(path, 1);
    }

    SStrPack(path, "\\", sizeof(path));
    SStrPack(path, fileName, sizeof(path));

    if (write) {
        return OsCreateFile(path, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);
    }

    return OsCreateFile(path, OS_GENERIC_READ, 0, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);
}

// OFFSET: 0x004D7B20/0x140116100
static void MD5AccountFile(const char* fileName, uint8_t* digest, bool global) {
    memset(digest, 0, 16);

    HOSFILE file = OpenAccountFile(global, fileName, false);

    if (file == OS_INVALID_FILE) {
        return;
    }

    if (OsGetFileSize(file)) {
        MD5_CTX context;
        MD5Init(&context);

        uint8_t buffer[1024];
        uint32_t bytesRead;

        do {
            OsReadFile(file, buffer, sizeof(buffer), &bytesRead);
            MD5Update(&context, buffer, bytesRead);
        } while (bytesRead);

        MD5Final(digest, &context);
    }

    OsCloseFile(file);
}

// OFFSET: 0x004D7BC0/0x1401161C0
static void LoadCachedChecksums(bool global) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        if (s_accountData[i].m_global == global) {
            count++;
        }
    }

    HOSFILE file = OpenAccountFile(global, "cache.md5", false);

    if (file != OS_INVALID_FILE) {
        if (OsGetFileSize(file) == (sizeof(s_accountData[0].m_md5) + sizeof(s_accountData[0].m_time)) * count) {
            for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
                AccountDataInfo& info = s_accountData[i];

                if (info.m_global == global) {
                    uint32_t bytesRead;
                    OsReadFile(file, info.m_md5, sizeof(info.m_md5), &bytesRead);
                    OsReadFile(file, &info.m_time, sizeof(info.m_time), &bytesRead);
                }
            }
        }

        OsCloseFile(file);
    }

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        AccountDataInfo& info = s_accountData[i];

        if (info.m_global == global) {
            uint8_t digest[16];
            MD5AccountFile(info.m_cacheFile, digest, info.m_global);
            info.m_changed = memcmp(digest, info.m_md5, sizeof(info.m_md5)) != 0;
        }
    }
}

// OFFSET: 0x004D7CF0/0x1401163B0
static void SaveCachedChecksums(bool global) {
    HOSFILE file = OpenAccountFile(global, "cache.md5", true);

    if (file == OS_INVALID_FILE) {
        return;
    }

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        AccountDataInfo& info = s_accountData[i];

        if (info.m_global == global) {
            uint32_t bytesWritten;
            OsWriteFile(file, info.m_md5, sizeof(info.m_md5), &bytesWritten);
            OsWriteFile(file, &info.m_time, sizeof(info.m_time), &bytesWritten);
        }
    }

    OsCloseFile(file);
}

// OFFSET: 0x004D7D60/0x140116460
static void UpdateCachedChecksum(int32_t type, int32_t time) {
    AccountDataInfo& info = s_accountData[type];

    uint8_t digest[16];
    MD5AccountFile(info.m_cacheFile, digest, info.m_global);
    memcpy(info.m_md5, digest, sizeof(info.m_md5));

    info.m_changed = false;
    info.m_time = time;
    info.m_serverTime = time;

    SaveCachedChecksums(info.m_global);
}

// PROVEN: a74ca588 IDENTICAL/NONE
// OFFSET: [0x00551020]
static int32_t Packet_SMSG_UPDATE_ACCOUNT_DATA_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t type;
    msg->Get(type);

    uint32_t result;
    msg->Get(result);

    return 1;
}

// OFFSET: 0x004D7970/0x140115F10
void AccountDataDestroy(bool global) {
    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        if (s_accountData[i].m_global == global) {
            s_accountData[i].m_active = false;
        }
    }
}

// OFFSET: 0x004D8360/0x140116C00
void LoadAccountData(int32_t type, ACCOUNTDATALOADFUNC func, void* param) {
    AccountDataInfo& info = s_accountData[type];

    if (info.m_downloading) {
        info.m_loadFunc = func;
        info.m_loadParam = param;
        return;
    }

    HOSFILE localFile = OpenAccountFile(info.m_global, info.m_localFile, false);
    HOSFILE cacheFile = OpenAccountFile(info.m_global, info.m_cacheFile, false);

    func(localFile, cacheFile, param);

    if (localFile != OS_INVALID_FILE) {
        OsCloseFile(localFile);
    }

    if (cacheFile != OS_INVALID_FILE) {
        OsCloseFile(cacheFile);
    }
}

// PROVEN: a3281402 IDENTICAL/IDENTICAL
// OFFSET: 0x004D79A0/0x140115F40
void SetClearConfigData(int32_t clear) {
    unk_C7BFF8 = clear;
    unk_C7BFFC = clear;
}

// OFFSET: 0x004D7DE0/0x140116550
void UploadAccountData(int32_t type) {
    HOSFILE file = OpenAccountFile(s_accountData[type].m_global, s_accountData[type].m_cacheFile, false);
    uint32_t size = file == OS_INVALID_FILE ? 0 : static_cast<uint32_t>(OsGetFileSize(file));

    uint32_t time = s_accountDataTime++;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_UPDATE_ACCOUNT_DATA));
    msg.Put(static_cast<uint32_t>(type));
    msg.Put(time);
    msg.Put(size);

    if (size) {
        void* allocated = NULL;
        uint8_t* data;

        if (size > 0x1000) {
            allocated = SMemAlloc(size, __FILE__, __LINE__, 0);
            data = static_cast<uint8_t*>(allocated);
        } else {
            data = static_cast<uint8_t*>(alloca(size));
        }

        uint32_t bytesRead;
        OsReadFile(file, data, size, &bytesRead);

        uint32_t compressedSize = compressBound(size);
        uint8_t* compressedAllocated = NULL;
        uint8_t* compressed;

        if (compressedSize > 0x2000) {
            compressedAllocated = static_cast<uint8_t*>(SMemAlloc(compressedSize, __FILE__, __LINE__, 0));
            compressed = compressedAllocated;
        } else {
            compressed = static_cast<uint8_t*>(alloca(compressedSize));
        }

        zlib_compress(compressed, &compressedSize, data, size, 1);
        msg.PutData(compressed, compressedSize);

        if (allocated) {
            SMemFree(allocated, __FILE__, __LINE__, 0);
        }

        if (compressedAllocated) {
            SMemFree(compressedAllocated, __FILE__, __LINE__, 0);
        }
    }

    msg.Finalize();
    ClientServices::Send2(&msg);

    if (file != OS_INVALID_FILE) {
        OsCloseFile(file);
    }

    UpdateCachedChecksum(type, time);
}

// OFFSET: 0x004D7FB0/0x1401167B0
void DownloadAccountData(int32_t type) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_ACCOUNT_DATA));
    msg.Put(static_cast<uint32_t>(type));
    msg.Finalize();
    ClientServices::Send2(&msg);

    s_accountData[type].m_downloading = true;
}

// OFFSET: 0x004D8030/0x140116860
static int32_t SaveTimerHandler(const void* data, void* param) {
    int32_t type = static_cast<int32_t>(reinterpret_cast<intptr_t>(param));
    AccountDataInfo& info = s_accountData[type];
    uint8_t digest[16];
    MD5AccountFile(info.m_cacheFile, digest, info.m_global);

    if (!memcmp(digest, info.m_md5, sizeof(digest))) {
        info.m_changed = false;
        info.m_time = info.m_serverTime;
        SaveCachedChecksums(info.m_global);
    } else {
        UploadAccountData(type);
    }

    info.unk_0020 = 0;
    return 1;
}

// OFFSET: 0x004D80E0/0x140116900
static void ScheduleAccountDataSave(int32_t type) {
    AccountDataInfo& info = s_accountData[type];
    info.m_changed = true;

    if (!s_cvSynchronizeSettings->m_intValue) {
        return;
    }

    CVar* cvar = CVar::Lookup(info.m_cvarName);

    if ((cvar && !cvar->m_intValue) || info.m_localOnly) {
        return;
    }

    if (info.unk_0020) {
        ClientKillTimer(info.unk_0020, SaveTimerHandler, "SaveTimerHandler");
        info.unk_0020 = 0;
    }

    info.unk_0020 = ClientSetTimer(30000, SaveTimerHandler, reinterpret_cast<void*>(static_cast<intptr_t>(type)));
}

// OFFSET: 0x004D84F0/0x140116E80
static int32_t Packet_SMSG_ACCOUNT_DATA_TIMES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t serverTime = 0;
    msg->Get(serverTime);
    s_accountDataTime = serverTime;

    uint8_t unk;
    msg->Get(unk);

    uint32_t mask;
    msg->Get(mask);

    uint32_t count = 0;

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        if (mask & (1 << i)) {
            count++;
        }
    }

    if (msg->m_size - msg->m_read != count * sizeof(uint32_t)) {
        return 1;
    }

    for (int32_t type = 0; type < static_cast<int32_t>(sizeof(s_accountData) / sizeof(s_accountData[0])); type++) {
        AccountDataInfo& info = s_accountData[type];

        if (info.m_localOnly) {
            if (mask & (1 << type)) {
                uint32_t discard = 0;
                msg->Get(discard);
            }
        } else {
            if (!(mask & (1 << type))) {
                continue;
            }

            uint32_t typeTime = 0;
            msg->Get(typeTime);
            info.m_serverTime = typeTime;
        }

        if (!info.m_active) {
            continue;
        }

        if (!unk && info.m_global && info.m_serverTime < info.m_time) {
            info.m_serverTime = info.m_time;
        }

        info.m_downloading = false;

        if (s_cvSynchronizeSettings->m_intValue) {
            CVar* var = CVar::Lookup(info.m_cvarName);

            if ((!var || var->m_intValue) && !info.m_localOnly) {
                if (info.m_changed || info.m_time > info.m_serverTime) {
                    UploadAccountData(type);
                } else if (info.m_time < info.m_serverTime) {
                    DownloadAccountData(type);
                }
            }
        }

        if (!info.m_downloading && info.m_loadFunc) {
            LoadAccountData(type, info.m_loadFunc, info.m_loadParam);
        }
    }

    return 1;
}

// OFFSET: 0x004D8690/0x140117030
static int32_t Packet_SMSG_UPDATE_ACCOUNT_DATA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    uint32_t type;
    msg->Get(type);

    AccountDataInfo& info = s_accountData[type];

    if ((guid != CGlueMgr::GetActive() && !info.m_global) || !info.m_downloading) {
        msg->m_read = msg->m_size;
        return 1;
    }

    uint32_t serverTime = 0;
    msg->Get(serverTime);

    if (info.m_changed || info.m_time < static_cast<int32_t>(serverTime)) {
        HOSFILE file = OpenAccountFile(info.m_global, info.m_cacheFile, true);

        if (file != OS_INVALID_FILE) {
            uint32_t size;
            msg->Get(size);

            if (size) {
                uint32_t compressedSize = msg->m_size - msg->m_read;
                void* compressed;
                msg->GetDataInSitu(compressed, compressedSize);

                uint8_t* buffer;
                bool stackBuffer;

                if (size >= 0x2000) {
                    buffer = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
                    stackBuffer = false;
                } else {
                    buffer = static_cast<uint8_t*>(alloca(size));
                    stackBuffer = true;
                }

                uint32_t uncompressedSize = size;

                if (!zlib_uncompress(buffer, &uncompressedSize, static_cast<const uint8_t*>(compressed), compressedSize)) {
                    uint32_t bytesWritten;
                    OsWriteFile(file, buffer, uncompressedSize, &bytesWritten);
                }

                if (!stackBuffer) {
                    SMemFree(buffer, __FILE__, __LINE__, 0);
                }
            }

            OsCloseFile(file);
            UpdateCachedChecksum(type, serverTime);
        }
    } else {
        msg->m_read = msg->m_size;
    }

    info.m_downloading = false;

    if (info.m_loadFunc) {
        LoadAccountData(type, info.m_loadFunc, info.m_loadParam);
    }

    return 1;
}

// OFFSET: 0x004D8880/0x140117230
void AccountDataInitializeBasicSystem() {
    s_cvSynchronizeSettings = CVar::Register("synchronizeSettings", "Whether client settings should be stored on the server", 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    ClientServices::SetMessageHandler(SMSG_ACCOUNT_DATA_TIMES, Packet_SMSG_ACCOUNT_DATA_TIMES, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_ACCOUNT_DATA, Packet_SMSG_UPDATE_ACCOUNT_DATA, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, Packet_SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, NULL);
}

// PROVEN: ab393978 IDENTICAL/IDENTICAL
// OFFSET: 0x004D8160/0x140116980
void AccountDataShutdownBasicSystem() {
    ClientServices::ClearMessageHandler(SMSG_ACCOUNT_DATA_TIMES);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_ACCOUNT_DATA);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE);
}

// OFFSET: 0x004D81A0/0x1401169D0
void AccountDataInitialize(bool global) {
    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        CVar::Register(s_accountData[i].m_cvarName, NULL, 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    }

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        AccountDataInfo& info = s_accountData[i];

        if (info.m_global == global) {
            info.m_downloading = true;
            memset(info.m_md5, 0, sizeof(info.m_md5));
            info.m_time = 0;
            info.m_serverTime = 0;
            info.m_active = true;
        }
    }

    LoadCachedChecksums(global);

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        AccountDataInfo& info = s_accountData[i];

        if (info.m_global != global) {
            continue;
        }

        HOSFILE file = OpenAccountFile(info.m_global, info.m_oldFile, false);

        if (file == OS_INVALID_FILE) {
            CopyAccountFile(info.m_global, info.m_cacheFile, info.m_oldFile);
        } else {
            OsCloseFile(file);
        }
    }

    uint32_t bytesWritten;

    if (global) {
        if (unk_C7BFF8) {
            HOSFILE file = OpenAccountFile(s_accountData[0].m_global, s_accountData[0].m_cacheFile, true);

            if (file != OS_INVALID_FILE) {
                OsWriteFile(file, "", 0, &bytesWritten);
                OsCloseFile(file);
            }

            s_accountData[0].m_changed = true;
            s_accountData[0].m_downloading = false;
        }
    } else if (unk_C7BFFC) {
        HOSFILE file = OpenAccountFile(s_accountData[1].m_global, s_accountData[1].m_cacheFile, true);

        if (file != OS_INVALID_FILE) {
            OsWriteFile(file, "", 0, &bytesWritten);
            OsCloseFile(file);
        }

        s_accountData[1].m_downloading = false;
        s_accountData[1].m_changed = true;
    }
}

// PROVEN: 673c4585 IDENTICAL/DIFFERENT
// OFFSET: 0x004D8320/0x140116BB0
void AccountDataFlush() {
    for (int32_t i = 0; i < static_cast<int32_t>(sizeof(s_accountData) / sizeof(s_accountData[0])); i++) {
        if (s_accountData[i].unk_0020) {
            ClientKillTimer(s_accountData[i].unk_0020, SaveTimerHandler, "SaveTimerHandler");
            SaveTimerHandler(NULL, reinterpret_cast<void*>(static_cast<intptr_t>(i)));
        }
    }
}

// OFFSET: 0x004D83F0/0x140116CB0
void SaveAccountData(int32_t type, ACCOUNTDATALOADFUNC func, void* param) {
    AccountDataInfo& info = s_accountData[type];

    if (info.m_downloading) {
        return;
    }

    HOSFILE localFile = OpenAccountFile(info.m_global, info.m_localFile, true);
    HOSFILE cacheFile = OpenAccountFile(info.m_global, info.m_cacheFile, true);
    func(localFile, cacheFile, param);

    if (localFile != OS_INVALID_FILE) {
        OsCloseFile(localFile);
    }

    if (cacheFile != OS_INVALID_FILE) {
        OsCloseFile(cacheFile);
    }

    ScheduleAccountDataSave(type);
}

// OFFSET: 0x004D8470/0x140116D60
void UploadAccountData() {
    AccountDataFlush();

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        AccountDataInfo& info = s_accountData[i];
        info.m_downloading = false;

        if (info.m_changed || info.m_time != info.m_serverTime) {
            UploadAccountData(i);
        }
    }
}

// OFFSET: 0x004D84B0/0x140116DF0
void DownloadAccountData() {
    AccountDataFlush();

    for (uint32_t i = 0; i < sizeof(s_accountData) / sizeof(s_accountData[0]); i++) {
        AccountDataInfo& info = s_accountData[i];

        if ((info.m_changed || info.m_time != info.m_serverTime) && !info.m_downloading) {
            DownloadAccountData(i);
        }
    }
}
