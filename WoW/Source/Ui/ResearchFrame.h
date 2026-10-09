#ifndef WOW_SOURCE_UI_RESEARCHFRAME_H
#define WOW_SOURCE_UI_RESEARCHFRAME_H

#include <cstdint>

class CGResearchFrame {
    public:
    static bool IsInsideResearchSite(int32_t siteId);
    static bool CanScanResearchSite(int32_t researchField);
};

#endif
