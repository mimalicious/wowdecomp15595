#ifndef ENGINE_SOURCE_GX_CGXDEVICEOPENGL_CGXOGLTABLES_H
#define ENGINE_SOURCE_GX_CGXDEVICEOPENGL_CGXOGLTABLES_H

#include "Engine/Source/Gx/CGxDeviceOpenGL/GlExt.h"
#include "Engine/Source/Gx/Types.h"

class CGxFormat;

extern const GLenum s_glTexTarget[4];
extern const GLint s_glInternalFormat[GxTexFormats_Last];
extern const GLenum s_glFormat[GxTexFormats_Last];
extern const GLenum s_glType[GxTexFormats_Last];
extern GLint s_glWrapMode[3];

HGLRC CreateGLContext(HDC dc, const CGxFormat& format);
BOOL WglDestroyContext(HGLRC context);

#endif
