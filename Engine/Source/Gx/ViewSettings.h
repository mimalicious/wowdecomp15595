#ifndef ENGINE_SOURCE_GX_VIEWSETTINGS_H
#define ENGINE_SOURCE_GX_VIEWSETTINGS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "Engine/Source/Tempest/CImVector.h"

namespace ViewSettings {
    C4Vector CalculateHomToScreen(float offsetX, float offsetY);
    C4Vector CalculateParams();
    void GetFog(uint32_t index, float& start, float& end, float& density, CImVector& color);
    int32_t GetSubmerged();
    void SetFog(uint32_t index, float start, float end, float density, const CImVector& color);
    void SetSubmerged(int32_t submerged);
    void SetViewExtents(const CAaRect& extents);
    void SetViewportFarClip(float farClip);
    void SetZNearFar(float zNear, float zFar);
}

#endif
