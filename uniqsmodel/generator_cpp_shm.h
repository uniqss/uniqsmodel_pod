#pragma once

#include "generator.h"

class Generator_cpp_shm : public Generator {
   public:
    Generator_cpp_shm();
    virtual ~Generator_cpp_shm() {}

   public:
    virtual bool Generate_i(UniqsProto& proto, const string& strTo, const string& strPBTo, std::string& strError);
    virtual const string& GetComplexLenType() {
        static string strComplexLenType = "unsigned short";
        return strComplexLenType;
    }

   private:
    bool _GenerateDefines(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs_header(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateStructs_source(UniqsProto& proto, const string& strTo, std::string& strError);
};
