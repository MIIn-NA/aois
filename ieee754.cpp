#include "ieee754.h"
#include "consts.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

using namespace std;

BinaryArray float_to_ieee_bin(float f) {
    BinaryArray result = {0};
    
    if (f == 0.0f) {
        return result;
    }
    
    int sign = (f < 0) ? 1 : 0;
    f = fabsf(f);
    
    // Нормализуем число для IEEE 754
    int exp = 0;
    
    // Приводим число к виду 1.xxx * 2^exp
    if (f >= 2.0f) {
        while (f >= 2.0f) {
            f /= 2.0f;
            exp++;
        }
    } else if (f < 1.0f) {
        while (f < 1.0f) {
            f *= 2.0f;
            exp--;
        }
    }
    
    // exp должен быть смещен на EXP_CONST
    int exp_biased = exp + EXP_CONST;
    
    // Записываем экспоненту (8 бит)
    for (int i = 7; i >= 0; i--) {
        result[1 + i] = (exp_biased >> (7 - i)) & 1;
    }
    
    // Получаем дробную часть мантиссы (убираем ведущую 1)
    f -= 1.0f;  // убираем ведущую 1
    
    // Записываем мантиссу (23 бита)
    for (int i = 0; i < LEN_OF_MANTISS; i++) {
        f *= 2.0f;
        if (f >= 1.0f) {
            result[END_OF_EXP + i] = 1;
            f -= 1.0f;
        } else {
            result[END_OF_EXP + i] = 0;
        }
    }
    
    result[0] = sign;
    return result;
}

float ieee_bin_to_float(const BinaryArray& bits) {
    // Проверка на ноль
    bool all_zero = true;
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        if (bits[i] != 0) {
            all_zero = false;
            break;
        }
    }
    if (all_zero) {
        return 0.0f;
    }
    
    int sign = (bits[0] == 1) ? -1 : 1;
    
    // Получаем экспоненту
    int exp_val = 0;
    for (int i = 1; i < END_OF_EXP; i++) {
        exp_val = exp_val * 2 + bits[i];
    }
    
    // Получаем мантиссу (с ведущей 1)
    float mantissa_val = 1.0f;
    float power = 0.5f;
    for (int i = END_OF_EXP; i < NUMB_OF_BITS; i++) {
        if (bits[i] == 1) {
            mantissa_val += power;
        }
        power /= 2.0f;
    }
    
    float result = sign * mantissa_val * powf(2.0f, exp_val - EXP_CONST);
    return result;
}

vector<int> add_mantissas(const vector<int>& a, const vector<int>& b) {
    vector<int> res;
    int carry = 0;
    
    for (int i = a.size() - 1; i >= 0; i--) {
        int total = a[i] + b[i] + carry;
        res.insert(res.begin(), total % 2);
        carry = total / 2;
    }
    
    if (carry) {
        res.insert(res.begin(), 1);
    }
    
    return res;
}

bool is_greater_or_equal_fixed(const vector<int>& a, const vector<int>& b) {
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] > b[i]) return true;
        if (a[i] < b[i]) return false;
    }
    return true;
}

