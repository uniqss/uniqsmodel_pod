#pragma once

#include "generator.h"

class Generator_cpp : public Generator {
   public:
    virtual bool Generate(UniqsProto& proto, const string& strTo, const string& strPBTo, std::string& strError);
    virtual const string& GetComplexLenType() {
        static string strComplexLenType = "unsigned short";
        return strComplexLenType;
    }

   private:
    bool _GenerateDefines(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs_header(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs_source(UniqsProto& proto, const string& strTo, std::string& strError);
};
