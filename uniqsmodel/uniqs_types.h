#pragma once

enum EUniqsRawType {
    EUniqsRawType_invalid,
    EUniqsRawType_bool,
    EUniqsRawType_int8,
    EUniqsRawType_uint8,
    EUniqsRawType_int16,
    EUniqsRawType_uint16,
    EUniqsRawType_int32,
    EUniqsRawType_uint32,
    EUniqsRawType_int64,
    EUniqsRawType_uint64,
    EUniqsRawType_string,
    EUniqsRawType_max,
};

enum EUniqsComplexType {
    EUniqsComplexType_invalid,
    EUniqsComplexType_rawarray, //原始数组
    EUniqsComplexType_array,
    // EUniqsComplexType_map, // pod不支持map
    EUniqsComplexType_max,
};
