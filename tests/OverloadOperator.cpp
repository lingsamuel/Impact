#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace std;
class Complex {
public:
    Complex() {
        real = 0;
        imag = 0;
    }

    Complex(int r, int i) {
        real = r;
        imag = i;
    }

    Complex operator+(Complex const &c2) {
        return Complex(real + c2.real, imag + c2.imag);
    };

    Complex operator+(const int &i) {
        return Complex(real += i, imag += i);
    }

    friend Complex operator+(const int &, const Complex& );

    void display() {
        cout << real << " " << imag << endl;
    };

private:
    int real;
    int imag;
};

Complex operator+(const int &i, const Complex& c) {
    return Complex(c.real + i ,c.imag+ i);
}


int main() {
    Complex a(1,1);
    Complex b(2,2);
    a.display();
    b.display();
    Complex c = a + b;
    c.display();
    a.display();
    a = 1 + a;
    // wow! int + obj!
    a.display();
    b = b + 1;
    b.display();

}
