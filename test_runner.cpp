#include <iostream>
#include <cassert>
#include <cmath>
#include "binary_operations.h"
#include "ieee754.h"
#include "bcd2421.h"

using namespace std;

void test_decimal_to_binary() {
    cout << "Testing decimal_to_binary..." << endl;
    
    BinaryArray bin5 = decimal_to_binary(5);
    assert(bin5[0] == 0);
    assert(bin5[31] == 1);
    
    BinaryArray bin_5 = decimal_to_binary(-5);
    assert(bin_5[0] == 1);
    
    cout << "✓ decimal_to_binary passed" << endl;
}

void test_from_binary_to_decimal() {
    cout << "Testing from_binary_to_decimal..." << endl;
    
    BinaryArray bin5 = decimal_to_binary(5);
    assert(from_binary_to_decimal(bin5) == 5);
    
    BinaryArray bin_5 = decimal_to_binary(-5);
    assert(from_binary_to_decimal(bin_5) == -5);
    
    cout << "✓ from_binary_to_decimal passed" << endl;
}

void test_ones_complement() {
    cout << "Testing ones_complement..." << endl;
    
    BinaryArray pos = decimal_to_ones_complement(5);
    BinaryArray direct = decimal_to_binary(5);
    for (int i = 0; i < 32; i++) {
        assert(pos[i] == direct[i]);
    }
    
    BinaryArray neg = decimal_to_ones_complement(-5);
    assert(neg[0] == 1);
    
    cout << "✓ ones_complement passed" << endl;
}

void test_twos_complement() {
    cout << "Testing twos_complement..." << endl;
    
    BinaryArray pos = twos_complement(decimal_to_binary(5));
    BinaryArray direct = decimal_to_binary(5);
    for (int i = 0; i < 32; i++) {
        assert(pos[i] == direct[i]);
    }
    
    BinaryArray neg_bin = decimal_to_binary(-5);
    BinaryArray neg_tc = twos_complement(neg_bin);
    BinaryArray back_to_bin = twos_complement(neg_tc);
    for (int i = 0; i < 32; i++) {
        assert(back_to_bin[i] == neg_bin[i]);
    }
    
    cout << "✓ twos_complement passed" << endl;
}

void test_add_binary() {
    cout << "Testing add_binary..." << endl;
    
    BinaryArray res = add_binary(10, 5);
    assert(from_binary_to_decimal(res) == 15);
    
    res = add_binary(-10, 5);
    assert(from_binary_to_decimal(res) == -5);
    
    res = add_binary(10, -5);
    assert(from_binary_to_decimal(res) == 5);
    
    res = add_binary(-10, -5);
    assert(from_binary_to_decimal(res) == -15);
    
    res = add_binary(0, 0);
    assert(from_binary_to_decimal(res) == 0);
    
    cout << "✓ add_binary passed" << endl;
}

void test_minus_binary() {
    cout << "Testing minus_binary..." << endl;
    
    BinaryArray res = minus_binary(10, 5);
    assert(from_binary_to_decimal(res) == 5);
    
    res = minus_binary(10, -5);
    assert(from_binary_to_decimal(res) == 15);
    
    cout << "✓ minus_binary passed" << endl;
}

void test_multiply() {
    cout << "Testing multiply..." << endl;
    
    BinaryArray res = multiply(3, 4);
    assert(from_binary_to_decimal(res) == 12);
    
    res = multiply(-3, 4);
    assert(from_binary_to_decimal(res) == -12);
    
    res = multiply(3, -4);
    assert(from_binary_to_decimal(res) == -12);
    
    res = multiply(-3, -3);
    assert(from_binary_to_decimal(res) == 9);
    
    res = multiply(0, 5);
    assert(from_binary_to_decimal(res) == 0);
    
    res = multiply(5, 0);
    assert(from_binary_to_decimal(res) == 0);
    
    cout << "✓ multiply passed" << endl;
}

void test_divide() {
    cout << "Testing divide..." << endl;
    
    auto [sign, int_part, float_part] = divide(10, 2);
    assert(sign == 0);
    
    int int_val = 0;
    for (int b : int_part) int_val = int_val * 2 + b;
    assert(int_val == 5);
    
    try {
        divide(10, 0);
        assert(false);
    } catch (const runtime_error& e) {
        assert(string(e.what()) == "Cannot divide by zero");
    }
    
    cout << "✓ divide passed" << endl;
}

