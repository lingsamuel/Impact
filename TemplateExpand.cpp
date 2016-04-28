#include <type_traits>
#include <string>

template<typename Left, typename Right>
class ConcatExpr;

template<char ch>
class MatchExpr;

template<typename RegExpr>
class MatchImpl {
public:
    using expr = RegExpr;
};


template<typename RegExpr>
class AccurExpr {
public:
    using expr = RegExpr;
};

template<typename RegExpr>
class AlterExpr {
public:
    using expr = RegExpr;
};

class nil {
};

template<typename First, typename ...Rest>
class List {
public:
    using first = First;
    using rest = List<Rest...>;
    static constexpr int len = 1 + rest::len;
};

template<typename First>
class List<First> {
public:
    using first = First;
    using rest = nil;
    static constexpr int len = 1;
};

template<char first, char ...ch>
class CharList {
public:
    using rest = CharList<ch...>;
    static constexpr char value = first;
    static constexpr int len = 1 + rest::len;

    static bool Apply(const char *target) {
        return *target && *target == value;
    }
};

template<char only>
class CharList<only> {
public:
    using rest = nil;
    static constexpr char value = only;
    static constexpr int len = 1;

    static bool Apply(const char *target) {
        return *target && *target == value;
    }
};


template<typename T>
class A {

};

template<>
class A<int> {

};


template<typename T, typename ...Remain>
class MatchImplement {
public:
    static constexpr int firstPatternLen = T::expr::len;
    // MatchImplement<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>::Apply()
//    template <typename Next>
    static bool Apply(const char *target) {
        return MatchImplement<T>::Apply(target) && MatchImplement<Remain...>::Apply(target+firstPatternLen);
    }
};

template<typename T>
class MatchImplement<T> {
public:
//    template <typename Next>
    static bool Apply(const char *target) {
        return MatchImplement<T>::Apply(target);
    }
};

template<char ...charlist>
class MatchImplement<AlterExpr<CharList<charlist...>>> {
public:
    using regexRest = typename CharList<charlist ...>::rest;

    static bool Apply(const char *target) {
        return CharList<charlist ...>::Apply(target)
               || MatchImplement<AlterExpr<typename CharList<charlist ...>::rest>>::Apply(target);
    }
};

template<char ...charlist>
class MatchImplement<AccurExpr<CharList<charlist...>>> {
public:
    using regexRest = typename CharList<charlist ...>::rest;

    static bool Apply(const char *target) {
        return CharList<charlist ...>::Apply(target)
               && MatchImplement<AccurExpr<typename CharList<charlist ...>::rest>>::Apply(target + 1);
    }
};

template<>
class MatchImplement<AlterExpr<nil>> {
public:
    static bool Apply(const char *target) {
        return false;
    }
};

template<>
class MatchImplement<AccurExpr<nil>> {
public:
    static bool Apply(const char *target) {
        return true;
    }
};

template<typename RegexExpr>
bool RegexMatch(const char *target) {
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
    constexpr const char * space = "                       ";
    cout << "Accuracy Pattern Test: "
    << (MatchImplement<AccurExpr<CharList<'a', 'b'>>>::Apply("ab") ? "OK" : "ERR") << endl;
    cout << space
    << (MatchImplement<AccurExpr<CharList<'b', 'b'>>>::Apply("ab") ? "ERR (This Test Should be False)" : "OK") << endl;
}

void alterTest() {
    constexpr const char * space = "                          ";
    cout << "Alternative Pattern Test: "
    << (MatchImplement<AlterExpr<CharList<'a', 'b'>>>::Apply("a") ? "OK" : "ERR") << endl;
    cout << space
    << (MatchImplement<AlterExpr<CharList<'a', 'b'>>>::Apply("a") ? "OK" : "ERR") << endl;
    cout << space
    << (MatchImplement<AlterExpr<CharList<'b', 'a'>>>::Apply("cc") ? "ERR (This Test Should be False)" : "OK") << endl;
}

void alterAccurTest() {
    constexpr const char * space = "                    ";
    cout << "Cross Pattern Test: "
    << (MatchImplement<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>::Apply("bba") ? "OK" : "ERR") <<
    endl;
    cout << space
    << (MatchImplement<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>::Apply("bbb") ? "OK" : "ERR") <<
    endl;
    cout << space
    << (MatchImplement<AccurExpr<CharList<'a', 'b'>>, AlterExpr<CharList<'a', 'c'>>>::Apply("bbc")
        ? "ERR (This Test Should be False)" : "OK") << endl;
    cout << space
    << (MatchImplement<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>::Apply("bbe")
        // NOTE: This test set one of Alter char to 'b' in case Alter match first char of Apply.
        ? "ERR (This Test Should be False)" : "OK") << endl;
}

int main() {
    listTest();
    accurTest();
    alterTest();
    alterAccurTest();
    return 0;
}
