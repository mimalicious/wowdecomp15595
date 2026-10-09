#ifndef WOW_SOURCE_UI_BLOBFRAME_H
#define WOW_SOURCE_UI_BLOBFRAME_H

class CSimpleFrame;

class CGBlobFrame {
    public:
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
};

#endif
