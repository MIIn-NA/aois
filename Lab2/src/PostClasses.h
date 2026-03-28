
#pragma once

#include <vector>
#include <string>

struct TruthTableResult;

struct PostClassesResult {
    bool t0, t1, s, m, l;
    
    PostClassesResult(bool zero, bool one, bool self_dual, bool monotone, bool linear)
        : t0(zero), t1(one), s(self_dual), m(monotone), l(linear) {}
};

class PostClasses {
public:
    PostClassesResult determine(const TruthTableResult& table);
    
private:
    bool is_self_dual(const TruthTableResult& table) const;
    bool is_monotone(const TruthTableResult& table) const;
    bool is_linear(const TruthTableResult& table) const;
    int count_bits(int value) const;
};
