#pragma once

#include "generator.h"
#include "typeparser.h"


class Generator_tolua : public Generator {
   public:
    Generator_tolua() {}
    virtual ~Generator_tolua() {}

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
    bool _GenerateAllocator_header(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _GenerateAllocator_source(UniqsProto& proto, const string& strTo, std::string& strError);

    bool _Generate_tolua_header(UniqsProto& proto, const string& strTo, std::string& strError);
    bool _Generate_tolua_source(UniqsProto& proto, const string& strTo, std::string& strError);

    bool _Generate_tolua_pb(UniqsProto& proto, const string& strPBTo, std::string& strError);

    // umap_int64_t_Player_ 这种
    std::string _GetToLuaComplexTypeExpand(const UniqsProto_StructProperty& rProp);
    // umap<int64_t,Player*> 这种
    std::string _GetToLuaComplexTypeTemplate(const UniqsProto_StructProperty& rProp);

    const char* _tolua_typename(EUniqsRawType eType);
};
