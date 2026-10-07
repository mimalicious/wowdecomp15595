#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJRENDER_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJRENDER_H

class CMapObjDefGroup;
class CWorldView;

class CMapObjRender {
    public:
    static void GatherGroup(CWorldView* view, CMapObjDefGroup* group);
};

#endif
