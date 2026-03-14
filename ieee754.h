#ifndef IEEE754_H
#define IEEE754_H

#include "binary_operations.h"
#include <vector>
#include <array>

using namespace std;

// Основные функции
BinaryArray float_to_ieee_bin(float f);
float ieee_bin_to_float(const BinaryArray& bits);
BinaryArray add_ieee(const BinaryArray& x1, const BinaryArray& x2);
BinaryArray multiply_ieee(const BinaryArray& x1, const BinaryArray& x2);
BinaryArray divide_ieee(const BinaryArray& x1, const BinaryArray& x2);

// Вспомогательные функции для IEEE-754
vector<int> add_mantissas(const vector<int>& a, const vector<int>& b);
vector<int> subtract_mantissas(const vector<int>& a, const vector<int>& b);
vector<int> multiply_mantissas(const vector<int>& a, const vector<int>& b);
BinaryArray decimal_to_binary_8bit(int val);
bool is_greater_or_equal_fixed(const vector<int>& a, const vector<int>& b);

#endif
