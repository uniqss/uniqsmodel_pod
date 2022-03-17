#pragma once

#include "stl.h"
#include "uniqs_types.h"

#include "uniqs_struct_property.h"

class Uniqs_Struct {
   public:
    std::vector<Uniqs_StructProperty> vecProperties;
    std::unordered_map<std::string, int> mapProperties;

    std::vector<std::string> vecSubsets;
    std::unordered_map<std::string, int> mapSubsets;

   public:
    std::string strName;
    std::string strBaseName;
    bool bSingleton;
    std::string strDescription;
};
