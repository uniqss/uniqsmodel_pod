#pragma once

#include "generator.h"

class Generator_cpp : public Generator {
   public:
    virtual bool Generate(UniqsModel& proto, const string& strTo, const string& strPBTo, std::string& strError);
    virtual const string& GetComplexLenType() {
        static string strComplexLenType = "unsigned short";
        return strComplexLenType;
    }

   private:
    bool _GenerateDefines(UniqsModel& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs_header(UniqsModel& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs_source(UniqsModel& proto, const string& strTo, std::string& strError);
};
