#ifndef BINARY_OPERATIONS_H
#define BINARY_OPERATIONS_H

#include "consts.h"
#include <array>
#include <vector>
#include <string>
#include <tuple>

using namespace std;

using BinaryArray = array<int, NUMB_OF_BITS>;
using DynamicBinary = vector<int>;

// Основные функции
BinaryArray decimal_to_binary(int x);
int from_binary_to_decimal(const BinaryArray& bits);
int unsigned_to_decimal(const vector<int>& bits);
BinaryArray decimal_to_ones_complement(int x);
BinaryArray twos_complement(const BinaryArray& arr);
BinaryArray add_binary(int x1, int x2);
BinaryArray minus_binary(int x1, int x2);
BinaryArray multiply(int x1, int x2);

// Вспомогательные функции для деления
DynamicBinary clean_zeros(const DynamicBinary& bin_array);
bool is_greater_or_equal(const DynamicBinary& a, const DynamicBinary& b);
DynamicBinary subtract_binary(const DynamicBinary& a, const DynamicBinary& b);
tuple<int, DynamicBinary, DynamicBinary> divide(int x1, int x2);

// Вспомогательные функции для вывода
string bin_to_string(const vector<int>& bits);
string bin_to_string(const BinaryArray& bits);

#endif
