#include "Object.h"

// STATIC: 0x00D18E3C
const uint32_t CGObject::s_savedFields[6] = {
    OBJECT_FIELD_DATA,
    OBJECT_FIELD_DATA + 1,
    OBJECT_FIELD_TYPE,
    OBJECT_FIELD_ENTRY,
    OBJECT_FIELD_SCALE_X,
    OBJECT_FIELD_PADDING,
};

// PROVEN: 784d98da IDENTICAL/NONE
// OFFSET: 0x00619630/0x1402A6B50
int32_t CGObject::TotalFieldsSaved() {
    return sizeof(CGObject::s_savedFields) / sizeof(CGObject::s_savedFields[0]);
}

// PROVEN: e4ddf03c IDENTICAL/NONE
// OFFSET: 0x00A27BC0/0x1407CF370
int32_t CGObject::GetSavedIndexFromDataIndex(uint32_t index) {
    for (int32_t i = 0; i < sizeof(CGObject::s_savedFields) / sizeof(CGObject::s_savedFields[0]); i++) {
        if (CGObject::s_savedFields[i] == index) {
            return i;
        }
    }

    return OBJECT_END;
}
