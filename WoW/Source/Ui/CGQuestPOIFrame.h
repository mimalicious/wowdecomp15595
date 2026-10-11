#ifndef WOW_SOURCE_UI_CGQUESTPOIFRAME_H
#define WOW_SOURCE_UI_CGQUESTPOIFRAME_H

#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/Ui/BlobFrame.h"

class CSimpleFrame;
struct lua_State;

class CGQuestPOIFrame : public CGBlobFrame {
    public:
    static CDataAllocator s_questPOIHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    ~CGQuestPOIFrame();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void DrawBlob(int32_t id, bool show) override;
    void MergeBlobs(uint32_t index) override;

    CGQuestPOIFrame(CSimpleFrame* parent);
    void UpdateMouseOverPOI(float x, float y);
};

#include "WoW/Source/Ui/CGQuestPOIFrame.inl"

#endif
