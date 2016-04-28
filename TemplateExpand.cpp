#include <type_traits>
#include <string>

template<typename Left, typename Right>
struct ConcatExpr;

template<char ch>
struct MatchExpr;

template<typename RegExpr>
struct MatchImpl;


template<typename RegExpr>
struct AccurExpr;

template<typename RegExpr>
struct AlterExpr;

struct nil {
};

template<typename First, typename ...Rest>
struct List {
    using first = First;
    using rest = List<Rest...>;
    static constexpr int len = 1 + rest::len;
};

template<typename First>
struct List<First> {
    using first = First;
    using rest = nil;
    static constexpr int len = 1;
};

template<char first, char ...ch>
struct CharList {
    using rest = CharList<ch...>;
    static constexpr char value = first;
    static constexpr int len = 1 + rest::len;
    static bool Apply(const char *target) {
        return *target && *target == first;
    }
};

template<char only>
struct CharList<only> {
    using rest = nil;
    static constexpr char value = only;
    static constexpr int len = 1;
    static bool Apply(const char *target) {
        return *target && *target == only;
    }
};

template <typename T>
class A{

};

template <>
class A<int>{

};

template <typename T>
struct MatchImplement {
    static bool Apply(const char *target) {
        return false;
    }
};

template<char ...charlist>
struct MatchImplement<AlterExpr<CharList<charlist...>>> {
    using regexRest = typename CharList<charlist ...>::rest;
    static bool Apply(const char *target) {
        return   *target && *target == CharList<charlist ...>::value
                 || MatchImplement<AlterExpr<typename CharList<charlist ...>::rest>>::Apply(target);
    }
};

template<char ...charlist>
struct MatchImplement<AccurExpr<CharList<charlist...>>> {
    using regexRest = typename CharList<charlist ...>::rest;
    static bool Apply(const char *target) {
        return   *target && *target == CharList<charlist ...>::value
                 && MatchImplement<AccurExpr<typename CharList<charlist ...>::rest>>::Apply(target + 1);
    }
};

template<>
struct MatchImplement<AlterExpr<nil>> {
    static bool Apply(const char *target) {
        return false;
    }
};

template<>
struct MatchImplement<AccurExpr<nil>> {
    static bool Apply(const char *target) {
        return true;
    }
};

template<typename RegexExpr>
bool RegexMatch (const char *target){
    return (MatchImplement<typename RegexExpr::value>::Apply(target));
}


#include <cassert>
#include <iostream>

using namespace std;

void listTest() {
//    assert((List<int,int,int>::len == 3));
    cout << "List Test: " << (List<int, int, int>::len == 3 ? "OK" : "ERR") << endl;
}

void accurTest() {

    cout << "Accuracy Pattern Test: "
    << (MatchImplement<AccurExpr<CharList<'a', 'b'>>>::Apply("ab") ? "OK" : "ERR") << endl;
    cout << "                       "
    << (MatchImplement<AccurExpr<CharList<'b', 'b'>>>::Apply("ab") ? "ERR (This Test Should be False)" : "OK") << endl;
}

void alterTest() {

    cout << "Alternative Pattern Test: "
    << (MatchImplement<AlterExpr<CharList<'a', 'b'>>>::Apply("a") ? "OK" : "ERR") << endl;
    cout << "                          "
    << (MatchImplement<AlterExpr<CharList<'b', 'a'>>>::Apply("cc") ? "ERR (This Test Should be False)" : "OK") << endl;
}

int main() {
    listTest();
    accurTest();
    alterTest();
    return 0;
}
