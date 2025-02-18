#include <iostream>
#include <vector>
#include <limits>
#include <cmath>

using namespace std;

class Complex {
private:
    double real;
    double imag;

public:
    // Конструктори
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // Методи доступу
    double getReal() const { return real; }
    double getImag() const { return imag; }
    void setReal(double r) { real = r; }
    void setImag(double i) { imag = i; }

    // Операції
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    
    Complex operator*(const Complex& other) const {
        return Complex(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
    }
    
    Complex operator/(const Complex& other) const {
        double denominator = other.real * other.real + other.imag * other.imag;
        return Complex((real * other.real + imag * other.imag) / denominator,
                       (imag * other.real - real * other.imag) / denominator);
    }
    
    // Модуль комплексного числа
    double magnitude() const {
        return std::sqrt(real * real + imag * imag);
    }
    
    // Вивід у потік
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        os << c.real << (c.imag >= 0 ? " + " : " - ") << std::abs(c.imag) << "i";
        return os;
    }
};

// Функція для знаходження оптимального бінарного дерева пошуку
double optimalBST(const vector<Complex>& keys, const vector<double>& freq) {
    int n = keys.size();
    vector<vector<double> > cost(n, vector<double>(n, 0));
    vector<vector<double> > sumFreq(n, vector<double>(n, 0));

    // Заповнюємо діагональну частину матриці вартістю одиничних вузлів
    for (int i = 0; i < n; i++) {
        cost[i][i] = freq[i];
        sumFreq[i][i] = freq[i];
    }

    // Заповнюємо таблицю динамічного програмування
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            cost[i][j] = numeric_limits<double>::infinity();
            sumFreq[i][j] = sumFreq[i][j - 1] + freq[j];

            for (int r = i; r <= j; r++) {
                double leftCost = (r > i) ? cost[i][r - 1] : 0;
                double rightCost = (r < j) ? cost[r + 1][j] : 0;
                double c = leftCost + rightCost + sumFreq[i][j];
                
                if (c < cost[i][j]) {
                    cost[i][j] = c;
                }
            }
        }
    }

    return cost[0][n - 1];
}

int main() {
    vector<Complex> keys;
    keys.push_back(Complex(10, 1));
    keys.push_back(Complex(20, -2));
    keys.push_back(Complex(30, 3));
    keys.push_back(Complex(40, -4));

    vector<double> freq;
    freq.push_back(3);
    freq.push_back(3);
    freq.push_back(1);
    freq.push_back(1);
    
    cout << "Мінімальна вартість оптимального BST: " << optimalBST(keys, freq) << endl;
    return 0;
}
