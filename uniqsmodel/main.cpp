#include "stl.h"

#include "uniqsproto.h"

#include "generator.h"

#include "typeparser_cpp.h"
#include "typeparser_cpp_shm.h"
#include "typeparser_csharp.h"
#include "typeparser_tolua.h"

#include "stl.h"
using namespace std;

void help() {
    cout << "uniqsproto.exe cpp/csharp/tolua inputfile.xml[ outputdir[ pboutputdir]]" << endl;
}

int main_logic(int argc, const char** argv) {
    if (argc < 3) {
        help();
        return -5;
    }

    UniqsProto proto;

    std::string strGenerateType = argv[1];
    std::string strPeerType = argv[2];
    EGenerateType eGenerateType;
    TypeParser_cpp oTypeParser_cpp;
    TypeParser_cpp_shm oTypeParser_cpp_shm;
    TypeParser_csharp oTypeParser_csharp;
    TypeParser_tolua oTypeParser_tolua;
    if (strGenerateType == "cpp") {
        eGenerateType = EGenerateType_cpp;

        g_pTypeParser = &oTypeParser_cpp;
    } else if (strGenerateType == "cpp_shm") {
        eGenerateType = EGenerateType_cpp_shm;

        g_pTypeParser = &oTypeParser_cpp_shm;
    } else if (strGenerateType == "csharp") {
        eGenerateType = EGenerateType_csharp;

        g_pTypeParser = &oTypeParser_csharp;
    } else if (strGenerateType == "tolua") {
        eGenerateType = EGenerateType_tolua;

        g_pTypeParser = &oTypeParser_tolua;
    } else {
        help();
        return -4;
    }

    std::string strFrom = argv[2];
    auto pos = strFrom.find(".xml");
    if (pos == std::string::npos) {
        help();
        return -3;
    }

    std::string strTo;
    if (argc > 3) {
        strTo = argv[3];
    } else {
        strTo = strFrom.substr(0, pos);
    }

    std::string strPBTo;
    if (argc > 4) {
        strPBTo = argv[4];
    } else {
        strPBTo = strTo;
    }

    std::string strError;
    bool bRet = false;
    bRet = proto.ReadProto(strFrom, strError);
    if (!bRet) {
        std::cout << strError << std::endl;
        return -1;
    }

    bRet = proto.Check(strError);
    if (!bRet) {
        std::cout << strError << std::endl;
        return -1;
    }

    // 先不check了，只管生成，只管序列化和反序列化，后面再优化
    Generator oGenerator;
    if (!oGenerator.Generate(eGenerateType, proto, strTo, strPBTo, strError)) {
        std::cout << strError << std::endl;
        return -2;
    }

    return 0;
}

int main(int argc, const char** argv) {
    int nRet = main_logic(argc, argv);
    if (nRet != 0) {
        return nRet;
    }

    cout << "uniqsmodel everything ok!!!" << endl;

    return 0;
}
