#pragma once

#include "stl.h"
#include "uniqsproto_types.h"

#include "uniqsproto_struct_property.h"

class UniqsProto_Struct {
   public:
    std::vector<UniqsProto_StructProperty> vecProperties;
    std::unordered_map<std::string, int> mapProperties;

   public:
    std::string strName;
    bool bSingleton;
    std::string strDescription;
};
