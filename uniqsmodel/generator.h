#pragma once

#include "uniqsproto.h"

class TypeParser;

class Generator {
   public:
    virtual bool Generate(UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError) = 0;

    void WriteUtf8BOMHead(std::ofstream &ofs) {
        char szTmp[4] = {(char)0xEF, (char)0xBB, (char)0xBF, '\0'};
        ofs << szTmp;
    }
    void WriteFileDeclarations(std::ofstream &ofs);

   protected:
    virtual const string &GetComplexLenType() {
        static std::string strRet = "";
        return strRet;
    };
};

extern Generator *g_pGenerator;
