#ifndef WOW_SOURCE_UI_ARCHAEOLOGYDIGSITEFRAME_H
#define WOW_SOURCE_UI_ARCHAEOLOGYDIGSITEFRAME_H

#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/Ui/BlobFrame.h"

class CSimpleFrame;
struct lua_State;

class CGArchaeologyDigSiteFrame : public CGBlobFrame {
    public:
    static CDataAllocator s_digSiteHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    ~CGArchaeologyDigSiteFrame();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void DrawBlob(int32_t id, bool show) override;

    CGArchaeologyDigSiteFrame(CSimpleFrame* parent);
};

#endif
