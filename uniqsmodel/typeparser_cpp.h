#pragma once

#include "typeparser.h"

class TypeParser_cpp : public TypeParser {
   public:
    virtual const char* GetRawTypeSpecific(EUniqsRawType eType);
    virtual std::string FormComplexStructMember(EUniqsComplexType eComplexType, EUniqsRawType eRawType, const std::string& strPropName, const std::string& strMax);
    virtual std::string FormComplexStructMemberClear(EUniqsComplexType eComplexType, EUniqsRawType eRawType, const std::string& strPropName, const std::string& strMax);
    virtual const char* GetComplexTypeSpecific(EUniqsComplexType eType);
    virtual const char* GetDefaultValueSpecific(EUniqsRawType eType);
};
