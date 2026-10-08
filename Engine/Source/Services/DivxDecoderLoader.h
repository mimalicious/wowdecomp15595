#ifndef ENGINE_SOURCE_SERVICES_DIVXDECODERLOADER_H
#define ENGINE_SOURCE_SERVICES_DIVXDECODERLOADER_H

#include <cstddef>
#include <cstdint>
#include "ThirdParty/DivxDecoder/DivxDecoder.h"

typedef int32_t (*DIVXDECODE)(int32_t decoder, DivxDecodeFrame* frame, int32_t flags);
typedef int32_t (*INITIALIZEDIVXDECODER)(int32_t decoder, int32_t width, int32_t height);
typedef int32_t (*SETOUTPUTFORMAT)(int32_t decoder, int32_t format, int32_t width, int32_t height);
typedef int32_t (*UNINITIALIZEDIVXDECODER)(int32_t decoder);

extern DIVXDECODE g_divxDecode;
extern INITIALIZEDIVXDECODER g_initializeDivxDecoder;
extern SETOUTPUTFORMAT g_setOutputFormat;
extern UNINITIALIZEDIVXDECODER g_unInitializeDivxDecoder;

void* DivxDecoderAlloc(uint32_t size);
void DivxDecoderFree(void* ptr);
int32_t LoadDivxDecoder();
void UnloadDivxDecoder();

#endif
