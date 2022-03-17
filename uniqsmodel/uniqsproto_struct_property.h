#pragma once

#include "stl.h"
#include "uniqsproto_types.h"

class UniqsProto_StructProperty {
   public:
    UniqsProto_StructProperty();

   public:
    std::string strName;

    std::string strType;
    EUniqsRawType eRawType;
    std::string strComplexType;
    EUniqsComplexType eComplexType;
    std::string strMax;
    std::string strKey;
    std::string strKeyType;
    EUniqsRawType eRawType_KeyType;

    std::string strDescription;
};
