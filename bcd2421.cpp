#include "bcd2421.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

using namespace std;

// Таблица соответствия для 2421 кода
const map<string, char> dict_2421_rev = {
    {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'}, {"0100", '4'},
    {"1011", '5'}, {"1100", '6'}, {"1101", '7'}, {"1110", '8'}, {"1111", '9'}
};

const map<char, Tetrad> dict_2421 = {
    {'0', {0, 0, 0, 0}}, {'1', {0, 0, 0, 1}}, {'2', {0, 0, 1, 0}}, 
    {'3', {0, 0, 1, 1}}, {'4', {0, 1, 0, 0}}, {'5', {1, 0, 1, 1}},
    {'6', {1, 1, 0, 0}}, {'7', {1, 1, 0, 1}}, {'8', {1, 1, 1, 0}}, 
    {'9', {1, 1, 1, 1}}
};

// Вспомогательная функция для преобразования тетрады в десятичную цифру
int tetrad_to_digit(const Tetrad& t) {
    string key;
    for (int bit : t) {
        key += to_string(bit);
    }
    
    auto it = dict_2421_rev.find(key);
    if (it != dict_2421_rev.end()) {
        return it->second - '0';
    }
    return -1; // ошибка
}

// Вспомогательная функция для преобразования десятичной цифры в тетраду
Tetrad digit_to_tetrad(int d) {
    char c = '0' + d;
    auto it = dict_2421.find(c);
    if (it != dict_2421.end()) {
        return it->second;
    }
    return {0, 0, 0, 0}; // fallback
}

BCDNumber int_to_2421_array(int num) {
    string num_str = to_string(abs(num));
    BCDNumber result;
    
    for (char digit : num_str) {
        auto it = dict_2421.find(digit);
        if (it != dict_2421.end()) {
            result.push_back(it->second);
        }
    }
    
    return result;
}

int array_2421_to_int(const BCDNumber& arr) {
    string result_str;
    
    for (const Tetrad& tetrad : arr) {
        string key;
        for (int bit : tetrad) {
            key += to_string(bit);
        }
        
        auto it = dict_2421_rev.find(key);
        if (it != dict_2421_rev.end()) {
            result_str += it->second;
        }
    }
    
    return stoi(result_str);
}

pair<int, Tetrad> add_4bit_binary(const Tetrad& a, const Tetrad& b, int cin) {
    Tetrad ans(4, 0);
    int carry = cin;
    
    for (int i = 3; i >= 0; i--) {
        int total = a[i] + b[i] + carry;
        ans[i] = total % 2;
        carry = total / 2;
    }
    
    return make_pair(carry, ans);
}

pair<int, Tetrad> add_2421_digit(const Tetrad& digit1, const Tetrad& digit2, int carry_in) {
    // Преобразуем тетрады в десятичные цифры
    int d1 = tetrad_to_digit(digit1);
    int d2 = tetrad_to_digit(digit2);
    
    if (d1 == -1 || d2 == -1) {
        // Если не удалось преобразовать, используем двоичное сложение
        auto [temp_cout, temp_sum] = add_4bit_binary(digit1, digit2, carry_in);
        return make_pair(temp_cout, temp_sum);
    }
    
    // Складываем десятичные цифры с учетом переноса
    int sum = d1 + d2 + carry_in;
    
    // Результат и новый перенос
    int result_digit = sum % 10;
    int new_carry = sum / 10;
    
    // Преобразуем обратно в тетраду 2421
    return make_pair(new_carry, digit_to_tetrad(result_digit));
}

BCDNumber add_2421_numbers(const BCDNumber& num1, const BCDNumber& num2) {
    size_t max_len = max(num1.size(), num2.size());
    
    // Выравниваем длины, добавляя нулевые тетрады слева
    BCDNumber num1_aligned = num1;
    BCDNumber num2_aligned = num2;
    
    while (num1_aligned.size() < max_len) {
        num1_aligned.insert(num1_aligned.begin(), {0, 0, 0, 0});
    }
    while (num2_aligned.size() < max_len) {
        num2_aligned.insert(num2_aligned.begin(), {0, 0, 0, 0});
    }
    
    BCDNumber result;
    int carry = 0;
    
    // Складываем справа налево (от младших разрядов к старшим)
    for (int i = max_len - 1; i >= 0; i--) {
        auto [new_carry, sum_digit] = add_2421_digit(num1_aligned[i], num2_aligned[i], carry);
        carry = new_carry;
        result.insert(result.begin(), sum_digit);
    }
    
    // Если остался перенос, добавляем новую тетраду
    if (carry > 0) {
        result.insert(result.begin(), digit_to_tetrad(carry));
    }
    
    return result;
}

string tetrad_to_string(const Tetrad& t) {
    string result;
    for (int bit : t) {
        result += to_string(bit);
    }
    return result;
}

string bcd_to_string(const BCDNumber& bcd) {
    string result;
    for (const Tetrad& t : bcd) {
        if (!result.empty()) result += " ";
        result += tetrad_to_string(t);
    }
    return result;
}
