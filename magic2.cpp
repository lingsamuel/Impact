#include <iostream>

using namespace std;

class rvalue {
public:
    class IVisitor {
    public:
        virtual void visit(Derived1 &) = 0;

        virtual void visit(Derived2 &) = 0;

        virtual ~IVisitor() { }
    };

    virtual void accept(IVisitor &visitor) = 0;
};

class Derived1 : public rvalue {
public:
    virtual __void_t accept(IVisitor &visitor) override {
        (&visitor)->visit(*this);
    }
};

class Derived2 : public rvalue {
public:
    virtual __void_t accept(IVisitor &visitor) override {
        (&visitor)->visit(*this);
    }
};

class Fuck : public rvalue::IVisitor {
public:
    virtual void visit(Derived1 &) override {
        cout << "fuck1";
    }

    virtual __void_t visit(Derived2 &) override {
        cout << "fuck2";
    }
};

int main() {
    rvalue *r1 = new Derived1();
    rvalue *r2 = new Derived2();
    Fuck fuck;
    r1->accept(fuck);
    r2->accept(fuck);
}
