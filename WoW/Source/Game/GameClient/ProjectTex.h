#ifndef WOW_SOURCE_GAME_GAMECLIENT_PROJECTTEX_H
#define WOW_SOURCE_GAME_GAMECLIENT_PROJECTTEX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"

class C3Vector;
class C44Matrix;
class CAaBox;
class CGxTex;
class CImVector;
class CTexture;

CGxTex* ProjectTex2dGetAddFadeTex();
CTexture* ProjectTex2dGetModFadeTex();
void ProjectTex2dDraw(const CAaBox& box, const CImVector& color, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset);
void ProjectTex2dDrawShaderEffect(const CAaBox& box, const CImVector& color, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset);
void ProjectTexDestroy();
void ProjectTexInitialize();
int32_t ProjectTex2dMakeMatrices(C44Matrix& texMatrix, C44Matrix& fadeMatrix, const CAaBox& box, const C3Vector* scale, const C44Matrix* rotation, float fade, int32_t local, int32_t flatDepth);
void ProjectTex2d(const CAaBox& box, const CImVector& color, const C44Matrix* rotation, float fade, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset);
void ProjectTex2d(const CAaBox& box, const CImVector& color, const C3Vector* scale, const C44Matrix* rotation, float fade, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset);

#endif
