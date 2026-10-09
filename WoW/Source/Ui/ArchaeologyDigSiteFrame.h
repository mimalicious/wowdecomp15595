#ifndef WOW_SOURCE_UI_ARCHAEOLOGYDIGSITEFRAME_H
#define WOW_SOURCE_UI_ARCHAEOLOGYDIGSITEFRAME_H

class CSimpleFrame;

class CGArchaeologyDigSiteFrame {
    public:
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
};

#endif
