#pragma once

#include "typeparser.h"

class Uniqs_StructProperty;

class TypeParser_cpp : public TypeParser {
   public:
    virtual const char* GetRawTypeSpecific(EUniqsRawType eType);
    virtual std::string FormComplexStructMember(const Uniqs_StructProperty& prop);
    virtual std::string FormComplexStructMemberClear(EUniqsComplexType eComplexType, EUniqsRawType eValueType, const std::string& strPropName, const std::string& strMax);
    virtual const char* GetComplexTypeSpecific(EUniqsComplexType eType);
    virtual const char* GetDefaultValueSpecific(EUniqsRawType eType);
};
