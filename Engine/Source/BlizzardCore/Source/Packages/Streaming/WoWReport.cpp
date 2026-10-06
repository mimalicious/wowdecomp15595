#include "WoWReport.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Report.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: %0x00D812E1
static bool s_enteredWorld;

// STATIC: %0x00D812F8
static int32_t s_startTime;

// STATIC: %0x00D81300
static int64_t s_initialLocal;

// OFFSET: %0x0070C0C0
void WoWReport::SetInitialDownloadData() {
    int64_t total = 0;
    s_initialLocal = 0;

    if (Blizzard::Streaming::IsStreamingMode()) {
        Blizzard::Mopaq::SFileGetTotalIsLocalAmount(s_initialLocal, total);
    }

    Report::SetData("lc", s_initialLocal);
}

// OFFSET: %0x0070C140
void WoWReport::SetFinalDownloadData() {
    std::string bandwidth = Blizzard::Streaming::ManifestInfo::Get()->GetBandwidthDataString();

    int32_t rate;
    sscanf(bandwidth.c_str(), "b,%d", &rate);
    Report::SetData("bw", rate);

    int64_t local = 0;
    int64_t total = 0;

    if (Blizzard::Streaming::IsStreamingMode()) {
        Blizzard::Mopaq::SFileGetTotalIsLocalAmount(local, total);
    }

    local -= s_initialLocal;
    Report::SetData("dn", local);
}

// OFFSET: %0x0070C260
void WoWReport::SetConverted(const char* version, const char* language) {
    bool initialized = Report::IsInitialized();

    if (!initialized) {
        Report::Initialize(std::string("STWO"), std::string(version), std::string(language));
    }

    Report::SetData("stg", std::string("conv"));
    Report::SendReport();

    if (!initialized) {
        Report::Shutdown();
    }
}

// OFFSET: %0x0070C3E0
void WoWReport::Shutdown() {
    Report::SetData("rt", static_cast<int64_t>(static_cast<uint32_t>(Blizzard::Time::Seconds() - s_startTime)));

    int32_t status;

    if (s_enteredWorld) {
        Report::SetData("stg", std::string("fini"));
        status = 1;
    } else {
        status = 2;
    }

    Report::SetData("sta", status);
    SetFinalDownloadData();
    Report::SendReport();
    Report::Shutdown();
}

// OFFSET: %0x0070C4E0
void WoWReport::SetStage(const char* stage) {
    Report::SetData("stg", std::string(stage));

    if (!Blizzard::String::Equal(stage, "wrld") || s_enteredWorld) {
        return;
    }

    s_enteredWorld = true;
    Report::SetData("rt", static_cast<int64_t>(static_cast<uint32_t>(Blizzard::Time::Seconds() - s_startTime)));
    SetFinalDownloadData();
    Report::SetData("sta", 1);
    Report::SendReport();
    s_startTime = static_cast<int32_t>(Blizzard::Time::Seconds());
    SetInitialDownloadData();
}

// OFFSET: %0x0070C610
void WoWReport::Initialize(const char* version, const char* language, bool converted, bool trial) {
    s_startTime = static_cast<int32_t>(Blizzard::Time::Seconds());
    const char* app = "SWOW";

    if (trial) {
        app = "STWO";

        if (converted) {
            app = "STW2";

            if (rand() % 100) {
                return;
            }
        }
    } else if (rand() % 100) {
        return;
    }

    Report::Initialize(std::string(app), std::string(version), std::string(language));
    SetStage("init");
    Report::SetData("sta", 0);
    Report::SendReport();
}
