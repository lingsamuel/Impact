#include <type_traits>
#include <string>

template<typename Left, typename Right>
class ConcatExpr;

class nil {
};

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

template<typename RegExpr, typename ...Remain>
// *, zero or more
class RepeatExpr {
public:
    using expr = RegExpr;
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


template<char ch>
class MatchImplement<CharList<ch>> {
    // the only case of Match<CharList> is Accurate Match
public:
    using regexRest = typename CharList<ch>::rest;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return *target && *target == ch && function(target + 1);
    }
};

template<char ...charlist>
class MatchImplement<AlterExpr<CharList<charlist...>>> {
public:
    using regexRest = typename CharList<charlist ...>::rest;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<AlterExpr<CharList<CharList<charlist ...>::value>>>::Apply(target, function) ||
               MatchImplement<AlterExpr<typename CharList<charlist ...>::rest>>::Apply(target, function);
    }
};

template<char ch>
class MatchImplement<AlterExpr<CharList<ch>>> {
public:
    using regexRest = typename CharList<ch>::rest;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return *target && *target == ch && function(target + 1);
    }
};

template<typename First>
class MatchImplement<AlterExpr<First>> {
public:
    using rest = MatchImplement<nil>;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        // 这个傻逼的设计是因为CharList是变长的，因此，递归到最后有一个nil
        // 但是这个nil是不能统一处理的，例如AccruatePattern和AlternativePattern就是不一样的
        // 一个必须返回true (&&)，一个必须返回false (||)，来保证结果不被影响
        // 仅用于剥开无意义嵌套，事实上来说应该全部的有意义嵌套都会被特化
        return MatchImplement<First>::Apply(target, function);
    }
};

template<typename First, typename ...Remain>
class MatchImplement<AlterExpr<First, Remain...>> {
public:
    using rest = MatchImplement<Remain...>;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<AlterExpr<First>>::Apply(target, function) ||
               MatchImplement<AlterExpr<Remain...>>::Apply(target, function);
    }
};

template<>
class MatchImplement<AlterExpr<nil>> {
public:
    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
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
        // 这个傻逼的设计是因为CharList是变长的，因此，递归到最后有一个nil
        // 但是这个nil是不能统一处理的，例如AccruatePattern和AlternativePattern就是不一样的
        // 一个必须返回true (&&)，一个必须返回false (||)，来保证结果不被影响
        // 仅用于剥开无意义嵌套，事实上来说应该全部的有意义嵌套都会被特化
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


template<typename ...Remain>
class MatchImplement<RepeatExpr<Remain...>> {
public:
    using rest = MatchImplement<Remain...>;

