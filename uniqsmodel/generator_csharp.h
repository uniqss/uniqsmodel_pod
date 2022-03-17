#pragma once

#include "generator.h"

class Generator_csharp : public Generator {
   public:
    Generator_csharp() {}
    virtual ~Generator_csharp() {}

   public:
    virtual bool Generate_i(UniqsProto& proto, const string& strTo, const string& strPBTo, std::string& strError);
    virtual const string& GetComplexLenType() {
        static string strComplexLenType = "UInt16";
        return strComplexLenType;
    }

   private:
    bool _GenerateDefines(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateMsgMapper(UniqsProto& proto, const string& strTo, std::string& strError);
};
