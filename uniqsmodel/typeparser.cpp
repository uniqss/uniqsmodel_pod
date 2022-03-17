#include "typeparser.h"
#include "uniqs_struct_property.h"

std::unordered_map<string, EUniqsRawType> TypeParser::m_mapRawTypes;
std::unordered_map<string, EUniqsComplexType> TypeParser::m_mapComplexTypes;
TypeParser* g_pTypeParser;

bool TypeParser::ParseRawType(const string& strRawType, EUniqsRawType& rRawType) {
    auto it = m_mapRawTypes.find(strRawType);
    if (it != m_mapRawTypes.end()) {
        rRawType = it->second;
        return true;
    }
    rRawType = EUniqsRawType_invalid;
    return false;
}

bool TypeParser::ParseComplexType(const string& strComplexType, EUniqsComplexType& rComplexType) {
    auto it = m_mapComplexTypes.find(strComplexType);
    if (it != m_mapComplexTypes.end()) {
        rComplexType = it->second;
        return true;
    }
    rComplexType = EUniqsComplexType_invalid;
    return false;
}

TypeParser::TypeParser() {
    if (m_mapRawTypes.empty()) {
        m_mapRawTypes["bool"] = EUniqsRawType_bool;
        m_mapRawTypes["int8"] = EUniqsRawType_int8;
        m_mapRawTypes["uint8"] = EUniqsRawType_uint8;
        m_mapRawTypes["int16"] = EUniqsRawType_int16;
        m_mapRawTypes["uint16"] = EUniqsRawType_uint16;
        m_mapRawTypes["int32"] = EUniqsRawType_int32;
        m_mapRawTypes["uint32"] = EUniqsRawType_uint32;
        m_mapRawTypes["int64"] = EUniqsRawType_int64;
        m_mapRawTypes["uint64"] = EUniqsRawType_uint64;
        m_mapRawTypes["string"] = EUniqsRawType_string;
    }

    if (m_mapComplexTypes.empty()) {
        m_mapComplexTypes["rawarray"] = EUniqsComplexType_rawarray;
        m_mapComplexTypes["array"] = EUniqsComplexType_array;
    }
}

bool TypeParser::IsRawValueType(const Uniqs_StructProperty& prop) {
    return prop.eValueType > EUniqsRawType_invalid && prop.eValueType < EUniqsRawType_max;
}

bool TypeParser::IsComplexType(const Uniqs_StructProperty& prop) {
    return prop.eComplexType > EUniqsComplexType_invalid && prop.eComplexType < EUniqsComplexType_max;
}

bool TypeParser::IsRawKeyType(const Uniqs_StructProperty& prop) {
    return prop.eKeyType > EUniqsRawType_invalid && prop.eKeyType < EUniqsRawType_max;
}
