#include "WoW/Source/JamClientAutoCode/JamReflection.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

static bool JamDefault_SetDefaultTrue(void* data);
static JamValue* JamDefault_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamDefault_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamDefault_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static uint32_t JamDefault_GetMemberCount(void* data, const void* meta);
static uint32_t JamDefault_GetMemberCapacity(void* data, const void* meta);
static JamValue* JamDefault_GetElement(JamValue* out);
static bool JamDefault_SetString(void* data, const void* meta, const char* value);
static bool JamDefault_SetInt32(void* data, const void* meta, int32_t value);
static bool JamDefault_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamDefault_SetInt64(void* data, const void* meta, int64_t value);
static bool JamDefault_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamDefault_SetFloat(void* data, const void* meta, float value);
static bool JamDefault_SetBool(void* data, const void* meta, bool value);
static bool JamDefault_GetString(const void* data, const void* meta, char* buffer, uint32_t size);
static bool JamDefault_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamDefault_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamDefault_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamDefault_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamDefault_GetFloat(const void* data, const void* meta, float* value);
static bool JamDefault_GetBool(const void* data, const void* meta, bool* value);
static bool JamDefault_SetDefault(void* data);
static JamValue* JamStruct_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamStruct_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamStruct_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static uint32_t JamStruct_GetMemberCount(void* data, const void* meta);
static uint32_t JamStruct_GetMemberCapacity(void* data, const void* meta);
static bool JamInt8_SetDefault(void* data);
static bool JamInt8_SetString(void* data, const void* meta, const char* value);
static bool JamInt8_SetInt32(void* data, const void* meta, int32_t value);
static bool JamInt8_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamInt8_SetInt64(void* data, const void* meta, int64_t value);
static bool JamInt8_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamInt8_SetFloat(void* data, const void* meta, float value);
static bool JamInt8_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamInt8_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamInt8_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamInt8_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamUInt8_SetDefault(void* data);
static bool JamUInt8_SetString(void* data, const void* meta, const char* value);
static bool JamUInt8_SetInt32(void* data, const void* meta, int32_t value);
static bool JamUInt8_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamUInt8_SetInt64(void* data, const void* meta, int64_t value);
static bool JamUInt8_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamUInt8_SetFloat(void* data, const void* meta, float value);
static bool JamUInt8_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamUInt8_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamUInt8_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamUInt8_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamInt16_SetDefault(void* data);
static bool JamInt16_SetString(void* data, const void* meta, const char* value);
static bool JamInt16_SetInt32(void* data, const void* meta, int32_t value);
static bool JamInt16_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamInt16_SetInt64(void* data, const void* meta, int64_t value);
static bool JamInt16_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamInt16_SetFloat(void* data, const void* meta, float value);
static bool JamInt16_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamInt16_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamInt16_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamInt16_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamUInt16_SetDefault(void* data);
static bool JamUInt16_SetString(void* data, const void* meta, const char* value);
static bool JamUInt16_SetInt32(void* data, const void* meta, int32_t value);
static bool JamUInt16_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamUInt16_SetInt64(void* data, const void* meta, int64_t value);
static bool JamUInt16_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamUInt16_SetFloat(void* data, const void* meta, float value);
static bool JamUInt16_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamUInt16_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamUInt16_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamUInt16_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamInt32_SetDefault(void* data);
static bool JamInt32_SetString(void* data, const void* meta, const char* value);
static bool JamInt32_SetInt32(void* data, const void* meta, int32_t value);
static bool JamInt32_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamInt32_SetInt64(void* data, const void* meta, int64_t value);
static bool JamInt32_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamInt32_SetFloat(void* data, const void* meta, float value);
static bool JamInt32_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamInt32_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamInt32_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamInt32_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamUInt32_SetDefault(void* data);
static bool JamUInt32_SetString(void* data, const void* meta, const char* value);
static bool JamUInt32_SetInt32(void* data, const void* meta, int32_t value);
static bool JamUInt32_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamUInt32_SetInt64(void* data, const void* meta, int64_t value);
static bool JamUInt32_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamUInt32_SetFloat(void* data, const void* meta, float value);
static bool JamUInt32_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamUInt32_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamUInt32_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamUInt32_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamInt64_SetDefault(void* data);
static bool JamInt64_SetString(void* data, const void* meta, const char* value);
static bool JamInt64_SetInt32(void* data, const void* meta, int32_t value);
static bool JamInt64_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamInt64_SetInt64(void* data, const void* meta, int64_t value);
static bool JamInt64_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamInt64_SetFloat(void* data, const void* meta, float value);
static bool JamInt64_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamInt64_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamInt64_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamInt64_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamUInt64_SetDefault(void* data);
static bool JamUInt64_SetString(void* data, const void* meta, const char* value);
static bool JamUInt64_SetInt32(void* data, const void* meta, int32_t value);
static bool JamUInt64_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamUInt64_SetInt64(void* data, const void* meta, int64_t value);
static bool JamUInt64_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamUInt64_SetFloat(void* data, const void* meta, float value);
static bool JamUInt64_GetInt32(const void* data, const void* meta, int32_t* value);
static bool JamUInt64_GetUInt32(const void* data, const void* meta, uint32_t* value);
static bool JamUInt64_GetInt64(const void* data, const void* meta, int64_t* value);
static bool JamUInt64_GetUInt64(const void* data, const void* meta, uint64_t* value);
static bool JamFloat_SetDefault(void* data);
static bool JamFloat_SetString(void* data, const void* meta, const char* value);
static bool JamFloat_SetInt32(void* data, const void* meta, int32_t value);
static bool JamFloat_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamFloat_SetInt64(void* data, const void* meta, int64_t value);
static bool JamFloat_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamFloat_SetFloat(void* data, const void* meta, float value);
static bool JamFloat_GetFloat(const void* data, const void* meta, float* value);
static JamValue* JamC2iVector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamC2iVector_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamC2iVector_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static uint32_t JamDefault_GetMemberCount2(void* data, const void* meta);
static uint32_t JamDefault_GetMemberCapacity2(void* data, const void* meta);
static JamValue* JamC3iVector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamC3iVector_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamC3iVector_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static uint32_t JamDefault_GetMemberCount3(void* data, const void* meta);
static uint32_t JamDefault_GetMemberCapacity3(void* data, const void* meta);
static JamValue* JamC4iVector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamC4iVector_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamC4iVector_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static uint32_t JamDefault_GetMemberCount4(void* data, const void* meta);
static uint32_t JamDefault_GetMemberCapacity4(void* data, const void* meta);
static JamValue* JamC2Vector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamC2Vector_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamC2Vector_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static JamValue* JamC3Vector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamC3Vector_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamC3Vector_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static JamValue* JamC4Vector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamC4Vector_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamC4Vector_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static bool JamString_SetDefault(void* data);
static bool JamString_SetString(void* data, const void* meta, const char* value);
static bool JamString_SetInt32(void* data, const void* meta, int32_t value);
static bool JamString_SetUInt32(void* data, const void* meta, uint32_t value);
static bool JamString_SetInt64(void* data, const void* meta, int64_t value);
static bool JamString_SetUInt64(void* data, const void* meta, uint64_t value);
static bool JamString_SetFloat(void* data, const void* meta, float value);
static bool JamString_GetString(const void* data, const void* meta, char* buffer, uint32_t size);
static JamValue* JamRealmAddress_GetMember(JamValue* out, void* data, const void* meta, uint32_t index);
static const char* JamRealmAddress_GetMemberName(void* data, const void* meta, uint32_t index);
static JamValue* JamRealmAddress_FindMember(JamValue* out, void* data, const void* meta, const char* name);
static bool JamBool_SetDefault(void* data);
static bool JamBool_SetBool(void* data, const void* meta, bool value);
static bool JamBool_GetBool(const void* data, const void* meta, bool* value);
static bool JamComplaintType_SetDefault(void* data);
static bool JamComplaintType_SetString(void* data, const void* meta, const char* value);
static bool JamComplaintType_GetString(const void* data, const void* meta, char* buffer, uint32_t size);
static bool JamSplineFace_SetDefault(void* data);
static bool JamSplineFace_SetString(void* data, const void* meta, const char* value);
static bool JamSplineFace_GetString(const void* data, const void* meta, char* buffer, uint32_t size);
static bool JamUpdateCollisionHeightReason_SetDefault(void* data);
static bool JamUpdateCollisionHeightReason_SetString(void* data, const void* meta, const char* value);
static bool JamUpdateCollisionHeightReason_GetString(const void* data, const void* meta, char* buffer, uint32_t size);
static bool JamTradeStatus_SetDefault(void* data);
static bool JamTradeStatus_SetString(void* data, const void* meta, const char* value);
static bool JamTradeStatus_GetString(const void* data, const void* meta, char* buffer, uint32_t size);

