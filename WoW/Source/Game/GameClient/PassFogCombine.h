#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSFOGCOMBINE_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSFOGCOMBINE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;
struct CGxBuf;
struct CGxPool;

class PassFogCombine : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;
    /* 0054 */ CGxPool* m_vertexPool;
    /* 0058 */ CGxBuf* m_vertexBuf;
    /* 005C */ CGxPool* m_indexPool;
    /* 0060 */ CGxBuf* m_indexBuf;
    /* 0064 */ C2iVector m_lastSize;
    /* 006C */ float m_density;
    /* 0070 */ float m_fadeTime;

    virtual ~PassFogCombine();
    virtual bool Valid();
    virtual void Execute();

    PassFogCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
    void BuildCircularMappingIndices();
    void BuildCircularMappingVertices(const FFX::Tex& tex0, const FFX::Tex& tex1);
};

#endif
