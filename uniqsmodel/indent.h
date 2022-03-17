#pragma once

#include "stl.h"

class Indent {
   private:
    int m_nIndent;

   public:
    Indent() { m_nIndent = 0; }
    Indent(int nIndent) { m_nIndent = nIndent; }

   public:
    Indent& operator++() {
        ++m_nIndent;
        return *this;
    }
    Indent& operator--() {
        --m_nIndent;
        return *this;
    }

    friend ofstream& operator<<(ofstream& ofs, Indent& indent) {
        for (int nIdx = 0; nIdx < indent.m_nIndent; ++nIdx) {
            ofs << '\t';
        }
        return ofs;
    }
};
