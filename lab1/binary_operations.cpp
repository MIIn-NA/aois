#include "binary_operations.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

using namespace std;

BinaryArray decimal_to_binary(int x) {
    BinaryArray arr = {0};
    if (x < 0) {
        arr[0] = 1;
    }
    
    x = abs(x);
    int i = NUMB_OF_BITS - 1;
    
    while (x != 0 && i > 0) {
        arr[i] = x % 2;
        x /= 2;
        i--;
    }
    
    return arr;
}

int from_binary_to_decimal(const BinaryArray& bits) {
    int ans = 0;
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        ans = ans * 2 + bits[i];
    }
    
    if (bits[0] == 1) {
        ans *= -1;
    }
    
    return ans;
}

int unsigned_to_decimal(const vector<int>& bits) {
    int ans = 0;
    for (int bit : bits) {
        ans = ans * 2 + bit;
    }
    return ans;
}

BinaryArray decimal_to_ones_complement(int x) {
    BinaryArray arr = decimal_to_binary(x);
    if (arr[0] == 1) {
        for (int i = 1; i < NUMB_OF_BITS; i++) {
            arr[i] = 1 - arr[i];
        }
    }
    return arr;
}

BinaryArray twos_complement(const BinaryArray& arr) {
    if (arr[0] == 0) {
        return arr;
    }
    
    BinaryArray res = arr;
    
    // Инвертируем биты
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        res[i] = 1 - res[i];
    }
    
    // Добавляем 1
    for (int i = NUMB_OF_BITS - 1; i > 0; i--) {
        res[i] += 1;
        if (res[i] == 2) {
            res[i] = 0;
        } else {
            break;
        }
    }
    
    return res;
}

BinaryArray add_binary(int x1, int x2) {
    BinaryArray bin1 = twos_complement(decimal_to_binary(x1));
    BinaryArray bin2 = twos_complement(decimal_to_binary(x2));
    
    BinaryArray ans = {0};
    int carry = 0;
    
    for (int bit = NUMB_OF_BITS - 1; bit >= 0; bit--) {
        int total = bin1[bit] + bin2[bit] + carry;
        ans[bit] = total % 2;
        carry = total / 2;
    }
    
    return twos_complement(ans);
}

BinaryArray minus_binary(int x1, int x2) {
    return add_binary(x1, -x2);
}

BinaryArray multiply(int x1, int x2) {
    if (x1 == 0 || x2 == 0) {
        BinaryArray zero = {0};
        return zero;
    }
    
    BinaryArray bin1 = decimal_to_binary(x1);
    BinaryArray bin2 = decimal_to_binary(x2);
    
    BinaryArray ans = {0};
    ans[0] = bin1[0] ^ bin2[0];  // XOR для знака
    
    vector<int> temp_ans(NUMB_OF_BITS * 2, 0);
    
    for (int i = 31; i > 0; i--) {
        if (bin2[i] == 1) {
            int diff = 31 - i;
            for (int j = 31; j > 0; j--) {
                int pos = j - diff;
                if (pos > 0) {
                    temp_ans[pos] += bin1[j];
                }
            }
        }
    }
    
    // Обработка переносов
    for (int i = NUMB_OF_BITS - 1; i > 0; i--) {
        int carry = temp_ans[i] / 2;
        temp_ans[i] %= 2;
        if (i > 1) {
            temp_ans[i - 1] += carry;
        }
    }
    
    // Копируем результат в ans (биты с 1 по 31)
    for (int i = 1; i < NUMB_OF_BITS; i++) {
        ans[i] = temp_ans[i];
    }
    
    return ans;
}

DynamicBinary clean_zeros(const DynamicBinary& bin_array) {
    DynamicBinary res = bin_array;
    while (res.size() > 1 && res[0] == 0) {
        res.erase(res.begin());
    }
    return res;
}

bool is_greater_or_equal(const DynamicBinary& a, const DynamicBinary& b) {
    DynamicBinary a_clean = clean_zeros(a);
    DynamicBinary b_clean = clean_zeros(b);
    
    if (a_clean.size() > b_clean.size()) return true;
    if (a_clean.size() < b_clean.size()) return false;
    
    for (size_t i = 0; i < a_clean.size(); i++) {
        if (a_clean[i] > b_clean[i]) return true;
        if (a_clean[i] < b_clean[i]) return false;
    }
    
    return true;
}