// STATIC: 0x01008998
static uint32_t s_vectorMeta[4];

// STATIC: 0x00D2776C
static const char* s_c2iVectorNames[2] = { "x", "y" };

// STATIC: 0x00D2777C
static const char* s_c3iVectorNames[3] = { "x", "y", "z" };

// STATIC: 0x00D2778C
static const char* s_c4iVectorNames[4] = { "x", "y", "z", "w" };

// STATIC: 0x00D2779C
static const char* s_c2VectorNames[2] = { "x", "y" };

// STATIC: 0x00D277AC
static const char* s_c3VectorNames[3] = { "x", "y", "z" };

// STATIC: 0x00D277BC
static const char* s_c4VectorNames[4] = { "x", "y", "z", "w" };

// STATIC: 0x00C45F90
const JamTypeInfo g_jamTypeNone = {
    0,
    JamDefault_SetDefaultTrue,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C45FE8
const JamTypeInfo g_jamTypeStruct = {
    2,
    JamDefault_SetDefault,
    JamStruct_GetMember,
    JamStruct_GetMemberName,
    JamStruct_FindMember,
    JamStruct_GetMemberCount,
    JamStruct_GetMemberCapacity,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46040
const JamTypeInfo g_jamTypeInt8 = {
    1,
    JamInt8_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamInt8_SetString,
    JamInt8_SetInt32,
    JamInt8_SetUInt32,
    JamInt8_SetInt64,
    JamInt8_SetUInt64,
    JamInt8_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamInt8_GetInt32,
    JamInt8_GetUInt32,
    JamInt8_GetInt64,
    JamInt8_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46098
const JamTypeInfo g_jamTypeUInt8 = {
    1,
    JamUInt8_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamUInt8_SetString,
    JamUInt8_SetInt32,
    JamUInt8_SetUInt32,
    JamUInt8_SetInt64,
    JamUInt8_SetUInt64,
    JamUInt8_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamUInt8_GetInt32,
    JamUInt8_GetUInt32,
    JamUInt8_GetInt64,
    JamUInt8_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C460F0
const JamTypeInfo g_jamTypeInt16 = {
    1,
    JamInt16_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamInt16_SetString,
    JamInt16_SetInt32,
    JamInt16_SetUInt32,
    JamInt16_SetInt64,
    JamInt16_SetUInt64,
    JamInt16_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamInt16_GetInt32,
    JamInt16_GetUInt32,
    JamInt16_GetInt64,
    JamInt16_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46148
const JamTypeInfo g_jamTypeUInt16 = {
    1,
    JamUInt16_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamUInt16_SetString,
    JamUInt16_SetInt32,
    JamUInt16_SetUInt32,
    JamUInt16_SetInt64,
    JamUInt16_SetUInt64,
    JamUInt16_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamUInt16_GetInt32,
    JamUInt16_GetUInt32,
    JamUInt16_GetInt64,
    JamUInt16_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C461A0
const JamTypeInfo g_jamTypeInt32 = {
    1,
    JamInt32_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamInt32_SetString,
    JamInt32_SetInt32,
    JamInt32_SetUInt32,
    JamInt32_SetInt64,
    JamInt32_SetUInt64,
    JamInt32_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamInt32_GetInt32,
    JamInt32_GetUInt32,
    JamInt32_GetInt64,
    JamInt32_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C461F8
const JamTypeInfo g_jamTypeUInt32 = {
    1,
    JamUInt32_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamUInt32_SetString,
    JamUInt32_SetInt32,
    JamUInt32_SetUInt32,
    JamUInt32_SetInt64,
    JamUInt32_SetUInt64,
    JamUInt32_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamUInt32_GetInt32,
    JamUInt32_GetUInt32,
    JamUInt32_GetInt64,
    JamUInt32_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46250
const JamTypeInfo g_jamTypeInt64 = {
    1,
    JamInt64_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamInt64_SetString,
    JamInt64_SetInt32,
    JamInt64_SetUInt32,
    JamInt64_SetInt64,
    JamInt64_SetUInt64,
    JamInt64_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamInt64_GetInt32,
    JamInt64_GetUInt32,
    JamInt64_GetInt64,
    JamInt64_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C462A8
const JamTypeInfo g_jamTypeUInt64 = {
    1,
    JamUInt64_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamUInt64_SetString,
    JamUInt64_SetInt32,
    JamUInt64_SetUInt32,
    JamUInt64_SetInt64,
    JamUInt64_SetUInt64,
    JamUInt64_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamUInt64_GetInt32,
    JamUInt64_GetUInt32,
    JamUInt64_GetInt64,
    JamUInt64_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46300
const JamTypeInfo g_jamTypeLong = {
    1,
    JamInt32_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamInt32_SetString,
    JamInt32_SetInt32,
    JamInt32_SetUInt32,
    JamInt32_SetInt64,
    JamInt32_SetUInt64,
    JamInt32_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamInt32_GetInt32,
    JamInt32_GetUInt32,
    JamInt32_GetInt64,
    JamInt32_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46358
const JamTypeInfo g_jamTypeFloat = {
    1,
    JamFloat_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamFloat_SetString,
    JamFloat_SetInt32,
    JamFloat_SetUInt32,
    JamFloat_SetInt64,
    JamFloat_SetUInt64,
    JamFloat_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamFloat_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C463B0
const JamTypeInfo g_jamTypeC2iVector = {
    2,
    JamDefault_SetDefault,
    JamC2iVector_GetMember,
    JamC2iVector_GetMemberName,
    JamC2iVector_FindMember,
    JamDefault_GetMemberCount2,
    JamDefault_GetMemberCapacity2,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46408
const JamTypeInfo g_jamTypeC3iVector = {
    2,
    JamDefault_SetDefault,
    JamC3iVector_GetMember,
    JamC3iVector_GetMemberName,
    JamC3iVector_FindMember,
    JamDefault_GetMemberCount3,
    JamDefault_GetMemberCapacity3,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46460
const JamTypeInfo g_jamTypeC4iVector = {
    2,
    JamDefault_SetDefault,
    JamC4iVector_GetMember,
    JamC4iVector_GetMemberName,
    JamC4iVector_FindMember,
    JamDefault_GetMemberCount4,
    JamDefault_GetMemberCapacity4,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C464B8
const JamTypeInfo g_jamTypeC2Vector = {
    2,
    JamDefault_SetDefault,
    JamC2Vector_GetMember,
    JamC2Vector_GetMemberName,
    JamC2Vector_FindMember,
    JamDefault_GetMemberCount2,
    JamDefault_GetMemberCapacity2,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46510
const JamTypeInfo g_jamTypeC3Vector = {
    2,
    JamDefault_SetDefault,
    JamC3Vector_GetMember,
    JamC3Vector_GetMemberName,
    JamC3Vector_FindMember,
    JamDefault_GetMemberCount3,
    JamDefault_GetMemberCapacity3,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46568
const JamTypeInfo g_jamTypeC4Vector = {
    2,
    JamDefault_SetDefault,
    JamC4Vector_GetMember,
    JamC4Vector_GetMemberName,
    JamC4Vector_FindMember,
    JamDefault_GetMemberCount4,
    JamDefault_GetMemberCapacity4,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C465C0
const JamTypeInfo g_jamTypeString = {
    1,
    JamString_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamString_SetString,
    JamString_SetInt32,
    JamString_SetUInt32,
    JamString_SetInt64,
    JamString_SetUInt64,
    JamString_SetFloat,
    JamDefault_SetBool,
    JamString_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46618
const JamTypeInfo g_jamTypeVoid = {
    0,
    JamDefault_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C46670
const JamTypeInfo g_jamTypeRealmAddress = {
    2,
    JamDefault_SetDefault,
    JamRealmAddress_GetMember,
    JamRealmAddress_GetMemberName,
    JamRealmAddress_FindMember,
    JamDefault_GetMemberCount3,
    JamDefault_GetMemberCapacity3,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00C466C8
const JamTypeInfo g_jamTypeBool = {
    1,
    JamBool_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamDefault_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamBool_SetBool,
    JamDefault_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamBool_GetBool
};

// STATIC: 0x00BBC640
const JamTypeInfo g_jamTypeComplaintType = {
    1,
    JamComplaintType_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamComplaintType_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamComplaintType_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00BC14C8
const JamTypeInfo g_jamTypeSplineFace = {
    1,
    JamSplineFace_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamSplineFace_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamSplineFace_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00BC1520
const JamTypeInfo g_jamTypeUpdateCollisionHeightReason = {
    1,
    JamUpdateCollisionHeightReason_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamUpdateCollisionHeightReason_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamUpdateCollisionHeightReason_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// STATIC: 0x00BC1A58
const JamTypeInfo g_jamTypeTradeStatus = {
    1,
    JamTradeStatus_SetDefault,
    JamDefault_GetMember,
    JamDefault_GetMemberName,
    JamDefault_FindMember,
    JamDefault_GetMemberCount,
    JamDefault_GetMemberCapacity,
    JamDefault_GetElement,
    JamTradeStatus_SetString,
    JamDefault_SetInt32,
    JamDefault_SetUInt32,
    JamDefault_SetInt64,
    JamDefault_SetUInt64,
    JamDefault_SetFloat,
    JamDefault_SetBool,
    JamTradeStatus_GetString,
    JamDefault_GetInt32,
    JamDefault_GetUInt32,
    JamDefault_GetInt64,
    JamDefault_GetUInt64,
    JamDefault_GetFloat,
    JamDefault_GetBool
};

// OFFSET: 0x00B075A0
static bool JamFloat_FromUInt64(float* data, uint64_t value) {
    float converted = static_cast<float>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *data = converted;
    return true;
}

// OFFSET: 0x00A3AA30
static bool JamDefault_SetDefaultTrue(void* data) {
    return true;
}

// OFFSET: 0x00B06D30
static JamValue* JamDefault_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeNone;
    out->m_data = NULL;
    out->m_meta = NULL;
    return out;
}

// OFFSET: 0x00842700
static const char* JamDefault_GetMemberName(void* data, const void* meta, uint32_t index) {
    return NULL;
}

// OFFSET: 0x00B06D30
static JamValue* JamDefault_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    out->m_type = &g_jamTypeNone;
    out->m_data = NULL;
    out->m_meta = NULL;
    return out;
}

// OFFSET: 0x00842700
static uint32_t JamDefault_GetMemberCount(void* data, const void* meta) {
    return 0;
}

// OFFSET: 0x00842700
static uint32_t JamDefault_GetMemberCapacity(void* data, const void* meta) {
    return 0;
}

// OFFSET: 0x00B06D30
static JamValue* JamDefault_GetElement(JamValue* out) {
    out->m_type = &g_jamTypeNone;
    out->m_data = NULL;
    out->m_meta = NULL;
    return out;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetString(void* data, const void* meta, const char* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetInt32(void* data, const void* meta, int32_t value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetUInt32(void* data, const void* meta, uint32_t value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetInt64(void* data, const void* meta, int64_t value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetUInt64(void* data, const void* meta, uint64_t value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetFloat(void* data, const void* meta, float value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetBool(void* data, const void* meta, bool value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetString(const void* data, const void* meta, char* buffer, uint32_t size) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetInt32(const void* data, const void* meta, int32_t* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetInt64(const void* data, const void* meta, int64_t* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetFloat(const void* data, const void* meta, float* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_GetBool(const void* data, const void* meta, bool* value) {
    return false;
}

// OFFSET: 0x00B06D20
static bool JamDefault_SetDefault(void* data) {
    return false;
}

// OFFSET: 0x00B06D80
static JamValue* JamStruct_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    const JamStructMeta* info = static_cast<const JamStructMeta*>(meta);
    out->m_type = info->m_types[index];
    out->m_data = static_cast<uint8_t*>(data) + info->m_offsets[index];
    out->m_meta = info->m_metas[index];
    return out;
}

// OFFSET: 0x00B06D50
static const char* JamStruct_GetMemberName(void* data, const void* meta, uint32_t index) {
    return static_cast<const JamStructMeta*>(meta)->m_names[index];
}

// OFFSET: 0x00B06DB0
static JamValue* JamStruct_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    const JamStructMeta* info = static_cast<const JamStructMeta*>(meta);

    if (!info->m_count) {
        out->m_data = NULL;
        out->m_meta = NULL;
        out->m_type = &g_jamTypeNone;
        return out;
    }

    uint32_t hash = info->m_seed;

    for (const uint8_t* c = reinterpret_cast<const uint8_t*>(name); *c; c++) {
        hash = *c ^ (0x1000193 * hash);
    }

    const uint16_t* bucket = &info->m_buckets[(info->m_bucketCount - 1) & ((hash >> 24) ^ (hash & 0xFF))];
    uint32_t end = bucket[1];

    for (uint32_t i = bucket[0]; i < end; i++) {
        if (info->m_hashes[i] == hash) {
            if (!strcmp(name, info->m_names[i])) {
                out->m_type = info->m_types[i];
                out->m_data = static_cast<uint8_t*>(data) + info->m_offsets[i];
                out->m_meta = info->m_metas[i];
                return out;
            }

            out->m_data = NULL;
            out->m_meta = NULL;
            out->m_type = &g_jamTypeNone;
            return out;
        }
    }

    out->m_data = NULL;
    out->m_meta = NULL;
    out->m_type = &g_jamTypeNone;
    return out;
}

// OFFSET: 0x00B06D70
static uint32_t JamStruct_GetMemberCount(void* data, const void* meta) {
    return static_cast<const JamStructMeta*>(meta)->m_count;
}

// OFFSET: 0x00B06D70
static uint32_t JamStruct_GetMemberCapacity(void* data, const void* meta) {
    return static_cast<const JamStructMeta*>(meta)->m_count;
}

// OFFSET: 0x00B071D0
static bool JamInt8_SetDefault(void* data) {
    *static_cast<int8_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B07600
static bool JamInt8_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    int8_t converted = static_cast<int8_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<int8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07630
static bool JamInt8_SetInt32(void* data, const void* meta, int32_t value) {
    int8_t converted = static_cast<int8_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<int8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07630
static bool JamInt8_SetUInt32(void* data, const void* meta, uint32_t value) {
    int8_t converted = static_cast<int8_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<int8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07650
static bool JamInt8_SetInt64(void* data, const void* meta, int64_t value) {
    int8_t converted = static_cast<int8_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<int8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07650
static bool JamInt8_SetUInt64(void* data, const void* meta, uint64_t value) {
    int8_t converted = static_cast<int8_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<int8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07670
static bool JamInt8_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    int8_t converted = static_cast<int8_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<int8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07690
static bool JamInt8_GetInt32(const void* data, const void* meta, int32_t* value) {
    int8_t current = *static_cast<const int8_t*>(data);
    *value = current;
    return static_cast<int8_t>(*value) == current;
}

// OFFSET: 0x00B07690
static bool JamInt8_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    int8_t current = *static_cast<const int8_t*>(data);
    *value = current;
    return static_cast<int8_t>(*value) == current;
}

// OFFSET: 0x00B076B0
static bool JamInt8_GetInt64(const void* data, const void* meta, int64_t* value) {
    int8_t current = *static_cast<const int8_t*>(data);
    *value = current;
    return static_cast<int8_t>(*value) == current;
}

// OFFSET: 0x00B076B0
static bool JamInt8_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    int8_t current = *static_cast<const int8_t*>(data);
    *value = current;
    return static_cast<int8_t>(*value) == current;
}

// OFFSET: 0x00B071D0
static bool JamUInt8_SetDefault(void* data) {
    *static_cast<uint8_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B076D0
static bool JamUInt8_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    uint8_t converted = static_cast<uint8_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<uint8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07700
static bool JamUInt8_SetInt32(void* data, const void* meta, int32_t value) {
    uint8_t converted = static_cast<uint8_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07700
static bool JamUInt8_SetUInt32(void* data, const void* meta, uint32_t value) {
    uint8_t converted = static_cast<uint8_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07720
static bool JamUInt8_SetInt64(void* data, const void* meta, int64_t value) {
    uint8_t converted = static_cast<uint8_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07720
static bool JamUInt8_SetUInt64(void* data, const void* meta, uint64_t value) {
    uint8_t converted = static_cast<uint8_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07740
static bool JamUInt8_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    uint8_t converted = static_cast<uint8_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<uint8_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07760
static bool JamUInt8_GetInt32(const void* data, const void* meta, int32_t* value) {
    uint8_t current = *static_cast<const uint8_t*>(data);
    *value = current;
    return static_cast<uint8_t>(*value) == current;
}

// OFFSET: 0x00B07760
static bool JamUInt8_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    uint8_t current = *static_cast<const uint8_t*>(data);
    *value = current;
    return static_cast<uint8_t>(*value) == current;
}

// OFFSET: 0x00B07780
static bool JamUInt8_GetInt64(const void* data, const void* meta, int64_t* value) {
    uint8_t current = *static_cast<const uint8_t*>(data);
    *value = current;
    return static_cast<uint8_t>(*value) == current;
}

// OFFSET: 0x00B07780
static bool JamUInt8_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    uint8_t current = *static_cast<const uint8_t*>(data);
    *value = current;
    return static_cast<uint8_t>(*value) == current;
}

// OFFSET: 0x00B07890
static bool JamInt16_SetDefault(void* data) {
    *static_cast<int16_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B077A0
static bool JamInt16_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    int16_t converted = static_cast<int16_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<int16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B077D0
static bool JamInt16_SetInt32(void* data, const void* meta, int32_t value) {
    int16_t converted = static_cast<int16_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<int16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B077D0
static bool JamInt16_SetUInt32(void* data, const void* meta, uint32_t value) {
    int16_t converted = static_cast<int16_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<int16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B077F0
static bool JamInt16_SetInt64(void* data, const void* meta, int64_t value) {
    int16_t converted = static_cast<int16_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<int16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B077F0
static bool JamInt16_SetUInt64(void* data, const void* meta, uint64_t value) {
    int16_t converted = static_cast<int16_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<int16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07820
static bool JamInt16_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    int16_t converted = static_cast<int16_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<int16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07840
static bool JamInt16_GetInt32(const void* data, const void* meta, int32_t* value) {
    int16_t current = *static_cast<const int16_t*>(data);
    *value = current;
    return static_cast<int16_t>(*value) == current;
}

// OFFSET: 0x00B07840
static bool JamInt16_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    int16_t current = *static_cast<const int16_t*>(data);
    *value = current;
    return static_cast<int16_t>(*value) == current;
}

// OFFSET: 0x00B07860
static bool JamInt16_GetInt64(const void* data, const void* meta, int64_t* value) {
    int16_t current = *static_cast<const int16_t*>(data);
    *value = current;
    return static_cast<int16_t>(*value) == current;
}

// OFFSET: 0x00B07860
static bool JamInt16_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    int16_t current = *static_cast<const int16_t*>(data);
    *value = current;
    return static_cast<int16_t>(*value) == current;
}

// OFFSET: 0x00B07890
static bool JamUInt16_SetDefault(void* data) {
    *static_cast<uint16_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B078A0
static bool JamUInt16_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    uint16_t converted = static_cast<uint16_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<uint16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B078D0
static bool JamUInt16_SetInt32(void* data, const void* meta, int32_t value) {
    uint16_t converted = static_cast<uint16_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B078D0
static bool JamUInt16_SetUInt32(void* data, const void* meta, uint32_t value) {
    uint16_t converted = static_cast<uint16_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B078F0
static bool JamUInt16_SetInt64(void* data, const void* meta, int64_t value) {
    uint16_t converted = static_cast<uint16_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B078F0
static bool JamUInt16_SetUInt64(void* data, const void* meta, uint64_t value) {
    uint16_t converted = static_cast<uint16_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07920
static bool JamUInt16_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    uint16_t converted = static_cast<uint16_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<uint16_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07940
static bool JamUInt16_GetInt32(const void* data, const void* meta, int32_t* value) {
    uint16_t current = *static_cast<const uint16_t*>(data);
    *value = current;
    return static_cast<uint16_t>(*value) == current;
}

// OFFSET: 0x00B07940
static bool JamUInt16_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    uint16_t current = *static_cast<const uint16_t*>(data);
    *value = current;
    return static_cast<uint16_t>(*value) == current;
}

// OFFSET: 0x00B07960
static bool JamUInt16_GetInt64(const void* data, const void* meta, int64_t* value) {
    uint16_t current = *static_cast<const uint16_t*>(data);
    *value = current;
    return static_cast<uint16_t>(*value) == current;
}

// OFFSET: 0x00B07960
static bool JamUInt16_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    uint16_t current = *static_cast<const uint16_t*>(data);
    *value = current;
    return static_cast<uint16_t>(*value) == current;
}

// OFFSET: 0x00B07C00
static bool JamInt32_SetDefault(void* data) {
    *static_cast<int32_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B07990
static bool JamInt32_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    int32_t converted = static_cast<int32_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<int32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BD0
static bool JamInt32_SetInt32(void* data, const void* meta, int32_t value) {
    int32_t converted = static_cast<int32_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<int32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BD0
static bool JamInt32_SetUInt32(void* data, const void* meta, uint32_t value) {
    int32_t converted = static_cast<int32_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<int32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BE0
static bool JamInt32_SetInt64(void* data, const void* meta, int64_t value) {
    int32_t converted = static_cast<int32_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<int32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BE0
static bool JamInt32_SetUInt64(void* data, const void* meta, uint64_t value) {
    int32_t converted = static_cast<int32_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<int32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07A50
static bool JamInt32_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    int32_t converted = static_cast<int32_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<int32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B079C0
static bool JamInt32_GetInt32(const void* data, const void* meta, int32_t* value) {
    int32_t current = *static_cast<const int32_t*>(data);
    *value = current;
    return static_cast<int32_t>(*value) == current;
}

// OFFSET: 0x00B079C0
static bool JamInt32_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    int32_t current = *static_cast<const int32_t*>(data);
    *value = current;
    return static_cast<int32_t>(*value) == current;
}

// OFFSET: 0x00B079E0
static bool JamInt32_GetInt64(const void* data, const void* meta, int64_t* value) {
    int32_t current = *static_cast<const int32_t*>(data);
    *value = current;
    return static_cast<int32_t>(*value) == current;
}

// OFFSET: 0x00B079E0
static bool JamInt32_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    int32_t current = *static_cast<const int32_t*>(data);
    *value = current;
    return static_cast<int32_t>(*value) == current;
}

// OFFSET: 0x00B07C00
static bool JamUInt32_SetDefault(void* data) {
    *static_cast<uint32_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B07A00
static bool JamUInt32_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    uint32_t converted = static_cast<uint32_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<uint32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BD0
static bool JamUInt32_SetInt32(void* data, const void* meta, int32_t value) {
    uint32_t converted = static_cast<uint32_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BD0
static bool JamUInt32_SetUInt32(void* data, const void* meta, uint32_t value) {
    uint32_t converted = static_cast<uint32_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07A30
static bool JamUInt32_SetInt64(void* data, const void* meta, int64_t value) {
    uint32_t converted = static_cast<uint32_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07A30
static bool JamUInt32_SetUInt64(void* data, const void* meta, uint64_t value) {
    uint32_t converted = static_cast<uint32_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07A50
static bool JamUInt32_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    uint32_t converted = static_cast<uint32_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<uint32_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B079C0
static bool JamUInt32_GetInt32(const void* data, const void* meta, int32_t* value) {
    uint32_t current = *static_cast<const uint32_t*>(data);
    *value = current;
    return static_cast<uint32_t>(*value) == current;
}

// OFFSET: 0x00B079C0
static bool JamUInt32_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    uint32_t current = *static_cast<const uint32_t*>(data);
    *value = current;
    return static_cast<uint32_t>(*value) == current;
}

// OFFSET: 0x00B07A70
static bool JamUInt32_GetInt64(const void* data, const void* meta, int64_t* value) {
    uint32_t current = *static_cast<const uint32_t*>(data);
    *value = current;
    return static_cast<uint32_t>(*value) == current;
}

// OFFSET: 0x00B07A70
static bool JamUInt32_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    uint32_t current = *static_cast<const uint32_t*>(data);
    *value = current;
    return static_cast<uint32_t>(*value) == current;
}

// OFFSET: 0x00B07B90
static bool JamInt64_SetDefault(void* data) {
    *static_cast<int64_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B07A90
static bool JamInt64_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    int64_t converted = static_cast<int64_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<int64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AB0
static bool JamInt64_SetInt32(void* data, const void* meta, int32_t value) {
    int64_t converted = static_cast<int64_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<int64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AD0
static bool JamInt64_SetUInt32(void* data, const void* meta, uint32_t value) {
    int64_t converted = static_cast<int64_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<int64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AF0
static bool JamInt64_SetInt64(void* data, const void* meta, int64_t value) {
    int64_t converted = static_cast<int64_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<int64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AF0
static bool JamInt64_SetUInt64(void* data, const void* meta, uint64_t value) {
    int64_t converted = static_cast<int64_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<int64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BB0
static bool JamInt64_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    int64_t converted = static_cast<int64_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<int64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07B10
static bool JamInt64_GetInt32(const void* data, const void* meta, int32_t* value) {
    int64_t current = *static_cast<const int64_t*>(data);
    *value = current;
    return static_cast<int64_t>(*value) == current;
}

// OFFSET: 0x00B07B40
static bool JamInt64_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    int64_t current = *static_cast<const int64_t*>(data);
    *value = current;
    return static_cast<int64_t>(*value) == current;
}

// OFFSET: 0x00B07B70
static bool JamInt64_GetInt64(const void* data, const void* meta, int64_t* value) {
    int64_t current = *static_cast<const int64_t*>(data);
    *value = current;
    return static_cast<int64_t>(*value) == current;
}

// OFFSET: 0x00B07B70
static bool JamInt64_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    int64_t current = *static_cast<const int64_t*>(data);
    *value = current;
    return static_cast<int64_t>(*value) == current;
}

// OFFSET: 0x00B07B90
static bool JamUInt64_SetDefault(void* data) {
    *static_cast<uint64_t*>(data) = 0;
    return true;
}

// OFFSET: 0x00B07A90
static bool JamUInt64_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    uint64_t converted = static_cast<uint64_t>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<uint64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AB0
static bool JamUInt64_SetInt32(void* data, const void* meta, int32_t value) {
    uint64_t converted = static_cast<uint64_t>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AD0
static bool JamUInt64_SetUInt32(void* data, const void* meta, uint32_t value) {
    uint64_t converted = static_cast<uint64_t>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<uint64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AF0
static bool JamUInt64_SetInt64(void* data, const void* meta, int64_t value) {
    uint64_t converted = static_cast<uint64_t>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07AF0
static bool JamUInt64_SetUInt64(void* data, const void* meta, uint64_t value) {
    uint64_t converted = static_cast<uint64_t>(value);

    if (static_cast<uint64_t>(converted) != value) {
        return false;
    }

    *static_cast<uint64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07BB0
static bool JamUInt64_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    uint64_t converted = static_cast<uint64_t>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<uint64_t*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07B10
static bool JamUInt64_GetInt32(const void* data, const void* meta, int32_t* value) {
    uint64_t current = *static_cast<const uint64_t*>(data);
    *value = current;
    return static_cast<uint64_t>(*value) == current;
}

// OFFSET: 0x00B07B40
static bool JamUInt64_GetUInt32(const void* data, const void* meta, uint32_t* value) {
    uint64_t current = *static_cast<const uint64_t*>(data);
    *value = current;
    return static_cast<uint64_t>(*value) == current;
}

// OFFSET: 0x00B07B70
static bool JamUInt64_GetInt64(const void* data, const void* meta, int64_t* value) {
    uint64_t current = *static_cast<const uint64_t*>(data);
    *value = current;
    return static_cast<uint64_t>(*value) == current;
}

// OFFSET: 0x00B07B70
static bool JamUInt64_GetUInt64(const void* data, const void* meta, uint64_t* value) {
    uint64_t current = *static_cast<const uint64_t*>(data);
    *value = current;
    return static_cast<uint64_t>(*value) == current;
}

// OFFSET: 0x00B07D80
static bool JamFloat_SetDefault(void* data) {
    *static_cast<float*>(data) = 0;
    return true;
}

// OFFSET: 0x00B07C10
static bool JamFloat_SetString(void* data, const void* meta, const char* value) {
    int64_t number = SStrToInt64(value);
    float converted = static_cast<float>(number);

    if (static_cast<int64_t>(converted) != number) {
        return false;
    }

    *static_cast<float*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07C60
static bool JamFloat_SetInt32(void* data, const void* meta, int32_t value) {
    float converted = static_cast<float>(value);

    if (static_cast<int32_t>(converted) != value) {
        return false;
    }

    *static_cast<float*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07C90
static bool JamFloat_SetUInt32(void* data, const void* meta, uint32_t value) {
    float converted = static_cast<float>(value);

    if (static_cast<uint32_t>(converted) != value) {
        return false;
    }

    *static_cast<float*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07CF0
static bool JamFloat_SetInt64(void* data, const void* meta, int64_t value) {
    float converted = static_cast<float>(value);

    if (static_cast<int64_t>(converted) != value) {
        return false;
    }

    *static_cast<float*>(data) = converted;
    return true;
}

// OFFSET: 0x00B07D30
static bool JamFloat_SetUInt64(void* data, const void* meta, uint64_t value) {
    return JamFloat_FromUInt64(static_cast<float*>(data), value);
}

// OFFSET: 0x00B07D50
static bool JamFloat_SetFloat(void* data, const void* meta, float value) {
    int32_t whole = static_cast<int32_t>(value);
    float converted = static_cast<float>(whole);

    if (static_cast<int32_t>(converted) != whole) {
        return false;
    }

    *static_cast<float*>(data) = converted;
    return true;
}

// OFFSET: 0x00B06EC0
static bool JamFloat_GetFloat(const void* data, const void* meta, float* value) {
    *value = *static_cast<const float*>(data);
    return true;
}

// OFFSET: 0x00B07210
static JamValue* JamC2iVector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B072B0
static const char* JamC2iVector_GetMemberName(void* data, const void* meta, uint32_t index) {
    return s_c2iVectorNames[index];
}

// OFFSET: 0x00B07240
static JamValue* JamC2iVector_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    uint32_t index = 0;

    if (!name[0] || name[1]) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    switch (name[0]) {
        case 'x':
            break;

        case 'y':
            index = 1;
            break;

        case 'z':
            index = 2;
            break;

        case 'w':
            index = 3;
            break;

        default:
            out->m_data = NULL;
            out->m_type = &g_jamTypeNone;
            out->m_meta = NULL;
            return out;
    }

    if (index >= 2) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    out->m_type = &g_jamTypeInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B2CAB0
static uint32_t JamDefault_GetMemberCount2(void* data, const void* meta) {
    return 2;
}

// OFFSET: 0x00B2CAB0
static uint32_t JamDefault_GetMemberCapacity2(void* data, const void* meta) {
    return 2;
}

// OFFSET: 0x00B07210
static JamValue* JamC3iVector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07340
static const char* JamC3iVector_GetMemberName(void* data, const void* meta, uint32_t index) {
    return s_c3iVectorNames[index];
}

// OFFSET: 0x00B072C0
static JamValue* JamC3iVector_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    uint32_t index = 0;

    if (!name[0] || name[1]) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    switch (name[0]) {
        case 'x':
            break;

        case 'y':
            index = 1;
            break;

        case 'z':
            index = 2;
            break;

        case 'w':
            index = 3;
            break;

        default:
            out->m_data = NULL;
            out->m_type = &g_jamTypeNone;
            out->m_meta = NULL;
            return out;
    }

    if (index >= 3) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    out->m_type = &g_jamTypeInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07330
static uint32_t JamDefault_GetMemberCount3(void* data, const void* meta) {
    return 3;
}

// OFFSET: 0x00B07330
static uint32_t JamDefault_GetMemberCapacity3(void* data, const void* meta) {
    return 3;
}

// OFFSET: 0x00B07210
static JamValue* JamC4iVector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B073D0
static const char* JamC4iVector_GetMemberName(void* data, const void* meta, uint32_t index) {
    return s_c4iVectorNames[index];
}

// OFFSET: 0x00B07350
static JamValue* JamC4iVector_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    uint32_t index = 0;

    if (!name[0] || name[1]) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    switch (name[0]) {
        case 'x':
            break;

        case 'y':
            index = 1;
            break;

        case 'z':
            index = 2;
            break;

        case 'w':
            index = 3;
            break;

        default:
            out->m_data = NULL;
            out->m_type = &g_jamTypeNone;
            out->m_meta = NULL;
            return out;
    }

    if (index >= 4) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    out->m_type = &g_jamTypeInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B2CAA0
static uint32_t JamDefault_GetMemberCount4(void* data, const void* meta) {
    return 4;
}

// OFFSET: 0x00B2CAA0
static uint32_t JamDefault_GetMemberCapacity4(void* data, const void* meta) {
    return 4;
}

// OFFSET: 0x00B07460
static JamValue* JamC2Vector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeFloat;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07450
static const char* JamC2Vector_GetMemberName(void* data, const void* meta, uint32_t index) {
    return s_c2VectorNames[index];
}

// OFFSET: 0x00B073E0
static JamValue* JamC2Vector_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    uint32_t index = 0;

    if (!name[0] || name[1]) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    switch (name[0]) {
        case 'x':
            break;

        case 'y':
            index = 1;
            break;

        case 'z':
            index = 2;
            break;

        case 'w':
            index = 3;
            break;

        default:
            out->m_data = NULL;
            out->m_type = &g_jamTypeNone;
            out->m_meta = NULL;
            return out;
    }

    if (index >= 2) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    out->m_type = &g_jamTypeFloat;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07460
static JamValue* JamC3Vector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeFloat;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07500
static const char* JamC3Vector_GetMemberName(void* data, const void* meta, uint32_t index) {
    return s_c3VectorNames[index];
}

// OFFSET: 0x00B07490
static JamValue* JamC3Vector_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    uint32_t index = 0;

    if (!name[0] || name[1]) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    switch (name[0]) {
        case 'x':
            break;

        case 'y':
            index = 1;
            break;

        case 'z':
            index = 2;
            break;

        case 'w':
            index = 3;
            break;

        default:
            out->m_data = NULL;
            out->m_type = &g_jamTypeNone;
            out->m_meta = NULL;
            return out;
    }

    if (index >= 3) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    out->m_type = &g_jamTypeFloat;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07460
static JamValue* JamC4Vector_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    out->m_type = &g_jamTypeFloat;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B07590
static const char* JamC4Vector_GetMemberName(void* data, const void* meta, uint32_t index) {
    return s_c4VectorNames[index];
}

// OFFSET: 0x00B07510
static JamValue* JamC4Vector_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    uint32_t index = 0;

    if (!name[0] || name[1]) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    switch (name[0]) {
        case 'x':
            break;

        case 'y':
            index = 1;
            break;

        case 'z':
            index = 2;
            break;

        case 'w':
            index = 3;
            break;

        default:
            out->m_data = NULL;
            out->m_type = &g_jamTypeNone;
            out->m_meta = NULL;
            return out;
    }

    if (index >= 4) {
        out->m_data = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_meta = NULL;
        return out;
    }

    out->m_type = &g_jamTypeFloat;
    out->m_data = static_cast<uint32_t*>(data) + index;
    out->m_meta = &s_vectorMeta[index];
    return out;
}

// OFFSET: 0x00B071D0
static bool JamString_SetDefault(void* data) {
    *static_cast<char*>(data) = 0;
    return true;
}

// OFFSET: 0x00B06EE0
static bool JamString_SetString(void* data, const void* meta, const char* value) {
    SStrCopy(static_cast<char*>(data), value, reinterpret_cast<size_t>(meta));
    return true;
}

// OFFSET: 0x00B06F30
static bool JamString_SetInt32(void* data, const void* meta, int32_t value) {
    SStrPrintf(static_cast<char*>(data), reinterpret_cast<size_t>(meta), "%d", value);
    return true;
}

// OFFSET: 0x00B06F50
static bool JamString_SetUInt32(void* data, const void* meta, uint32_t value) {
    SStrPrintf(static_cast<char*>(data), reinterpret_cast<size_t>(meta), "%u", value);
    return true;
}

// OFFSET: 0x00B06F70
static bool JamString_SetInt64(void* data, const void* meta, int64_t value) {
    SStrPrintf(static_cast<char*>(data), reinterpret_cast<size_t>(meta), "%lld", value);
    return true;
}

// OFFSET: 0x00B06FA0
static bool JamString_SetUInt64(void* data, const void* meta, uint64_t value) {
    SStrPrintf(static_cast<char*>(data), reinterpret_cast<size_t>(meta), "%llu", value);
    return true;
}

// OFFSET: 0x00B06FD0
static bool JamString_SetFloat(void* data, const void* meta, float value) {
    SStrPrintf(static_cast<char*>(data), reinterpret_cast<size_t>(meta), "%f", value);
    return true;
}

// OFFSET: 0x00B07000
static bool JamString_GetString(const void* data, const void* meta, char* buffer, uint32_t size) {
    SStrCopy(buffer, static_cast<const char*>(data), size);
    return true;
}

// OFFSET: 0x00B07050
static JamValue* JamRealmAddress_GetMember(JamValue* out, void* data, const void* meta, uint32_t index) {
    if (index > 2) {
        out->m_meta = NULL;
        out->m_type = &g_jamTypeNone;
        out->m_data = NULL;
        return out;
    }

    out->m_meta = NULL;
    out->m_type = &g_jamTypeUInt32;
    out->m_data = static_cast<uint32_t*>(data) + index;
    return out;
}

// OFFSET: 0x00B070C0
static const char* JamRealmAddress_GetMemberName(void* data, const void* meta, uint32_t index) {
    switch (index) {
        case 0:
            return "realm";

        case 1:
            return "type";

        case 2:
            return "server";
    }

    return NULL;
}

// OFFSET: 0x00B070F0
static JamValue* JamRealmAddress_FindMember(JamValue* out, void* data, const void* meta, const char* name) {
    for (uint32_t i = 0; i < 3; i++) {
        if (!strcmp(name, JamRealmAddress_GetMemberName(data, meta, i))) {
            return JamRealmAddress_GetMember(out, data, meta, i);
        }
    }

    out->m_meta = NULL;
    out->m_data = NULL;
    out->m_type = &g_jamTypeNone;
    return out;
}

// OFFSET: 0x00B071D0
static bool JamBool_SetDefault(void* data) {
    *static_cast<bool*>(data) = false;
    return true;
}

// OFFSET: 0x00B071E0
static bool JamBool_SetBool(void* data, const void* meta, bool value) {
    *static_cast<bool*>(data) = value;
    return true;
}

// OFFSET: 0x00B071F0
static bool JamBool_GetBool(const void* data, const void* meta, bool* value) {
    *value = *static_cast<const bool*>(data);
    return true;
}

// OFFSET: 0x0066B180
static bool JamComplaintType_SetDefault(void* data) {
    *static_cast<uint32_t*>(data) = 7;
    return true;
}

// OFFSET: 0x0066B190
static bool JamComplaintType_SetString(void* data, const void* meta, const char* value) {
    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_COMPLAINT_ANY", value)) {
        *static_cast<uint32_t*>(data) = 7;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_HARASSMENT", value)) {
        *static_cast<uint32_t*>(data) = 4;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_BATTLETAG", value)) {
        *static_cast<uint32_t*>(data) = 15;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_NAME", value)) {
        *static_cast<uint32_t*>(data) = 3;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_IS_HACKER", value)) {
        *static_cast<uint32_t*>(data) = 1;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_MAP_COMPLAINT_ANY", value)) {
        *static_cast<uint32_t*>(data) = 12;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_MAP_INAPPROPRIATE_CONTENT", value)) {
        *static_cast<uint32_t*>(data) = 5;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_MAP_INAPPROPRIATE_NAME", value)) {
        *static_cast<uint32_t*>(data) = 13;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_REAL_LIFE_THREAT", value)) {
        *static_cast<uint32_t*>(data) = 11;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_SPAM", value)) {
        *static_cast<uint32_t*>(data) = 0;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_TOON_COMPLAINT_ANY", value)) {
        *static_cast<uint32_t*>(data) = 2;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_LANGUAGE", value)) {
        *static_cast<uint32_t*>(data) = 10;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_GUILD_NAME", value)) {
        *static_cast<uint32_t*>(data) = 6;
        return true;
    }

    if (!strcmp("JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_ARENA_TEAM_NAME", value)) {
        *static_cast<uint32_t*>(data) = 9;
        return true;
    }

    return false;
}

// OFFSET: 0x0066B520
static bool JamComplaintType_GetString(const void* data, const void* meta, char* buffer, uint32_t size) {
    switch (*static_cast<const uint32_t*>(data)) {
        case 0:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_SPAM", size);
            return true;

        case 1:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_IS_HACKER", size);
            return true;

        case 2:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_TOON_COMPLAINT_ANY", size);
            return true;

        case 3:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_NAME", size);
            return true;

        case 4:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_HARASSMENT", size);
            return true;

        case 5:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_MAP_INAPPROPRIATE_CONTENT", size);
            return true;

        case 6:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_GUILD_NAME", size);
            return true;

        case 7:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_COMPLAINT_ANY", size);
            return true;

        case 9:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_ARENA_TEAM_NAME", size);
            return true;

        case 10:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_LANGUAGE", size);
            return true;

        case 11:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_REAL_LIFE_THREAT", size);
            return true;

        case 12:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_MAP_COMPLAINT_ANY", size);
            return true;

        case 13:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_MAP_INAPPROPRIATE_NAME", size);
            return true;

        case 15:
            SStrCopy(buffer, "JAM_CLI_COMPLAINT_TYPE_INAPPROPRIATE_BATTLETAG", size);
            return true;

        default:
            return false;
    }
}

// OFFSET: 0x00696570
static bool JamSplineFace_SetDefault(void* data) {
    *static_cast<uint32_t*>(data) = 3;
    return true;
}

// OFFSET: 0x00696580
static bool JamSplineFace_SetString(void* data, const void* meta, const char* value) {
    if (!strcmp("SPLINE_FACE_NOTHING", value)) {
        *static_cast<uint32_t*>(data) = 3;
        return true;
    }

    if (!strcmp("SPLINE_FACE_DIRECTION", value)) {
        *static_cast<uint32_t*>(data) = 0;
        return true;
    }

    if (!strcmp("SPLINE_FACE_GUID", value)) {
        *static_cast<uint32_t*>(data) = 2;
        return true;
    }

    if (!strcmp("SPLINE_FACE_SPOT", value)) {
        *static_cast<uint32_t*>(data) = 1;
        return true;
    }

    return false;
}

// OFFSET: 0x00696690
static bool JamSplineFace_GetString(const void* data, const void* meta, char* buffer, uint32_t size) {
    switch (*static_cast<const uint32_t*>(data)) {
        case 0:
            SStrCopy(buffer, "SPLINE_FACE_DIRECTION", size);
            return true;

        case 1:
            SStrCopy(buffer, "SPLINE_FACE_SPOT", size);
            return true;

        case 2:
            SStrCopy(buffer, "SPLINE_FACE_GUID", size);
            return true;

        case 3:
            SStrCopy(buffer, "SPLINE_FACE_NOTHING", size);
            return true;

        default:
            return false;
    }
}

// OFFSET: 0x006967C0
static bool JamUpdateCollisionHeightReason_SetDefault(void* data) {
    *static_cast<uint32_t*>(data) = 2;
    return true;
}

// OFFSET: 0x006967D0
static bool JamUpdateCollisionHeightReason_SetString(void* data, const void* meta, const char* value) {
    if (!strcmp("UPDATE_COLLISION_HEIGHT_FORCE", value)) {
        *static_cast<uint32_t*>(data) = 2;
        return true;
    }

    if (!strcmp("UPDATE_COLLISION_HEIGHT_MOUNT", value)) {
        *static_cast<uint32_t*>(data) = 0;
        return true;
    }

    if (!strcmp("UPDATE_COLLISION_HEIGHT_SCALE", value)) {
        *static_cast<uint32_t*>(data) = 1;
        return true;
    }

    return false;
}

// OFFSET: 0x006968A0
static bool JamUpdateCollisionHeightReason_GetString(const void* data, const void* meta, char* buffer, uint32_t size) {
    switch (*static_cast<const uint32_t*>(data)) {
        case 0:
            SStrCopy(buffer, "UPDATE_COLLISION_HEIGHT_MOUNT", size);
            return true;

        case 1:
            SStrCopy(buffer, "UPDATE_COLLISION_HEIGHT_SCALE", size);
            return true;

        case 2:
            SStrCopy(buffer, "UPDATE_COLLISION_HEIGHT_FORCE", size);
            return true;

        default:
            return false;
    }
}

// OFFSET: 0x0069FFA0
static bool JamTradeStatus_SetDefault(void* data) {
    *static_cast<uint32_t*>(data) = 21;
    return true;
}

// OFFSET: 0x0069FFB0
static bool JamTradeStatus_SetString(void* data, const void* meta, const char* value) {
    if (!strcmp("TRADE_STATUS_PLAYER_BUSY", value)) {
        *static_cast<uint32_t*>(data) = 21;
        return true;
    }

    if (!strcmp("TRADE_STATUS_PROPOSED", value)) {
        *static_cast<uint32_t*>(data) = 12;
        return true;
    }

    if (!strcmp("TRADE_STATUS_INITIATED", value)) {
        *static_cast<uint32_t*>(data) = 0;
        return true;
    }

    if (!strcmp("TRADE_STATUS_CANCELLED", value)) {
        *static_cast<uint32_t*>(data) = 23;
        return true;
    }

    if (!strcmp("TRADE_STATUS_ACCEPTED", value)) {
        *static_cast<uint32_t*>(data) = 6;
        return true;
    }

    if (!strcmp("TRADE_STATUS_ALREADY_TRADING", value)) {
        *static_cast<uint32_t*>(data) = 18;
        return true;
    }

    if (!strcmp("TRADE_STATUS_PLAYER_NOT_FOUND", value)) {
        *static_cast<uint32_t*>(data) = 17;
        return true;
    }

    if (!strcmp("TRADE_STATUS_STATE_CHANGED", value)) {
        *static_cast<uint32_t*>(data) = 25;
        return true;
    }

    if (!strcmp("TRADE_STATUS_COMPLETE", value)) {
        *static_cast<uint32_t*>(data) = 9;
        return true;
    }

    if (!strcmp("TRADE_STATUS_UNACCEPTED", value)) {
        *static_cast<uint32_t*>(data) = 22;
        return true;
    }

    if (!strcmp("TRADE_STATUS_TOO_FAR_AWAY", value)) {
        *static_cast<uint32_t*>(data) = 16;
        return true;
    }

    if (!strcmp("TRADE_STATUS_WRONG_FACTION", value)) {
        *static_cast<uint32_t*>(data) = 20;
        return true;
    }

    if (!strcmp("TRADE_STATUS_FAILED", value)) {
        *static_cast<uint32_t*>(data) = 31;
        return true;
    }

    if (!strcmp("TRADE_STATUS_PETITION", value)) {
        *static_cast<uint32_t*>(data) = 1;
        return true;
    }

    if (!strcmp("TRADE_STATUS_PLAYER_IGNORED", value)) {
        *static_cast<uint32_t*>(data) = 4;
        return true;
    }

    if (!strcmp("TRADE_STATUS_STUNNED", value)) {
        *static_cast<uint32_t*>(data) = 27;
        return true;
    }

    if (!strcmp("TRADE_STATUS_TARGET_STUNNED", value)) {
        *static_cast<uint32_t*>(data) = 29;
        return true;
    }

    if (!strcmp("TRADE_STATUS_DEAD", value)) {
        *static_cast<uint32_t*>(data) = 13;
        return true;
    }

    if (!strcmp("TRADE_STATUS_TARGET_DEAD", value)) {
        *static_cast<uint32_t*>(data) = 5;
        return true;
    }

    if (!strcmp("TRADE_STATUS_LOGGING_OUT", value)) {
        *static_cast<uint32_t*>(data) = 3;
        return true;
    }

    if (!strcmp("TRADE_STATUS_TARGET_LOGGING_OUT", value)) {
        *static_cast<uint32_t*>(data) = 7;
        return true;
    }

    if (!strcmp("TRADE_STATUS_RESTRICTED_ACCOUNT", value)) {
        *static_cast<uint32_t*>(data) = 10;
        return true;
    }

    if (!strcmp("TRADE_STATUS_WRONG_REALM", value)) {
        *static_cast<uint32_t*>(data) = 26;
        return true;
    }

    if (!strcmp("TRADE_STATUS_NOT_ON_TAPLIST", value)) {
        *static_cast<uint32_t*>(data) = 2;
        return true;
    }

    if (!strcmp("TRADE_STATUS_CURRENCY_NOT_TRADABLE", value)) {
        *static_cast<uint32_t*>(data) = 19;
        return true;
    }

    if (!strcmp("TRADE_STATUS_NOT_ENOUGH_CURRENCY", value)) {
        *static_cast<uint32_t*>(data) = 24;
        return true;
    }

    return false;
}

// OFFSET: 0x006A0640
static bool JamTradeStatus_GetString(const void* data, const void* meta, char* buffer, uint32_t size) {
    switch (*static_cast<const uint32_t*>(data)) {
        case 0:
            SStrCopy(buffer, "TRADE_STATUS_INITIATED", size);
            return true;

        case 1:
            SStrCopy(buffer, "TRADE_STATUS_PETITION", size);
            return true;

        case 2:
            SStrCopy(buffer, "TRADE_STATUS_NOT_ON_TAPLIST", size);
            return true;

        case 3:
            SStrCopy(buffer, "TRADE_STATUS_LOGGING_OUT", size);
            return true;

        case 4:
            SStrCopy(buffer, "TRADE_STATUS_PLAYER_IGNORED", size);
            return true;

        case 5:
            SStrCopy(buffer, "TRADE_STATUS_TARGET_DEAD", size);
            return true;

        case 6:
            SStrCopy(buffer, "TRADE_STATUS_ACCEPTED", size);
            return true;

        case 7:
            SStrCopy(buffer, "TRADE_STATUS_TARGET_LOGGING_OUT", size);
            return true;

        case 9:
            SStrCopy(buffer, "TRADE_STATUS_COMPLETE", size);
            return true;

        case 10:
            SStrCopy(buffer, "TRADE_STATUS_RESTRICTED_ACCOUNT", size);
            return true;

        case 12:
            SStrCopy(buffer, "TRADE_STATUS_PROPOSED", size);
            return true;

        case 13:
            SStrCopy(buffer, "TRADE_STATUS_DEAD", size);
            return true;

        case 16:
            SStrCopy(buffer, "TRADE_STATUS_TOO_FAR_AWAY", size);
            return true;

        case 17:
            SStrCopy(buffer, "TRADE_STATUS_PLAYER_NOT_FOUND", size);
            return true;

        case 18:
            SStrCopy(buffer, "TRADE_STATUS_ALREADY_TRADING", size);
            return true;

        case 19:
            SStrCopy(buffer, "TRADE_STATUS_CURRENCY_NOT_TRADABLE", size);
            return true;

        case 20:
            SStrCopy(buffer, "TRADE_STATUS_WRONG_FACTION", size);
            return true;

        case 21:
            SStrCopy(buffer, "TRADE_STATUS_PLAYER_BUSY", size);
            return true;

        case 22:
            SStrCopy(buffer, "TRADE_STATUS_UNACCEPTED", size);
            return true;

        case 23:
            SStrCopy(buffer, "TRADE_STATUS_CANCELLED", size);
            return true;

        case 24:
            SStrCopy(buffer, "TRADE_STATUS_NOT_ENOUGH_CURRENCY", size);
            return true;

        case 25:
            SStrCopy(buffer, "TRADE_STATUS_STATE_CHANGED", size);
            return true;

        case 26:
            SStrCopy(buffer, "TRADE_STATUS_WRONG_REALM", size);
            return true;

        case 27:
            SStrCopy(buffer, "TRADE_STATUS_STUNNED", size);
            return true;

        case 29:
            SStrCopy(buffer, "TRADE_STATUS_TARGET_STUNNED", size);
            return true;

        case 31:
            SStrCopy(buffer, "TRADE_STATUS_FAILED", size);
            return true;

        default:
            return false;
    }
}
