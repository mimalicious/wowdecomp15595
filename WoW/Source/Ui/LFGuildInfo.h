#ifndef WOW_SOURCE_UI_LFGUILDINFO_H
#define WOW_SOURCE_UI_LFGUILDINFO_H

class CGLFGuildInfo {
    public:
    static void ShutdownGame();
    static void InitializeGame();
    static void ClearRecruitmentSettings(bool signal);
};

#endif
