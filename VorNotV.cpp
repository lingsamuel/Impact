//
// Created by lingsamuel on 16-5-12.
//
#include <iostream>

using std::cout;
using std::endl;

class A {
public:
    int sb() {
        cout << "A::sb" << endl;
    };

    virtual int sb2() {
        cout << "A::sb2" << endl;
    };
};

class B1 : public A {
public:
    int sb() {
        cout << "b::sb" << endl;
    };

    virtual int sb2() {
        cout << "b::sb2" << endl;
    };
};
class C{
public:
    int sb(){
        cout << "eyhl";
    }
};
int main() {
    A a;
    B1 b;
    A a2 = b;
    a.sb();
    a.sb2();
    b.B::sb();
    b.sb2();
    a2 .sb();
    a2.sb2();

    A* pa = new A();
    B1* pb = new B1;
    A* pa2 = pb;
    pa->sb();
    pa->sb2();
    pb->sb();
    pb->sb2();
    pa2->sb();

    pa2->sb2();




}