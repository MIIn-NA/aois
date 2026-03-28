#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include "binary_operations.h"
#include "ieee754.h"
#include "bcd2421.h"

using namespace std;

void print_binary(const string& prefix, const BinaryArray& bits) {
    cout << prefix << bin_to_string(bits) << endl;
}

void print_binary(const string& prefix, const vector<int>& bits) {
    cout << prefix << bin_to_string(bits) << endl;
}

void print_bcd(const string& prefix, const BCDNumber& bcd) {
    cout << prefix << bcd_to_string(bcd) << endl;
}

int main() {
    int choice;
    
    while (true) {
        cout << "\n" << string(50, '=') << endl;
        cout << " ОПЕРАЦИИ " << endl;
        cout << string(50, '=') << endl;
        cout << "1. Перевод в прямой, обратный и дополнительный коды" << endl;
        cout << "2. Сложение 2 чисел в дополнительном коде" << endl;
        cout << "3. Вычитание 2 чисел в дополнительном коде" << endl;
        cout << "4. Умножение 2 чисел в прямом коде" << endl;
        cout << "5. Деление 2 чисел в прямом коде (до 5 знаков)" << endl;
        cout << "6. Операции с плавающей точкой (IEEE-754)" << endl;
        cout << "7. Сложение в коде 2421 BCD" << endl;
        cout << "0. Выход" << endl;
        cout << string(50, '-') << endl;
        
        cout << "Выберите пункт меню: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Неверный ввод. Пожалуйста, введите число." << endl;
            continue;
        }
        
        if (choice == 0) {
            cout << "Завершение работы." << endl;
            break;
        }
        
        else if (choice == 1) {
            int num;
            cout << "Введите десятичное число: ";
            cin >> num;
            
            BinaryArray direct = decimal_to_binary(num);
            BinaryArray ones = decimal_to_ones_complement(num);
            BinaryArray twos = twos_complement(direct);
            
            cout << "Прямой код:         " << bin_to_string(direct) << endl;
            cout << "Обратный код:       " << bin_to_string(ones) << endl;
            cout << "Дополнительный код: " << bin_to_string(twos) << endl;
        }
        
        else if (choice == 2) {
            int x1, x2;
            cout << "Первое число: ";
            cin >> x1;
            cout << "Второе число: ";
            cin >> x2;
            
            BinaryArray res_bin = add_binary(x1, x2);
            int res_dec = from_binary_to_decimal(res_bin);
            
            cout << "Результат (2-ый):  " << bin_to_string(res_bin) << endl;
            cout << "Результат (10-ый): " << res_dec << endl;
        }
        
        else if (choice == 3) {
            int x1, x2;
            cout << "Уменьшаемое: ";
            cin >> x1;
            cout << "Вычитаемое: ";
            cin >> x2;
            
            BinaryArray res_bin = minus_binary(x1, x2);
            int res_dec = from_binary_to_decimal(res_bin);
            
            cout << "Результат (2-ый):  " << bin_to_string(res_bin) << endl;
            cout << "Результат (10-ый): " << res_dec << endl;
        }
        
        else if (choice == 4) {
            int x1, x2;
            cout << "Первое число: ";
            cin >> x1;
            cout << "Второе число: ";
            cin >> x2;
            
            BinaryArray res_bin = multiply(x1, x2);
            int res_dec = from_binary_to_decimal(res_bin);
            
            cout << "Результат (2-ый):  " << bin_to_string(res_bin) << endl;
            cout << "Результат (10-ый): " << res_dec << endl;
        }
        
        else if (choice == 5) {
            int x1, x2;
            cout << "Делимое: ";
            cin >> x1;
            cout << "Делитель: ";
            cin >> x2;
            
            try {
                auto [sign, int_part, float_part] = divide(x1, x2);
                
                // Вывод в двоичном формате
                cout << "Результат (2-ый):  ";
                if (sign == 1) cout << "-";
                cout << bin_to_string(int_part) << ".";
                
                // Выводим только 5 знаков после запятой
                for (int i = 0; i < 5 && i < static_cast<int>(float_part.size()); i++) {
                    cout << float_part[i];
                }
                cout << endl;
                
                // Перевод в десятичный
                int int_val = 0;
                for (int b : int_part) {
                    int_val = int_val * 2 + b;
                }
                
                double float_val = 0.0;
                double power = 0.5;
                for (size_t i = 0; i < float_part.size() && i < 17; i++) {
                    if (float_part[i] == 1) {
                        float_val += power;
                    }
                    power /= 2.0;
                }
                
                double dec_val = int_val + float_val;
                if (sign == 1) {
                    dec_val = -dec_val;
                }
                
                cout << fixed << setprecision(5) << "Результат (10-ый): " << dec_val << endl;
            }
            catch (const runtime_error& e) {
                cout << "Ошибка: " << e.what() << endl;
            }
        }
        
        else if (choice == 6) {
            float f1, f2;
            char op;
            
            cout << "Первое число (float): ";
            cin >> f1;
            cout << "Второе число (float): ";
            cin >> f2;
            cout << "Выберите операцию (+, -, *, /): ";
            cin >> op;
            
            BinaryArray bin1 = float_to_ieee_bin(f1);
            BinaryArray bin2 = float_to_ieee_bin(f2);
            
            BinaryArray res_bin;
            
            try {
                if (op == '+') {
                    res_bin = add_ieee(bin1, bin2);
                }
                else if (op == '-') {
                    BinaryArray bin2_neg = bin2;
                    bin2_neg[0] = 1 - bin2_neg[0];
                    res_bin = add_ieee(bin1, bin2_neg);
                }
                else if (op == '*') {
                    res_bin = multiply_ieee(bin1, bin2);
                }
                else if (op == '/') {
                    res_bin = divide_ieee(bin1, bin2);
                }
                else {
                    cout << "Неизвестная операция." << endl;
                    continue;
                }
                
                float res_dec = ieee_bin_to_float(res_bin);
                
                cout << "IEEE-754 (2-ый):   " << bin_to_string(res_bin) << endl;
                cout << "Результат (10-ый): " << res_dec << endl;
            }
            catch (const runtime_error& e) {
                cout << "Ошибка: " << e.what() << endl;
            }
        }
        
        else if (choice == 7) {
            int x1, x2;
            cout << "Первое положительное число: ";
            cin >> x1;
            cout << "Второе положительное число: ";
            cin >> x2;
            
            if (x1 < 0 || x2 < 0) {
                cout << "Числа должны быть положительными для BCD операций." << endl;
                continue;
            }
            
            BCDNumber arr1 = int_to_2421_array(x1);
            BCDNumber arr2 = int_to_2421_array(x2);
            
            BCDNumber res_arr = add_2421_numbers(arr1, arr2);
            int res_dec = array_2421_to_int(res_arr);
            
            cout << "Результат (2-ый 2421): ";
            for (const auto& tetrad : res_arr) {
                cout << tetrad_to_string(tetrad) << " ";
            }
            cout << endl;
            cout << "Результат (10-ый):     " << res_dec << endl;
        }
        
        else {
            cout << "Неверный ввод. Пожалуйста, выберите число от 0 до 7." << endl;
        }
    }
    
    return 0;
}
