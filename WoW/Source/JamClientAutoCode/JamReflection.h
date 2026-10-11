#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMREFLECTION_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMREFLECTION_H

#include <cstddef>
#include <cstdint>

struct JamTypeInfo;

struct JamValue {
    /* 0000 */ const JamTypeInfo* m_type;
    /* 0004 */ void* m_data;
    /* 0008 */ const void* m_meta;
};

struct JamStructMeta {
    /* 0000 */ uint32_t m_count;
    /* 0004 */ const void* const* m_metas;
    /* 0008 */ const char* const* m_names;
    /* 000C */ const JamTypeInfo* const* m_types;
    /* 0010 */ const uint16_t* m_offsets;
    /* 0014 */ const uint16_t* m_buckets;
    /* 0018 */ const uint32_t* m_hashes;
    /* 001C */ uint32_t m_seed;
    /* 0020 */ uint32_t m_bucketCount;
};

struct JamTypeInfo {
    /* 0000 */ uint32_t m_kind;
    /* 0004 */ bool (*m_setDefault)(void* data);
    /* 0008 */ JamValue* (*m_getMember)(JamValue* out, void* data, const void* meta, uint32_t index);
    /* 000C */ const char* (*m_getMemberName)(void* data, const void* meta, uint32_t index);
    /* 0010 */ JamValue* (*m_findMember)(JamValue* out, void* data, const void* meta, const char* name);
    /* 0014 */ uint32_t (*m_getMemberCount)(void* data, const void* meta);
    /* 0018 */ uint32_t (*m_getMemberCapacity)(void* data, const void* meta);
    /* 001C */ JamValue* (*m_getElement)(JamValue* out);
    /* 0020 */ bool (*m_setString)(void* data, const void* meta, const char* value);
    /* 0024 */ bool (*m_setInt32)(void* data, const void* meta, int32_t value);
    /* 0028 */ bool (*m_setUInt32)(void* data, const void* meta, uint32_t value);
    /* 002C */ bool (*m_setInt64)(void* data, const void* meta, int64_t value);
    /* 0030 */ bool (*m_setUInt64)(void* data, const void* meta, uint64_t value);
    /* 0034 */ bool (*m_setFloat)(void* data, const void* meta, float value);
    /* 0038 */ bool (*m_setBool)(void* data, const void* meta, bool value);
    /* 003C */ bool (*m_getString)(const void* data, const void* meta, char* buffer, uint32_t size);
    /* 0040 */ bool (*m_getInt32)(const void* data, const void* meta, int32_t* value);
    /* 0044 */ bool (*m_getUInt32)(const void* data, const void* meta, uint32_t* value);
    /* 0048 */ bool (*m_getInt64)(const void* data, const void* meta, int64_t* value);
    /* 004C */ bool (*m_getUInt64)(const void* data, const void* meta, uint64_t* value);
    /* 0050 */ bool (*m_getFloat)(const void* data, const void* meta, float* value);
    /* 0054 */ bool (*m_getBool)(const void* data, const void* meta, bool* value);
};

extern const JamTypeInfo g_jamTypeNone;
extern const JamTypeInfo g_jamTypeStruct;
extern const JamTypeInfo g_jamTypeInt8;
extern const JamTypeInfo g_jamTypeUInt8;
extern const JamTypeInfo g_jamTypeInt16;
extern const JamTypeInfo g_jamTypeUInt16;
extern const JamTypeInfo g_jamTypeInt32;
extern const JamTypeInfo g_jamTypeUInt32;
extern const JamTypeInfo g_jamTypeInt64;
extern const JamTypeInfo g_jamTypeUInt64;
extern const JamTypeInfo g_jamTypeLong;
extern const JamTypeInfo g_jamTypeFloat;
extern const JamTypeInfo g_jamTypeC2iVector;
extern const JamTypeInfo g_jamTypeC3iVector;
extern const JamTypeInfo g_jamTypeC4iVector;
extern const JamTypeInfo g_jamTypeC2Vector;
extern const JamTypeInfo g_jamTypeC3Vector;
extern const JamTypeInfo g_jamTypeC4Vector;
extern const JamTypeInfo g_jamTypeString;
extern const JamTypeInfo g_jamTypeVoid;
extern const JamTypeInfo g_jamTypeRealmAddress;
extern const JamTypeInfo g_jamTypeBool;
extern const JamTypeInfo g_jamTypeComplaintType;
extern const JamTypeInfo g_jamTypeSplineFace;
extern const JamTypeInfo g_jamTypeUpdateCollisionHeightReason;
extern const JamTypeInfo g_jamTypeTradeStatus;

#endif
