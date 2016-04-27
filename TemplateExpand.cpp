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
//    using rest = Match<ch...>;
//    static bool Apply(const char *target) {
//        return *target && *target == first && rest::Apply(target+1);
//    }
};

template<char only>
struct CharList<only> {
    using rest = nil;
    static constexpr char value = only;
    static constexpr int len = 1;
//    static bool Apply(const char *target) {
//        return *target && *target == only;
//    }
};

template <typename T>
class A{

};

template <>
class A<int>{

};


//template <typename T>
//struct MatchImplement {
//
//};


template<typename Charlist>
struct MatchImplement {
    using regexRest = typename Charlist::rest;
    static bool Apply(const char *target) {
        return   *target && *target == Charlist::value && MatchImplement<typename Charlist::rest>::Apply(target + 1);
    }
};

template<>
struct MatchImplement<nil> {
    static bool Apply(const char *target) {
        return true;
    }
};

template<typename RegexExpr>
bool RegexMatch (const char *target){
    return (MatchImplement<RegexExpr>::Apply(target));
}


#include <cassert>
#include <iostream>

using namespace std;

void listTest() {
//    assert((List<int,int,int>::len == 3));
    cout << "List Test: " << (List<int, int, int>::len == 3 ? "OK" : "ERR") << endl;
}

void charListTest() {

    cout << "Char List Test: " << (MatchImplement<CharList<'a', 'b'>>::Apply("ab") ? "OK" : "ERR") << endl;

    cout << "                " << (MatchImplement<CharList<'b', 'b'>>::Apply("ab") ? "ERR (This Test Should be False)" : "OK") << endl;
}

int main() {
    listTest();
    charListTest();
    return 0;
}
