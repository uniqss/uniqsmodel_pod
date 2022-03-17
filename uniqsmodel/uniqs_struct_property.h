#pragma once

#include "stl.h"
#include "uniqs_types.h"

class Uniqs_StructProperty {
   public:
    Uniqs_StructProperty();

   public:
    std::string strName;

    std::string strRefs;
    std::vector<std::string> vecRefs;

    std::string strValueType;
    EUniqsRawType eValueType;
    std::string strComplexType;
    EUniqsComplexType eComplexType;
    std::string strMax;
    std::string strKey;
    std::string strKeyType;
    EUniqsRawType eKeyType;

    std::string strDescription;
};
