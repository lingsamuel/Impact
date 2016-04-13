//
// Created by lingsamuel on 16-4-11.
//
#include <iostream>

using namespace std;

class X {
public:
    X() {
        cout << "default\n";
    }

    X(int i) {
        cout << "param\n";
    }

    X(const X &) {
        cout << "copy\n";
    }

    X(X &&) {
        cout << "move\n";
    }
    X &operator=(const X &)  &&{ cout << "copy assignment rvalue\n";return *this; }
    X &operator=(X &&)       &&{ cout << "move assignment rvalue\n";return *this; }
    X &operator=(const X &)  &{ cout << "copy assignment lvalue\n";return *this; }
    X &operator=(X &&)       &{ cout << "move assignment lvalue\n";return *this; }
};

X f5() {
    return X();
}

int main() {
    f5() = X(1);
    cout << "___\n";
    f5() = X();
    cout << "___\n";
    f5() = X(f5());
}
