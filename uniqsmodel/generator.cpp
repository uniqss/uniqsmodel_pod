#include "generator.h"

#include "generator_cpp.h"
#include "generator_cpp_shm.h"
#include "generator_csharp.h"
#include "generator_tolua.h"

#include "uniqsproto_constdefines.h"

#include "stl.h"
using namespace std;

bool Generator::Generate_i(UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError) {
    return false;
}

bool Generator::Generate(EGenerateType etype, UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError) {
    if (etype == EGenerateType_cpp) {
        Generator_cpp oTmp;
        return oTmp.Generate_i(proto, strTo, strPBTo, strError);
    } else if (etype == EGenerateType_cpp_shm) {
        Generator_cpp_shm oTmp;
        return oTmp.Generate_i(proto, strTo, strPBTo, strError);
    } else if (etype == EGenerateType_csharp) {
        Generator_csharp oTmp;
        return oTmp.Generate_i(proto, strTo, strPBTo, strError);
    } else if (etype == EGenerateType_tolua) {
        Generator_tolua oTmp;
        return oTmp.Generate_i(proto, strTo, strPBTo, strError);
    } else {
        strError = "generator not implemented.";
    }
    return false;
}

void Generator::WriteFileDeclarations(std::ofstream &ofs) {
    // std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    // ofs << conv.to_bytes(g_strFileDeclaration);
    ofs << g_strFileDeclaration;
    ofs << endl;
    ofs << endl;
}
