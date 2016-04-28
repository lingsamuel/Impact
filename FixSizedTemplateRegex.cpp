#include <type_traits>
#include <string>

template<char ch>
class MatchChar;

template<typename Left,typename Right>
class AccurExpr;

template<typename Left,typename Right>
class AlterExpr;

template <typename Expr>
class RepeatExpr;

template <typename Regex>
class MatchImplement;

template<typename Left, typename Right>
class MatchImplement<AccurExpr<Left, Right>> {
public:
    static bool Apply(const char *target) {
        return MatchImplement<Left>::Apply(target) && MatchImplement<Right>::Apply(target);
    }
};

template<char ch>
class MatchImplement<MatchChar<ch>> {
public:
//    template <typename Next>
    static bool Apply(const char *target) {
        return *target && (*target == ch);
    }
};

template<typename Left,typename Right>
class MatchImplement<AlterExpr<Left,Right>> {
public:
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
    return (MatchImplement<RegexExpr>::Apply(target));
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
