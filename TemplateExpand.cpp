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

template<typename RegExpr, typename ...Remain>
class AccurExpr {
public:
    using expr = RegExpr;
};

template<typename RegExpr, typename ...Remain>
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
};

template<char only>
class CharList<only> {
public:
    using rest = nil;
    static constexpr char value = only;
    static constexpr int len = 1;
};

template<typename T, typename ...Remain>
class MatchImplement;

//template<typename T>
//class MatchImplement<T> {
//public:
//    template<typename Lambda>
//    static bool Apply(const char *target, Lambda function) {
//        return MatchImplement<T>::Apply(target);
//    }
//};

//template<char ...charlist>
//class MatchImplement<AlterExpr<CharList<charlist...>>> {
//public:
//    using regexRest = typename CharList<charlist ...>::rest;
//
//    template<typename Lambda>
//    static bool Apply(const char *target, Lambda function) {
//        return CharList<charlist ...>::Apply(target,
//                                             [function](const char *rest) -> bool {
//                                                 return *rest == '\0';
//                                             })
//               || MatchImplement<AlterExpr<typename CharList<charlist ...>::rest>>::Apply(target,
//                                                                                          [function](
//                                                                                                  const char *rest) -> bool {
//                                                                                              return *rest == '\0';
//                                                                                          });
//    }
//};

template<>
class MatchImplement<AlterExpr<nil>> {
public:
    static bool Apply(const char *target) {
        return false;
    }
};

template<char ...charlist>
class MatchImplement<AccurExpr<CharList<charlist...>>> {
public:
    using regexRest = typename CharList<charlist ...>::rest;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<AccurExpr<CharList<CharList<charlist ...>::value>>>::Apply(target,
                                                                                         [function](
                                                                                                 const char *rest) -> bool {
                                                                                             return MatchImplement<AccurExpr<typename CharList<charlist ...>::rest>>::Apply(
                                                                                                     rest, function);
                                                                                         }
        );
    }
};

template<char ch>
class MatchImplement<AccurExpr<CharList<ch>>> {
public:
    using regexRest = typename CharList<ch>::rest;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return *target && *target == ch && function(target + 1);
    }
};

template<typename First>
class MatchImplement<AccurExpr<First>> {
public:
    using rest = MatchImplement<nil>;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<AccurExpr<First>>::Apply(target,
                                                       [](const char *rest) -> bool {
                                                           return *rest == '\0';
                                                       }
        );
    }
};

template<typename First>
class MatchImplement<AccurExpr<First>> {
public:
    using rest = MatchImplement<nil>;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<First>::Apply(target, function);
    }
};

template<typename First, typename ...Remain>
class MatchImplement<AccurExpr<First, Remain...>> {
public:
    using rest = MatchImplement<Remain...>;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<AccurExpr<First>>::Apply(target,
                                                       [function](const char *rest) -> bool {
                                                           return MatchImplement<AccurExpr<Remain...>>::Apply(
                                                                   rest, function);
                                                       }
        );
    }
};


template<>
class MatchImplement<AccurExpr<nil>> {
public:
    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return true;
    }
};

template<typename RegexExpr>
bool RegexMatch(const char *target) {
    return (MatchImplement<RegexExpr>::Apply(target,
                                             [](const char *rest) -> bool {
                                                 return *rest == '\0';
                                             })
    );
}

#include <cassert>
#include <iostream>

using namespace std;

void listTest() {
//    assert((List<int,int,int>::len == 3));
    cout << "List Test: " << (List<int, int, int>::len == 3 ? "OK" : "ERR") << endl;
}

void accurTest() {
    constexpr const char *space = "                       ";
    cout << "Accuracy Pattern Test: "
    << (RegexMatch<AccurExpr<CharList<'a', 'b'>, AccurExpr<CharList<'a', 'b'>>>>("abab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'a', 'b'>,
            AccurExpr<CharList<'c', 'b'>,
                    AccurExpr<CharList<'h', 'b'>,
                            AccurExpr<CharList<'a', 'b'>,
                                    AccurExpr<CharList<'y', 'b'>, CharList<'y', 'b'>, CharList<'y', 'b'>>>>
            >>>("abcbhbabybybyb") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'a', 'b'>>>("ab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'b', 'b'>>>("ab") ? "ERR (This Test Should be False)" : "OK") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'a', 'b'>>>("abc") ? "ERR (This Test Should be False)" : "OK") << endl;
    cout << space
    <<
    (RegexMatch<AccurExpr<CharList<'a', 'b'>, AccurExpr<CharList<'a', 'b'>>>>("ab") ? "ERR (This Test Should be False)"
                                                                                    : "OK") << endl;
}

//void alterTest() {
//    constexpr const char *space = "                          ";
//    cout << "Alternative Pattern Test: "
//    << (RegexMatch<AlterExpr<CharList<'a', 'b'>>>("a") ? "OK" : "ERR") << endl;
//    cout << space
//    << (RegexMatch<AlterExpr<CharList<'a', 'b'>>>("a") ? "OK" : "ERR") << endl;
//    cout << space
//    << (RegexMatch<AlterExpr<CharList<'b', 'a'>>>("cc") ? "ERR (This Test Should be False)" : "OK") << endl;
//}
//
//void alterAccurTest() {
//    constexpr const char *space = "                    ";
//    cout << "Cross Pattern Test: "
//    << (RegexMatch<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>("bba") ? "OK" : "ERR") <<
//    endl;
//    cout << space
//    << (RegexMatch<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>("bbb") ? "OK" : "ERR") <<
//    endl;
//    cout << space
//    << (RegexMatch<AccurExpr<CharList<'a', 'b'>>, AlterExpr<CharList<'a', 'c'>>>("bbc")
//        ? "ERR (This Test Should be False)" : "OK") << endl;
//    cout << space
//    << (RegexMatch<AccurExpr<CharList<'b', 'b'>>, AlterExpr<CharList<'a', 'b'>>>("bbe")
////       NOTE: This test set one of Alter char to 'b' in case Alter match first char of Apply.
//        ? "ERR (This Test Should be False)" : "OK") << endl;
//}

int main() {
//    listTest();
    accurTest();
//    alterTest();
//    alterAccurTest();
    return 0;
}