DynamicBinary subtract_binary(const DynamicBinary& a, const DynamicBinary& b) {
    DynamicBinary a_clean = clean_zeros(a);
    DynamicBinary b_clean = clean_zeros(b);
    
    // Выравниваем длины
    while (b_clean.size() < a_clean.size()) {
        b_clean.insert(b_clean.begin(), 0);
    }
    
    DynamicBinary result;
    int borrow = 0;
    
    for (int i = a_clean.size() - 1; i >= 0; i--) {
        int diff = a_clean[i] - b_clean[i] - borrow;
        if (diff < 0) {
            diff += 2;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.insert(result.begin(), diff);
    }
    
    return clean_zeros(result);
}

tuple<int, DynamicBinary, DynamicBinary> divide(int x1, int x2) {
    if (x2 == 0) {
        throw runtime_error("Cannot divide by zero");
    }
    
    BinaryArray x1_bin = decimal_to_binary(x1);
    BinaryArray x2_bin = decimal_to_binary(x2);
    
    // Знак результата - XOR знаков
    int sign = x1_bin[0] ^ x2_bin[0];
    
    // Берем модули чисел (убираем знаковый бит)
    DynamicBinary x1_mag(x1_bin.begin() + 1, x1_bin.end());
    DynamicBinary x2_mag(x2_bin.begin() + 1, x2_bin.end());
    
    // Удаляем ведущие нули
    x1_mag = clean_zeros(x1_mag);
    x2_mag = clean_zeros(x2_mag);
    
    DynamicBinary integer_part;
    DynamicBinary float_part;
    DynamicBinary remainder;
    
    // Особый случай: деление на 1
    if (x2_mag.size() == 1 && x2_mag[0] == 1) {
        integer_part = x1_mag;
        if (integer_part.empty()) integer_part = {0};
        
        // Дробная часть - все нули
        for (int i = 0; i < 17; i++) {
            float_part.push_back(0);
        }
        
        return make_tuple(sign, integer_part, float_part);
    }
    
    // Особый случай: делимое равно 0
    if (x1_mag.size() == 1 && x1_mag[0] == 0) {
        integer_part = {0};
        for (int i = 0; i < 17; i++) {
            float_part.push_back(0);
        }
        return make_tuple(sign, integer_part, float_part);
    }
    
    // Случай 1: делимое меньше делителя
    if (!is_greater_or_equal(x1_mag, x2_mag)) {
        integer_part = {0};  // Целая часть = 0
        remainder = x1_mag;   // Остаток = делимое
        
        // Вычисляем дробную часть (17 бит точности)
        for (int i = 0; i < 17; i++) {
            if (remainder.size() == 1 && remainder[0] == 0) {
                float_part.push_back(0);
                continue;
            }
            
            // Умножаем остаток на 2 (добавляем 0 в конец)
            remainder.push_back(0);
            remainder = clean_zeros(remainder);
            
            if (is_greater_or_equal(remainder, x2_mag)) {
                float_part.push_back(1);
                remainder = subtract_binary(remainder, x2_mag);
            } else {
                float_part.push_back(0);
            }
        }
        
        return make_tuple(sign, integer_part, float_part);
    }
    
    // Случай 2: обычное деление
    // Целая часть
    for (int bit : x1_mag) {
        remainder.push_back(bit);
        remainder = clean_zeros(remainder);
        
        if (is_greater_or_equal(remainder, x2_mag)) {
            integer_part.push_back(1);
            remainder = subtract_binary(remainder, x2_mag);
        } else {
            if (!integer_part.empty()) {
                integer_part.push_back(0);
            }
        }
    }
    
    if (integer_part.empty()) {
        integer_part.push_back(0);
    }
    
    // Дробная часть (17 бит точности)
    for (int i = 0; i < 17; i++) {
        if (remainder.size() == 1 && remainder[0] == 0) {
            float_part.push_back(0);
            continue;
        }
        
        // Умножаем остаток на 2
        remainder.push_back(0);
        remainder = clean_zeros(remainder);
        
        if (is_greater_or_equal(remainder, x2_mag)) {
            float_part.push_back(1);
            remainder = subtract_binary(remainder, x2_mag);
        } else {
            float_part.push_back(0);
        }
    }
    
    return make_tuple(sign, integer_part, float_part);
}

string bin_to_string(const vector<int>& bits) {
    string result;
    for (int bit : bits) {
        result += to_string(bit);
    }
    return result;
}

string bin_to_string(const BinaryArray& bits) {
    string result;
    for (int i = 0; i < NUMB_OF_BITS; i++) {
        result += to_string(bits[i]);
    }
    return result;
}
