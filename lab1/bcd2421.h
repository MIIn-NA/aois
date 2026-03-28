#ifndef BCD2421_H
#define BCD2421_H

#include "consts.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

using Tetrad = vector<int>;  // 4 бита для одной десятичной цифры
using BCDNumber = vector<Tetrad>;  // Массив тетрад

// Основные функции
BCDNumber int_to_2421_array(int num);
int array_2421_to_int(const BCDNumber& arr);
BCDNumber add_2421_numbers(const BCDNumber& num1, const BCDNumber& num2);

// Вспомогательные функции
pair<int, Tetrad> add_4bit_binary(const Tetrad& a, const Tetrad& b, int cin = 0);
pair<int, Tetrad> add_2421_digit(const Tetrad& digit1, const Tetrad& digit2, int carry_in = 0);
string tetrad_to_string(const Tetrad& t);
string bcd_to_string(const BCDNumber& bcd);

#endif
