#include "generator_tolua.h"

#include "stl.h"

#include "uniqsproto_constdefines.h"
#include "indent.h"

#include "typeparser.h"

#define LEFT_BRACE                \
    ofs << indent << "{" << endl; \
    ++indent;

#define RIGHT_BRACE \
    --indent;       \
    ofs << indent << "}" << endl;
#define RIGHT_BRACE_FORCLASS \
    --indent;                \
    ofs << indent << "};" << endl;

#define PUBLIC_DECLARATION              \
    --indent;                           \
    ofs << indent << "public:" << endl; \
    ++indent;
#define PRIVATE_DECLARATION              \
    --indent;                            \
    ofs << indent << "private:" << endl; \
    ++indent;

#define IFDEFCPLUSPLUS_BEGIN ofs << "#ifdef __cplusplus" << endl;
#define EIF ofs << "#endif" << endl;

#define IFNDEFTOLUARELEASE ofs << "#ifndef TOLUA_RELEASE" << endl;

bool Generator_tolua::_GenerateAllocator_header(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strToFile = strTo;
    strToFile += "uniqsallocator_" + proto.m_strName + ".h";

    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = __FUNCTION__;
        strError += " ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);
    WriteFileDeclarations(ofs);

    std::string strPragmaAndInclude = "#pragma once";
    strPragmaAndInclude += "\n#include <stdint.h>";
    strPragmaAndInclude += "\n#include <string>";
    strPragmaAndInclude += "\n#include <vector>";
    strPragmaAndInclude += "\n#include <unordered_map>";
    ofs << strPragmaAndInclude << endl;

    Indent indent;

    for (const auto &it : proto.vecStructs) {
        if (it.bSingleton) continue;
        std::unordered_set<std::string> setAvoidConflict;
        if (setAvoidConflict.find(it.strName) != setAvoidConflict.end()) continue;
        setAvoidConflict.insert(it.strName);

        ofs << "#include \"" << it.strName << ".h\"" << endl;
    }

    ofs << endl;
    if (!g_strNamespace.empty()) {
        ofs << indent << g_strNamespace << endl;
    }

    LEFT_BRACE;

    ofs << indent << "// uniqs allocator" << endl;
    ofs << indent << "// ****if you need special allocator****" << endl;
    ofs << indent << "// use YourAllocator_" << proto.m_strName << " : public UniqsAllocator_" << proto.m_strName << endl;
    ofs << indent << "// and implement the `virtual UAlloc_*** functions` and `virtual UFree_*** functions`." << endl;
    ofs << indent << "// Remember to set g_pUniqsAllocator_" << proto.m_strName << " on program startup" << endl;
    ofs << indent << "class UniqsAllocator_" << proto.m_strName << endl;
    LEFT_BRACE;

    PUBLIC_DECLARATION;

    for (const auto &it : proto.vecStructs) {
        if (it.bSingleton) continue;
        std::unordered_set<std::string> setAvoidConflict;
        if (setAvoidConflict.find(it.strName) != setAvoidConflict.end()) continue;
        setAvoidConflict.insert(it.strName);

        ofs << indent << "virtual " << it.strName << "* UAlloc_" << it.strName << "();" << endl;
        ofs << indent << "virtual void UFree_" << it.strName << "(" << it.strName << "* p);" << endl;
    }

    RIGHT_BRACE_FORCLASS;

    ofs << endl;

    ofs << indent << "extern UniqsAllocator_" << proto.m_strName << "* g_pUniqsAllocator_" << proto.m_strName << ";" << endl;

    RIGHT_BRACE;

    return true;
}

bool Generator_tolua::_GenerateAllocator_source(UniqsProto &proto, const string &strTo, std::string &strError) {
    string strDefineName = proto.m_strName + pszDefinePostfix;

    string strToFile = strTo;
    strToFile += "uniqsallocator_" + proto.m_strName + ".cpp";

    std::ofstream ofs(strToFile.c_str());
    if (!ofs) {
        strError = __FUNCTION__;
        strError += " ofs create failed.";
        return false;
    }

    WriteUtf8BOMHead(ofs);
    WriteFileDeclarations(ofs);

    ofs << "#include \"uniqsallocator_" + proto.m_strName + ".h\"" << endl;

    Indent indent;
    if (!g_strNamespace.empty()) {
        ofs << indent << g_strNamespace << endl;
    }
    LEFT_BRACE;

    ofs << indent << "UniqsAllocator_" << proto.m_strName << " g_oUniqsAllocator_" << proto.m_strName << ";" << endl;
    ofs << indent << "UniqsAllocator_" << proto.m_strName << "* g_pUniqsAllocator_" << proto.m_strName << " = &g_oUniqsAllocator_" << proto.m_strName << ";" << endl;

    ofs << endl;

    for (const auto &it : proto.vecStructs) {
        if (it.bSingleton) {
            continue;
        }
        std::unordered_set<std::string> setAvoidConflict;
        if (setAvoidConflict.find(it.strName) != setAvoidConflict.end()) continue;
        setAvoidConflict.insert(it.strName);

        // UAlloc
        ofs << indent << "" << it.strName << "* UniqsAllocator_" << proto.m_strName << "::UAlloc_" << it.strName << "()" << endl;
        LEFT_BRACE;
        ofs << indent << "" << it.strName << "* p = new " << it.strName << "(); return p;" << endl;
        RIGHT_BRACE;
        // UFree
        ofs << indent << "void UniqsAllocator_" << proto.m_strName << "::UFree_" << it.strName << "(" << it.strName << "* p)" << endl;
        LEFT_BRACE;
        ofs << indent << "if (p) delete p; p = nullptr;" << endl;
        RIGHT_BRACE;
    }

    RIGHT_BRACE;

    return true;
}
