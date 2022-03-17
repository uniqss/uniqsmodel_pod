#include "stl.h"

#include "uniqsmodel.h"

#include "generator.h"

#include "typeparser_cpp.h"
#include "generator_cpp.h"

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

    UniqsModel proto;

    std::string strGenerateType = argv[1];
    std::string strPeerType = argv[2];

    TypeParser_cpp oTypeParser_cpp;
    Generator_cpp oGenerator_cpp;
    
    if (strGenerateType == "cpp") {
        g_pGenerator = &oGenerator_cpp;
        g_pTypeParser = &oTypeParser_cpp;
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
    if (!g_pGenerator->Generate(proto, strTo, strPBTo, strError)) {
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
