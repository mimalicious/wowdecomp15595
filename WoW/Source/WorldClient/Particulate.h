#ifndef WOW_SOURCE_WORLDCLIENT_PARTICULATE_H
#define WOW_SOURCE_WORLDCLIENT_PARTICULATE_H

#include <cstdint>

class CWorldView;

class Particulate {
    public:
    /* 0000 */ uint8_t unk_0000[64020];
    /* FA14 */ uint8_t m_enabled;
    /* FA15 */ uint8_t unk_FA15[43];

    Particulate(float scale, float a2, const char* textureName);
    ~Particulate();
    void Render(CWorldView* view);
    void Update();
    void SetLiquidType(uint32_t liquidType);
    void SetScale(float scale);
};

#endif
