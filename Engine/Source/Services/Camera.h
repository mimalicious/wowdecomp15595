#ifndef ENGINE_SOURCE_SERVICES_CAMERA_H
#define ENGINE_SOURCE_SERVICES_CAMERA_H

#include <cstddef>
#include <cstdint>

class C2Vector;
class C3Vector;
class CCamera;
class CRect;

void CameraAddRef(CCamera* camera);
CCamera* CameraCreate();
CCamera* CameraDuplicate(CCamera* camera);
void CameraGetCoord(CCamera* camera, uint32_t index, C3Vector* value);
void CameraGetLineSegment(float x, float y, C3Vector* start, C3Vector* end);
float CameraGetFloat(CCamera* camera, uint32_t index);
void CameraRelease(CCamera*& camera);
void CameraSetCoord(CCamera* camera, uint32_t index, const C3Vector& value);
void CameraSetFloat(CCamera* camera, uint32_t index, float value);
void CameraSetupScreenProjection(const CRect& rect, const C2Vector& offset, float depth, int32_t forceOrtho);
void CameraSetupWorldProjection(CCamera* camera, const CRect& rect, uint32_t flags);

#endif
