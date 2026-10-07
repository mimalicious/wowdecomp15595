#ifndef WOW_SOURCE_WORLDCLIENT_SHADOW_H
#define WOW_SOURCE_WORLDCLIENT_SHADOW_H

#include <cstdint>

class C3Vector;
class CAaBox;

int32_t ShadowBlobsEnabled();
void ShadowDestroy();
void ShadowInit();
void ShadowRender(const C3Vector& position, float facing, float scale, const CAaBox& box, float alpha, const C3Vector& cameraPos);

#endif
