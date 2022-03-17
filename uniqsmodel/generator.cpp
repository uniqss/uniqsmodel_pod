#include "generator.h"

#include "generator_cpp.h"

#include "uniqsproto_constdefines.h"

#include "stl.h"
using namespace std;

Generator *g_pGenerator;

void Generator::WriteFileDeclarations(std::ofstream &ofs) {
    // std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    // ofs << conv.to_bytes(g_strFileDeclaration);
    ofs << g_strFileDeclaration;
    ofs << endl;
    ofs << endl;
}