vector<int> subtract_mantissas(const vector<int>& a, const vector<int>& b) {
    vector<int> res;
    int borrow = 0;
    
    for (int i = a.size() - 1; i >= 0; i--) {
        int diff = a[i] - b[i] - borrow;
        if (diff < 0) {
            diff += 2;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res.insert(res.begin(), diff);
    }
    
    return res;
}

BinaryArray decimal_to_binary_8bit(int val) {
    BinaryArray arr = {0};
    // Заполняем только первые 8 бит после знака
    for (int i = 7; i >= 0; i--) {
        arr[1 + i] = (val >> (7 - i)) & 1;
    }
    return arr;
}

vector<int> multiply_mantissas(const vector<int>& a, const vector<int>& b) {
    vector<int> res(a.size() + b.size(), 0);
    
    for (int i = b.size() - 1; i >= 0; i--) {
        if (b[i] == 1) {
            int carry = 0;
            int offset = b.size() - 1 - i;
            
            for (int j = a.size() - 1; j >= 0; j--) {
                int pos = res.size() - 1 - offset - (a.size() - 1 - j);
                int total = res[pos] + a[j] + carry;
                res[pos] = total % 2;
                carry = total / 2;
            }
            
            if (carry) {
                res[res.size() - 1 - offset - a.size()] += carry;
            }
        }
    }
    
    return res;
}

BinaryArray add_ieee(const BinaryArray& x1, const BinaryArray& x2) {
    int sign1 = x1[0];
    int sign2 = x2[0];
    
    // Получаем экспоненты
    int exp1 = 0, exp2 = 0;
    for (int i = 1; i < END_OF_EXP; i++) {
        exp1 = exp1 * 2 + x1[i];
        exp2 = exp2 * 2 + x2[i];
    }
    int final_exp = max(exp1, exp2);
    
    // Получаем мантиссы (с ведущей 1)
    vector<int> man1 = {1};
    vector<int> man2 = {1};
    for (int i = END_OF_EXP; i < NUMB_OF_BITS; i++) {
        man1.push_back(x1[i]);
        man2.push_back(x2[i]);
    }
    
    // Выравниваем экспоненты
    if (exp1 < exp2) {
        int diff = exp2 - exp1;
        man1.insert(man1.begin(), diff, 0);
        man2.insert(man2.end(), diff, 0);
    } else if (exp1 > exp2) {
        int diff = exp1 - exp2;
        man2.insert(man2.begin(), diff, 0);
        man1.insert(man1.end(), diff, 0);
    }
    
    int final_sign;
    vector<int> sum_man;
    
    if (sign1 == sign2) {
        final_sign = sign1;
        sum_man = add_mantissas(man1, man2);
        
        if (sum_man.size() > man1.size()) {
            final_exp++;
            sum_man.pop_back();
        }
    } else {
        if (is_greater_or_equal_fixed(man1, man2)) {
            final_sign = sign1;
            sum_man = subtract_mantissas(man1, man2);
        } else {
            final_sign = sign2;
            sum_man = subtract_mantissas(man2, man1);
        }
        
        // Проверка на ноль
        bool all_zero = true;
        for (int bit : sum_man) {
            if (bit != 0) {
                all_zero = false;
                break;
            }
        }
        if (all_zero) {
            BinaryArray zero = {0};
            return zero;
        }
        
        // Нормализация
        while (!sum_man.empty() && sum_man[0] == 0) {
            sum_man.erase(sum_man.begin());
            sum_man.push_back(0);
            final_exp--;
        }
    }
    
    // Получаем финальную мантиссу (23 бита)
    vector<int> final_man;
    if (sum_man.size() > 1) {
        final_man.insert(final_man.end(), sum_man.begin() + 1, 
                        sum_man.begin() + min(static_cast<int>(sum_man.size()), LEN_OF_MANTISS + 1));
    }
    
    while (final_man.size() < LEN_OF_MANTISS) {
        final_man.push_back(0);
    }
    
    // Получаем финальную экспоненту
    BinaryArray final_exp_bits = decimal_to_binary_8bit(final_exp);
    
    // Собираем результат
    BinaryArray result;
    result[0] = final_sign;
    for (int i = 0; i < 8; i++) {
        result[1 + i] = final_exp_bits[1 + i];
    }
    for (int i = 0; i < LEN_OF_MANTISS; i++) {
        result[END_OF_EXP + i] = final_man[i];
    }
    
    return result;
}

BinaryArray multiply_ieee(const BinaryArray& x1, const BinaryArray& x2) {
    int sign = x1[0] ^ x2[0];
    
    // Проверка на ноль
    bool x1_zero = true, x2_zero = true;
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        if (x1[i] != 0) x1_zero = false;
        if (x2[i] != 0) x2_zero = false;
    }
    
    if (x1_zero || x2_zero) {
        BinaryArray result = {0};
        result[0] = sign;
        return result;
    }
    
    // Получаем экспоненты
    int exp1 = 0, exp2 = 0;
    for (int i = 1; i < END_OF_EXP; i++) {
        exp1 = exp1 * 2 + x1[i];
        exp2 = exp2 * 2 + x2[i];
    }
    int final_exp = exp1 + exp2 - EXP_CONST;
    
    // Получаем мантиссы (с ведущей 1)
    vector<int> man1 = {1};
    vector<int> man2 = {1};
    for (int i = END_OF_EXP; i < NUMB_OF_BITS; i++) {
        man1.push_back(x1[i]);
        man2.push_back(x2[i]);
    }
    
    // Умножаем мантиссы
    vector<int> prod = multiply_mantissas(man1, man2);
    
    // Нормализация
    vector<int> final_man;
    if (prod[0] == 1) {
        final_exp++;
        final_man.insert(final_man.end(), prod.begin() + 1, 
                        prod.begin() + min(static_cast<int>(prod.size()), LEN_OF_MANTISS + 1));
    } else {
        final_man.insert(final_man.end(), prod.begin() + 2, 
                        prod.begin() + min(static_cast<int>(prod.size()), LEN_OF_MANTISS + 2));
    }
    
    while (final_man.size() < LEN_OF_MANTISS) {
        final_man.push_back(0);
    }
    
    // Получаем финальную экспоненту
    BinaryArray final_exp_bits = decimal_to_binary_8bit(final_exp);
    
    // Собираем результат
    BinaryArray result;
    result[0] = sign;
    for (int i = 0; i < 8; i++) {
        result[1 + i] = final_exp_bits[1 + i];
    }
    for (int i = 0; i < LEN_OF_MANTISS; i++) {
        result[END_OF_EXP + i] = final_man[i];
    }
    
    return result;
}

