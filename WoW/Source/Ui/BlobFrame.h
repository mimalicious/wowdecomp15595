#ifndef WOW_SOURCE_UI_BLOBFRAME_H
#define WOW_SOURCE_UI_BLOBFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CRect.h"

class CRenderBatch;
class CStatus;
class CTexture;
class linkObject;
struct BLOB;
struct QuestPOIInfo;
struct BLOBPOI;
struct lua_State;

class CGBlobFrame : public CSimpleFrame {
    public:
    struct BlobRenderData {
        /* 0000 */ BLOB* m_blob;
        /* 0004 */ TSGrowableArray<C3Vector> m_fillVerts;
        /* 0014 */ TSGrowableArray<C2Vector> m_fillTexCoords;
        /* 0024 */ TSGrowableArray<uint16_t> m_fillIndices;
        /* 0034 */ TSGrowableArray<C3Vector> m_borderVerts;
        /* 0044 */ TSGrowableArray<C2Vector> m_borderTexCoords;
        /* 0054 */ TSGrowableArray<uint16_t> m_borderIndices;
        /* 0064 */ CRect m_bounds;
        /* 0074 */ int32_t m_objectives[4];
        /* 0084 */ uint32_t m_numObjectives;
        /* 0088 */ uint8_t m_valid;

        ~BlobRenderData();
    };

    struct BlobRenderGroup {
        /* 0000 */ QuestPOIInfo* m_info;
        /* 0004 */ uint8_t m_dirty;
        /* 0008 */ TSGrowableArray<BlobRenderData> m_renderData;
    };

    static CDataAllocator s_blobHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);
    static void RenderCallback(void* param);
    static void BuildBlobOutline(BLOB* blob, TSGrowableArray<BLOBPOI>* points, bool smoothing, uint32_t numSplinePoints);

    /* 01F0 */ CTexture* m_fillTexture;
    /* 01F4 */ CTexture* m_borderTexture;
    /* 01F8 */ uint8_t m_fillAlpha;
    /* 01F9 */ uint8_t m_borderAlpha;
    /* 01FC */ float m_borderScalar;
    /* 0200 */ uint8_t m_smoothing;
    /* 0201 */ uint8_t m_merging;
    /* 0204 */ float m_mergeThreshold;
    /* 0208 */ uint32_t m_numSplinePoints;
    /* 020C */ int32_t m_mouseOverQuestId;
    /* 0210 */ uint32_t m_mouseOverCount;
    /* 0214 */ int32_t m_mouseOverObjectives[4];
    /* 0224 */ BlobRenderGroup m_groups[4];

    ~CGBlobFrame();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void OnFrameRender(CRenderBatch* batch, uint32_t layer) override;
    void LoadXML(linkObject* node, CStatus* status) override;
    virtual void Render();
    virtual void DrawBlob(int32_t id, bool show);
    virtual void DrawNone();
    virtual void MergeBlobs(uint32_t index);

    CGBlobFrame(CSimpleFrame* parent);
    void BuildBlobRenderData(BlobRenderData* data, BLOB* blob, bool filter, int32_t mask);
    void SetFillTexture(const char* fileName);
    void SetBorderTexture(const char* fileName);
    void RemoveBlob(int32_t id);
    bool BuildRenderData(int32_t index, bool filter);
    void AddBlob(QuestPOIInfo* info, bool filter);
};

#include "WoW/Source/Ui/BlobFrame.inl"

#endif
