#include <iostream>
#include <type_traits>

using namespace std;

class BigNum {
private:
    string value;
public:
    BigNum( string &str): value(str) {

    }

    BigNum( string &&str): value(str) {

    }

    BigNum( const char* chars): value(chars) {

    }

    friend ostream &operator<<(ostream &os, BigNum &a);
    friend ostream &operator<<(ostream &os, BigNum &&a);
    friend BigNum mul( BigNum &&a, BigNum &&b);
    friend BigNum operator*( BigNum &&a, BigNum &&b);
};


BigNum operator"" _l ( const char * lit, size_t size) {
    return BigNum(lit);
}

ostream &operator<<(ostream &os, BigNum &a) {
    return (os << a.value);
}

ostream &operator<<(ostream &os, BigNum &&a) {
    return (os << a.value);
}

BigNum mul( BigNum &&a, BigNum &&b) {

    return forward<BigNum>(a.value+b.value);
}

BigNum operator*( BigNum &&a, BigNum &&b) {
    return forward<BigNum>(a.value+b.value);
//    return move<BigNum>(a.value+b.value);
//    return a.value+b.value;
}

int main();
//int main () {
//auto main() {
auto main() -> decltype(main()) {
//auto main() -> int {
    cout << "fuck shabi" << endl;

    auto c = mul("321", "7432");
    cout << c << endl;

    auto d = "32131"_l * "0932"_l;
    cout << d << endl;

    cout << "32131"_l * "0932"_l << endl;



    return 0;
}