#pragma once

#include "stl.h"
#include "uniqsproto_types.h"

class TypeParser {
    static std::unordered_map<string, EUniqsRawType> m_mapRawTypes;
    static std::unordered_map<string, EUniqsComplexType> m_mapComplexTypes;

   public:
    TypeParser();

   public:
    bool ParseRawType(const string& strRawType, EUniqsRawType& rRawType);
    bool ParseComplexType(const string& strComplexType, EUniqsComplexType& rComplexType);

   public:
    bool IsRawType(const string& strType) { return m_mapRawTypes.find(strType) != m_mapRawTypes.end(); }
    bool IsComplexType(const string& strType) { return m_mapComplexTypes.find(strType) != m_mapComplexTypes.end(); }
    bool IsRawType(EUniqsRawType eType) { return eType > EUniqsRawType_invalid && eType < EUniqsRawType_max; }
    bool IsComplexType(EUniqsComplexType eType) { return eType > EUniqsComplexType_invalid && eType < EUniqsComplexType_max; }

   public:
    virtual const char* GetRawTypeSpecific(EUniqsRawType eType) = 0;
    virtual const char* GetComplexTypeSpecific(EUniqsComplexType eType) = 0;
    virtual const char* GetDefaultValueSpecific(EUniqsRawType eType) = 0;
};

extern TypeParser* g_pTypeParser;
