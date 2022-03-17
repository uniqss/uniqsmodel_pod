#pragma once

#include "uniqsproto.h"

class TypeParser;

class Generator {
   protected:
    std::unique_ptr<TypeParser> m_pTypeParser;

   public:
    Generator() : m_pTypeParser(nullptr) {}

   public:
    virtual bool Generate(EGenerateType etype, UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError);

    void WriteUtf8BOMHead(std::ofstream &ofs) {
        char szTmp[4] = {(char)0xEF, (char)0xBB, (char)0xBF, '\0'};
        ofs << szTmp;
    }
    void WriteFileDeclarations(std::ofstream &ofs);

   protected:
    virtual bool Generate_i(UniqsProto &proto, const string &strTo, const string &strPBTo, std::string &strError);
    virtual const string &GetComplexLenType() {
        static std::string strRet = "";
        return strRet;
    };
};
