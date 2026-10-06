#ifndef WOW_SOURCE_DB_WOWCLIENTDB2_H
#define WOW_SOURCE_DB_WOWCLIENTDB2_H

#include <cstdint>
#include "Engine/Source/Os/OsFile.h"

struct ClientDBReply;
struct ClientHotfixNotify;
struct ClientHotfixNotifyBlob;

class WowClientDB2_Base {
    public:
    static void GetCacheDir(char* buffer, const char* fileName, bool create);
    static WowClientDB2_Base* GetTableByHash(uint32_t tableHash);
    static bool HotfixNotifyBlob(ClientHotfixNotifyBlob* notify);
    static bool HotfixNotifyMultiplex(ClientHotfixNotify* notify);
    static bool DBReplyMultiplex(ClientDBReply* reply);

    void RestoreHotfix(uint32_t recordID, uint32_t timestamp);
    void Load(uint32_t hotfixTime);
    void Save(uint32_t hotfixTime);
    void Unload();
    void WriteHotfixes(HOSFILE file);
};

extern WowClientDB2_Base* g_clientDB2Tables[];
extern bool (*g_hotfixNotifyBlobHandler)(ClientHotfixNotifyBlob* notify);
extern bool (*g_hotfixNotifyMultiplexHandler)(ClientHotfixNotify* notify);
extern bool (*g_dbReplyMultiplexHandler)(ClientDBReply* reply);

void ClientDB2StartThread();

#endif
