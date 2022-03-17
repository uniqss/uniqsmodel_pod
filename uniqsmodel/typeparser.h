#pragma once

#include "stl.h"
#include "uniqsproto_types.h"

class UniqsProto_StructProperty;

class TypeParser {
    static std::unordered_map<string, EUniqsRawType> m_mapRawTypes;
    static std::unordered_map<string, EUniqsComplexType> m_mapComplexTypes;

   public:
    TypeParser();

   public:
    bool ParseRawType(const string& strRawType, EUniqsRawType& rRawType);
    bool ParseComplexType(const string& strComplexType, EUniqsComplexType& rComplexType);

   public:
    bool IsRawValueType(const UniqsProto_StructProperty& prop);
    bool IsComplexType(const UniqsProto_StructProperty& prop);
    bool IsRawKeyType(const UniqsProto_StructProperty& prop);

   public:
    virtual const char* GetRawTypeSpecific(EUniqsRawType eType) = 0;
    virtual std::string FormComplexStructMember(const UniqsProto_StructProperty& prop) = 0;
    virtual std::string FormComplexStructMemberClear(EUniqsComplexType eComplexType, EUniqsRawType eValueType, const std::string& strPropName,
                                                     const std::string& strMax) = 0;
    virtual const char* GetComplexTypeSpecific(EUniqsComplexType eType) = 0;
    virtual const char* GetDefaultValueSpecific(EUniqsRawType eType) = 0;
};

extern TypeParser* g_pTypeParser;