BinaryArray divide_ieee(const BinaryArray& x1, const BinaryArray& x2) {
    // Проверка деления на ноль
    bool x2_zero = true;
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        if (x2[i] != 0) {
            x2_zero = false;
            break;
        }
    }
    if (x2_zero) {
        throw runtime_error("Cannot divide by zero in IEEE-754");
    }
    
    // Проверка делимого на ноль
    bool x1_zero = true;
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        if (x1[i] != 0) {
            x1_zero = false;
            break;
        }
    }
    if (x1_zero) {
        BinaryArray result = {0};
        result[0] = x1[0] ^ x2[0];
        return result;
    }
    
    int sign = x1[0] ^ x2[0];
    
    // Получаем экспоненты
    int exp1 = 0, exp2 = 0;
    for (int i = 1; i < END_OF_EXP; i++) {
        exp1 = exp1 * 2 + x1[i];
        exp2 = exp2 * 2 + x2[i];
    }
    int final_exp = exp1 - exp2 + EXP_CONST;
    
    // Получаем мантиссы (с ведущей 1)
    vector<int> man1 = {1};
    vector<int> man2 = {1};
    for (int i = END_OF_EXP; i < NUMB_OF_BITS; i++) {
        man1.push_back(x1[i]);
        man2.push_back(x2[i]);
    }
    
    // Очищаем мантиссу делителя от ведущих нулей
    DynamicBinary man2_clean;
    for (int bit : man2) {
        man2_clean.push_back(bit);
    }
    man2_clean = clean_zeros(man2_clean);
    
    // Деление мантисс
    DynamicBinary integer_part;
    DynamicBinary frac_part;
    DynamicBinary current;
    
    // Целая часть
    for (int bit : man1) {
        current.push_back(bit);
        current = clean_zeros(current);
        
        if (is_greater_or_equal(current, man2_clean)) {
            integer_part.push_back(1);
            current = subtract_binary(current, man2_clean);
        } else {
            if (!integer_part.empty()) {
                integer_part.push_back(0);
            }
        }
    }
    
    if (integer_part.empty()) {
        integer_part.push_back(0);
    }
    
    // Дробная часть
    for (int i = 0; i < 25; i++) {
        current.push_back(0);
        current = clean_zeros(current);
        
        if (is_greater_or_equal(current, man2_clean)) {
            frac_part.push_back(1);
            current = subtract_binary(current, man2_clean);
        } else {
            frac_part.push_back(0);
        }
    }
    
    // Объединяем целую и дробную части
    DynamicBinary full_res;
    full_res.insert(full_res.end(), integer_part.begin(), integer_part.end());
    full_res.insert(full_res.end(), frac_part.begin(), frac_part.end());
    
    // Находим первую 1
    auto it = find(full_res.begin(), full_res.end(), 1);
    if (it == full_res.end()) {
        BinaryArray zero = {0};
        return zero;
    }
    
    int first_one_idx = it - full_res.begin();
    final_exp -= first_one_idx;
    
    // Получаем мантиссу (23 бита)
    vector<int> final_man;
    int start_idx = first_one_idx + 1;
    for (int i = start_idx; i < start_idx + LEN_OF_MANTISS && i < static_cast<int>(full_res.size()); i++) {
        final_man.push_back(full_res[i]);
    }
    
    while (final_man.size() < LEN_OF_MANTISS) {
        final_man.push_back(0);
    }
    
    // Получаем финальную экспоненту
    BinaryArray final_exp_bits = decimal_to_binary_8bit(final_exp);
    
    // Собираем результат
    BinaryArray result;
    result[0] = sign;
    for (int i = 0; i < 8; i++) {
        result[1 + i] = final_exp_bits[1 + i];
    }
    for (int i = 0; i < LEN_OF_MANTISS; i++) {
        result[END_OF_EXP + i] = final_man[i];
    }
    
    return result;
}
