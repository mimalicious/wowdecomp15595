#ifndef ENGINE_SOURCE_FFX_FFX_H
#define ENGINE_SOURCE_FFX_FFX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxFrameBuffer;
class CGxShader;
class CGxTex;
class CTexture;
struct CVar;

namespace FFX {
    struct Tex {
        /* 0000 */ CTexture* m_texture;
        /* 0004 */ CGxFrameBuffer* m_frameBuffer;
        /* 0008 */ C2iVector m_texSize;
        /* 0010 */ C2iVector m_size;
        /* 0018 */ C2Vector m_texelSize;
    };

    class Pass {
        public:
        /* 0004 */ Tex* m_inputs[16];
        /* 0044 */ Tex* m_output;
        /* 0048 */ EGxBlend m_blend;

        virtual ~Pass();
        virtual bool Valid() = 0;
        virtual void Execute() = 0;

        Pass(Tex** inputs, uint32_t count, Tex* output, EGxBlend blend);
    };

    class Effect {
        public:
        /* 0004 */ CVar* m_cvar;
        /* 0008 */ TSGrowableArray<Pass*> m_passes;

        virtual ~Effect();
        virtual bool Valid();
        virtual void Execute();
        virtual void Begin();
        virtual void End();
        virtual void SetParameters(int32_t count, const int32_t* params);

        Effect();
    };

    struct PortalEffect {
        /* 0000 */ Effect* m_effect;
        /* 0004 */ C44Matrix m_matrix;
        /* 0044 */ const C3Vector* m_geo;
        /* 0048 */ const C3Vector* m_normal;
        /* 004C */ const CImVector* m_col0;
        /* 0050 */ const CImVector* m_col1;
        /* 0054 */ const C2Vector* m_tex;
        /* 0058 */ const C4Vector* m_tangent;
        /* 005C */ const uint16_t* m_idx;
        /* 0060 */ CGxBatch m_batch;

        PortalEffect();
    };

    extern bool s_useRECT;
    extern bool s_defaultEnabled;
    extern CVar* s_cvarFFXRectangle;
    extern CVar* s_cvarFFX;
    extern float s_oldMaxZ;
    extern float s_oldMinZ;
    extern float s_oldMaxY;
    extern float s_oldMinY;
    extern float s_oldMaxX;
    extern float s_oldMinX;
    extern const uint16_t* s_idx;
    extern const C2Vector* s_tex;
    extern const CImVector* s_col1;
    extern const CImVector* s_col0;
    extern const C3Vector* s_normal;
    extern const C3Vector* s_geo;
    extern CGxShader* s_passThrough_PCT4;
    extern CGxShader* s_passThrough_PCT3;
    extern CGxShader* s_passThrough_PCT2;
    extern CGxShader* s_passThrough_PCT;
    extern PortalEffect* s_portalEffect;
    extern Effect* s_effect[3];
    extern uint32_t s_curTimeMs;
    extern float s_tickTimeSec;
    extern CGxFrameBuffer* s_oldFrameBuffer;
    extern C3Vector s_camPos;
    extern Tex s_waveTex;
    extern Tex s_screen;
    extern C4Vector psProcWaterConsts[15];
    extern TSGrowableArray<PortalEffect> s_portalEffects;
    extern Tex s_full[2];
    extern Tex s_quart[3];
    extern C2iVector s_wavePeriod;
    extern float s_waveScaleX;
    extern float s_waveScaleY;
    extern CGxBatch s_batch;
    extern C44Matrix s_oldProjMtxNative;
    extern C44Matrix s_oldProjMtx;
    extern C44Matrix s_oldViewMtx;

    void AddPortalEffect(Effect* effect, const C44Matrix& matrix, const C3Vector* geo, const C3Vector* normal, const CImVector* col0, const CImVector* col1, const C2Vector* tex, const C4Vector* tangent, const uint16_t* idx, const CGxBatch& batch);
    void AllocTex();
    void CreateTex(Tex& tex, const C2iVector& size, const char* name, bool depth, bool renderTarget);
    bool CVarCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    void Destroy();
    void DoPass(uint32_t pass);
    void Finish();
    int32_t EffectIsSet(Effect* effect);
    void FlushPortalEffects();
    C44Matrix* GetOldViewMtx();
    C4Vector GetWaveAmplitude(const C2Vector& texelSize, float amplitude);
    C44Matrix GetWaveMtx(const C2iVector& size);
    void Init();
    void InitCVars();
    void Render();
    void SetEffect(uint32_t index, Effect* effect);
    void SetupGeo_PortalEffect(C44Matrix* modelView, C44Matrix* projection, C44Matrix* model);
    void SetupGeo_Quad(const C2iVector& size, C44Matrix* matrix);
    void SetupGx(Tex* target, int32_t depth);
    void SetupIB();
    void SetupReset();
    void SetupTexMtx(const C2iVector& size, const C2iVector& texSize, C44Matrix* matrix, int32_t allowRect);
    void SetupVB_PCT(const C44Matrix& posMtx, const C44Matrix& texMtx, const float* shade);
    void SetupVB_PCT(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& texMtx);
    void SetupVB_PCT2(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& tex0Mtx, const C44Matrix& tex1Mtx);
    void SetupVB_PCT3(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& tex0Mtx, const C44Matrix& tex1Mtx, const C44Matrix& tex2Mtx);
    void SetupVB_PCT4(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& texMtx, const C2Vector* offsets, const C2Vector& scale);
    void SetupVB_PNC2T_TS();
    void SetupVB_PNCT();
    void SetupVB_PT(const C44Matrix& posMtx, const C44Matrix& texMtx);
    void Start();
}

#include "Engine/Source/FFX/FFX.inl"

#endif