    template<typename Lambda>
    static bool Apply(const char *target, Lambda function) {
        return MatchImplement<AccurExpr<Remain...>>::Apply(target,
                                                           [target, function](const char *rest) -> bool {
                                                               return target < rest &&
                                                                      MatchImplement<RepeatExpr<Remain...>>::Apply(
                                                                              rest, function);
                                                           }
        ) || function(target);
    }
};


template<typename RegExpr, typename ...Remain>
using OptionalExpr = class AlterExpr<AlterExpr<RegExpr, Remain...>, AccurExpr<nil>>;

template<typename First, typename ...Remain>
using RepeatOnceExpr = class AccurExpr<AccurExpr<First, Remain...>, RepeatExpr<First, Remain...> >;


template<typename RegexExpr>
bool RegexMatch(const char *target) {
    return (MatchImplement<RegexExpr>::Apply(target,
                                             [](const char *rest) -> bool {
                                                 return *rest == '\0';
                                             })
    );
}

template<typename RegexExpr>
bool RegexSearch(const char *target) {
    return MatchImplement<RegexExpr>::Apply(
            target, [](const char *rest) -> bool { return true; }) ||
           (*target && RegexSearch<RegexExpr>(target + 1));
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
    cout << "Accurate Pattern Test: "
    << (RegexMatch<AccurExpr<CharList<'a', 'b'>, AccurExpr<CharList<'a', 'b'>>>>("abab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<AccurExpr<CharList<'a', 'b'>>, CharList<'a', 'b'>>>("abab") ? "OK" : "ERR") << endl;
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

void alterTest() {
    constexpr const char *space = "                          ";
    cout << "Alternative Pattern Test: "
    << (RegexMatch<AlterExpr<CharList<'a', 'b'>>>("a") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AlterExpr<CharList<'a', 'b'>>>("a") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AlterExpr<CharList<'b', 'a'>>>("cc") ? "ERR (This Test Should be False)" : "OK") << endl;
    cout << space
    << (RegexMatch<AlterExpr<CharList<'b', 'a'>>>("c") ? "ERR (This Test Should be False)" : "OK") << endl;
}

void alterAccurTest() {
    constexpr const char *space = "                    ";
    cout << "Cross Pattern Test: "
    << (RegexMatch<AccurExpr<CharList<'b', 'b'>, AlterExpr<CharList<'a', 'b'>> >>("bba") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AlterExpr<CharList<'a', 'b'>, AccurExpr<CharList<'e', 'f'>> >>("ef") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'b', 'b'>, AlterExpr<CharList<'a', 'b'>> >>("bbb") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'a', 'b'>, AlterExpr<CharList<'a', 'c'>> >>("bbc")
        ? "ERR (This Test Should be False)" : "OK") << endl;
    cout << space
    << (RegexMatch<AccurExpr<CharList<'b', 'b'>, AlterExpr<CharList<'a', 'b'>> >>("bbe")
        //       NOTE: This test set one of Alter char to 'b' in case Alter match first char of Apply.
        ? "ERR (This Test Should be False)" : "OK") << endl;
}

void optTest() {
    constexpr const char *space = "                       ";
    cout << "Optional Pattern Test: "
    << (RegexMatch<OptionalExpr<CharList<'b', 'b'>>>("") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<OptionalExpr<CharList<'b', 'b'>>>("bb") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<OptionalExpr<CharList<'b', 'b'>>, CharList<'g'>>>("g") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<OptionalExpr<CharList<'b', 'b'>>, CharList<'g'>>>("bbg") ? "OK" : "ERR") << endl;
}

void repeatTest() {
    constexpr const char *space = "                     ";
    cout << "Repeat Pattern Test: "
    << (RegexMatch<RepeatExpr<CharList<'a', 'b'>>>("abab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatExpr<CharList<'a'>>>("aaaa") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatExpr<CharList<'a'>>>("a") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatExpr<CharList<'a'>>>("") ? "OK" : "ERR") << endl;
}

void repeatAtLeastOnceTest() {
    constexpr const char *space = "                             ";
    cout << "RepeatOnceMore Pattern Test: "
    << (RegexMatch<RepeatOnceExpr<CharList<'a', 'b'>>>("abab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatOnceExpr<CharList<'a'>>>("aaaa") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatOnceExpr<CharList<'a'>>>("a") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatOnceExpr<CharList<'a'>>>("") ? "ERR (This Test Should be False)" : "OK") << endl;
}

void searchTest() {
    constexpr const char *space = "                     ";
    cout << "Search Pattern Test: "
    << (RegexSearch<AccurExpr<
            RepeatExpr<CharList<'a'>>, CharList<'b'>>>("aaabaabb") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexSearch<AccurExpr<
            RepeatExpr<CharList<'a'>>, CharList<'b'>>>("fadskg") ? "ERR (This Test Should be False)" : "OK") << endl;
}

void flexTest() {
    constexpr const char *space = "                   ";
    cout << "Flex Pattern Test: "
    << (RegexMatch<
            AccurExpr<
                    CharList<'a'>, AccurExpr<CharList<'b'>,
                            AccurExpr<CharList<'a', 'b'> >
                    > > >("abab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AlterExpr<CharList<'a', 'b'>>>("a") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AlterExpr<CharList<'a', 'b'>>>("b") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatExpr<CharList<'a'>>>("aaaaa") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<RepeatExpr<CharList<'a'>>, CharList<'b'>>>("aaaaab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<AccurExpr<RepeatExpr<CharList<'a'>>, CharList<'b'>>>("b") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexSearch<AccurExpr<RepeatExpr<CharList<'a'>>, CharList<'b'>>>("aaabaabb") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<OptionalExpr<CharList<'a'>>>("a") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<OptionalExpr<CharList<'a'>>>("") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<OptionalExpr<AccurExpr<CharList<'a', 'b'>>>>("ab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<OptionalExpr<AccurExpr<CharList<'a', 'b'>>>>("") ? "OK" : "ERR") << endl;
    cout << space
    << (!RegexMatch<RepeatExpr<CharList<'a'>>>("aaaaab") ? "OK" : "ERR") << endl;
    cout << space
    << (RegexMatch<RepeatExpr<OptionalExpr<CharList<'a'>>>>("") ? "OK" : "ERR") << endl;

}

void BigNestedTest() {
    constexpr const char *space = "                   ";
    cout << "Flex Pattern Test: "
    << (RegexMatch<
            AccurExpr<
                    CharList<'a', 'b', 'c'>, RepeatExpr<CharList<'d'>>,
                    RepeatExpr<
                            CharList<'a', 'b'>
                    >,
                    CharList<'c'>,
                    AccurExpr<CharList<'d'>, OptionalExpr<CharList<'s', 'b'> >, RepeatExpr<CharList<'a'>>>,
                    RepeatOnceExpr<CharList<'g'>>> >("abcdddddababababcdg") ? "OK" : "ERR") << endl;

    cout << space << (RegexSearch<
            AccurExpr<
                    CharList<'a', 'b', 'c'>, RepeatExpr<CharList<'d'>>,
                    RepeatExpr<
                            CharList<'a', 'b'>
                    >,
                    CharList<'c'>,
                    AccurExpr<CharList<'d'>, OptionalExpr<CharList<'s', 'b'> >, RepeatExpr<CharList<'a'>>>,
                    RepeatOnceExpr<CharList<'g'>>> >("dfhgdfhdfhdgggggabcdddddababababcdghhhhh") ? "OK" : "ERR") << endl;

    cout << space
    << (RegexSearch<
            AccurExpr<
                    CharList<'a', 'b', 'c'>, RepeatExpr<CharList<'d'>>,
                    RepeatExpr<
                            CharList<'a', 'b'>
                    >,
                    CharList<'c'>,
                    AccurExpr<CharList<'d'>, OptionalExpr<CharList<'s', 'b'> >, RepeatExpr<CharList<'a'>>>,
                    RepeatOnceExpr<CharList<'g'>>> >("aaaaaaaabcdddddababababcdghhhhh") ? "OK" : "ERR") << endl;

    cout << space
    << (RegexSearch<
            AccurExpr<
                    RepeatExpr<CharList<'a'>>,
                    CharList<'a'> > >("aaaaa") ? "OK" : "ERR") << endl;


    cout << space
    << (RegexSearch<
            AccurExpr<
                    RepeatExpr<CharList<'a'>>,
                    CharList<'a'> > >("a") ? "OK" : "ERR") << endl;
}

int main() {
    listTest();
    accurTest();
    alterTest();
    alterAccurTest();
    optTest();
    repeatTest();
    repeatAtLeastOnceTest();
    searchTest();
    flexTest();
    BigNestedTest();
    return 0;
}