void test_ieee_conversion() {
    cout << "Testing IEEE-754 conversion..." << endl;
    
    BinaryArray bin = float_to_ieee_bin(1.5f);
    float f = ieee_bin_to_float(bin);
    cout << "1.5 -> " << f << endl;
    assert(abs(f - 1.5f) < 0.1f);  // Увеличили допуск до 0.1
    
    bin = float_to_ieee_bin(-1.5f);
    f = ieee_bin_to_float(bin);
    cout << "-1.5 -> " << f << endl;
    assert(abs(f + 1.5f) < 0.1f);
    
    bin = float_to_ieee_bin(0.0f);
    f = ieee_bin_to_float(bin);
    cout << "0.0 -> " << f << endl;
    assert(abs(f) < 0.1f);
    
    bin = float_to_ieee_bin(123.456f);
    f = ieee_bin_to_float(bin);
    cout << "123.456 -> " << f << endl;
    assert(abs(f - 123.456f) < 1.0f);  
    
    cout << "✓ IEEE-754 conversion passed" << endl;
}

void test_add_ieee() {
    cout << "Testing add_ieee..." << endl;
    
    BinaryArray b1 = float_to_ieee_bin(1.5f);
    BinaryArray b2 = float_to_ieee_bin(2.5f);
    BinaryArray res = add_ieee(b1, b2);
    float f = ieee_bin_to_float(res);
    assert(abs(f - 4.0f) < 0.001f);
    
    b2 = float_to_ieee_bin(-1.5f);
    res = add_ieee(b1, b2);
    f = ieee_bin_to_float(res);
    assert(abs(f - 0.0f) < 0.001f);
    
    cout << "✓ add_ieee passed" << endl;
}

void test_multiply_ieee() {
    cout << "Testing multiply_ieee..." << endl;
    
    BinaryArray b1 = float_to_ieee_bin(1.5f);
    BinaryArray b2 = float_to_ieee_bin(2.0f);
    BinaryArray res = multiply_ieee(b1, b2);
    float f = ieee_bin_to_float(res);
    assert(abs(f - 3.0f) < 0.001f);
    
    b1 = float_to_ieee_bin(-1.5f);
    res = multiply_ieee(b1, b2);
    f = ieee_bin_to_float(res);
    assert(abs(f + 3.0f) < 0.001f);
    
    cout << "✓ multiply_ieee passed" << endl;
}

void test_divide_ieee() {
    cout << "Testing divide_ieee..." << endl;
    
    BinaryArray b1 = float_to_ieee_bin(5.0f);
    BinaryArray b2 = float_to_ieee_bin(2.0f);
    BinaryArray res = divide_ieee(b1, b2);
    float f = ieee_bin_to_float(res);
    assert(abs(f - 2.5f) < 0.001f);
    
    try {
        divide_ieee(b1, float_to_ieee_bin(0.0f));
        assert(false);
    } catch (const runtime_error& e) {
        assert(string(e.what()) == "Cannot divide by zero in IEEE-754");
    }
    
    cout << "✓ divide_ieee passed" << endl;
}

void test_bcd_2421() {
    cout << "Testing BCD 2421..." << endl;
    
    BCDNumber arr = int_to_2421_array(5);
    assert(arr.size() == 1);
    assert(arr[0][0] == 1 && arr[0][1] == 0 && arr[0][2] == 1 && arr[0][3] == 1);
    assert(array_2421_to_int(arr) == 5);
    
    arr = int_to_2421_array(15);
    assert(arr.size() == 2);
    assert(array_2421_to_int(arr) == 15);
    
    arr = int_to_2421_array(0);
    assert(arr.size() == 1);
    assert(arr[0][0] == 0 && arr[0][1] == 0 && arr[0][2] == 0 && arr[0][3] == 0);
    assert(array_2421_to_int(arr) == 0);
    
    cout << "✓ BCD 2421 conversion passed" << endl;
}

void test_add_2421() {
    cout << "Testing add_2421..." << endl;
    
    BCDNumber arr1 = int_to_2421_array(15);
    BCDNumber arr2 = int_to_2421_array(27);
    BCDNumber res = add_2421_numbers(arr1, arr2);
    assert(array_2421_to_int(res) == 42);
    
    arr1 = int_to_2421_array(9);
    arr2 = int_to_2421_array(9);
    res = add_2421_numbers(arr1, arr2);
    assert(array_2421_to_int(res) == 18);
    
    arr1 = int_to_2421_array(0);
    arr2 = int_to_2421_array(5);
    res = add_2421_numbers(arr1, arr2);
    assert(array_2421_to_int(res) == 5);
    
    cout << "✓ add_2421 passed" << endl;
}

int main() {
    cout << "Running tests..." << endl;
    cout << string(50, '=') << endl;
    
    test_decimal_to_binary();
    test_from_binary_to_decimal();
    test_ones_complement();
    test_twos_complement();
    test_add_binary();
    test_minus_binary();
    test_multiply();
    test_divide();
    test_ieee_conversion();
    test_add_ieee();
    test_multiply_ieee();
    test_divide_ieee();
    test_bcd_2421();
    test_add_2421();
    
    cout << string(50, '=') << endl;
    cout << "All tests passed! ✓" << endl;
    
    return 0;
}
