//
// Created by lingsamuel on 16-4-27.
//

template<typename Left, typename Right>
struct ConcatExpr;

template<typename Left, typename Right>
struct AlternativeExpr;

template<typename SubExpr>
struct RepeatExpr;

template<char ch>
struct MatchExpr;

template<typename RegExpr>
struct MatchImpl;

struct EpsilonExpr;

template<typename SubExpr>
using OptionalExpr = AlternativeExpr<SubExpr, EpsilonExpr>;

template<typename Left, typename Right>
// 1.1 MatchExpr<'a'>, MatchExpr<'b'>
struct MatchImpl<ConcatExpr<Left, Right>> {
    // reg like "ab" == "a""b"
    // 连续匹配的正则表达式
    template<typename Continuation>
    // 1.2 Bool isTerminal
    static bool Apply(const char *target, Continuation cont) {
        // 1.3 MatchImpl<MatchExpr<'b'>>::Apply(rest, isTerminal);
        return MatchImpl<Left>::Apply(target, [cont](const char *rest) -> bool {
            return MatchImpl<Right>::Apply(rest, cont);
        });
    }
};

template<typename Left, typename Right>
struct MatchImpl<AlternativeExpr<Left, Right>> {
    template<typename Continuation>
    static bool Apply(const char *target, Continuation cont) {
        return MatchImpl<Left>::Apply(target, cont) ||
               MatchImpl<Right>::Apply(target, cont);
    }
};

template<typename SubExpr>
struct MatchImpl<RepeatExpr<SubExpr>> {
    template<typename Continuation>
    static bool Apply(const char *target, Continuation cont) {
        return MatchImpl<SubExpr>::Apply(
                target,
                [target, cont](const char *rest) -> bool {
                    return target < rest &&
                           MatchImpl<RepeatExpr<SubExpr>>::Apply(rest, cont);
                }) ||
               cont(target);
    }
};

template<char ch>
struct MatchImpl<MatchExpr<ch>> {
    template<typename Continuation>
    // 1.4 Bool isTerminal
    static bool Apply(const char *target, Continuation cont) {
        // target invaild, return false
        // next target invaild, return false
        return *target && *target == ch && cont(target + 1);
    }
};

template<>
struct MatchImpl<EpsilonExpr> {
    template<typename Continuation>
    static bool Apply(const char *target, Continuation cont) {
        return cont(target);
    }
};

template<typename RegExpr>
bool RegexMatch(const char *target) {
    // usage: RegexMatch<RegexExpression>(String)

    // 1.ConcatExpr< MatchExpr<'a'>, MatchExpr<'b'> >
    return MatchImpl<RegExpr>::Apply(
            // Apply RegexExpression, diff expr has  diff imple
            target, [](const char *rest) -> bool { return *rest == '\0'; });
}

template<typename RegExpr>
bool RegexSearch(const char *target) {
    return MatchImpl<RegExpr>::Apply(
            target, [](const char *rest) -> bool { return true; }) ||
           (*target && RegexSearch<RegExpr>(target + 1));
}

#include <cassert>

int main() {
    assert((RegexMatch<ConcatExpr< MatchExpr<'a'>, MatchExpr<'b'> > >("ab") ) );
    assert((RegexMatch<
            ConcatExpr<
                    MatchExpr<'a'>, ConcatExpr<MatchExpr<'b'> ,
                            ConcatExpr< MatchExpr<'a'>, MatchExpr<'b'> >
                    > > >("abab") ) );
    assert((RegexMatch<AlternativeExpr<MatchExpr<'a'>, MatchExpr<'b'>>>("a")));
    assert((RegexMatch<AlternativeExpr<MatchExpr<'a'>, MatchExpr<'b'>>>("b")));
    assert((RegexMatch<RepeatExpr<MatchExpr<'a'>>>("aaaaa")));
    assert((RegexMatch<ConcatExpr<RepeatExpr<MatchExpr<'a'>>, MatchExpr<'b'>>>(
            "aaaaab")));
    assert((RegexMatch<ConcatExpr<RepeatExpr<MatchExpr<'a'>>, MatchExpr<'b'>>>("b")));
    assert((RegexSearch<ConcatExpr<RepeatExpr<MatchExpr<'a'>>, MatchExpr<'b'>>>(
            "aaaababb")));
    assert((RegexMatch<OptionalExpr<MatchExpr<'a'>>>("a")));
    assert((RegexMatch<OptionalExpr<MatchExpr<'a'>>>("")));
    assert((RegexMatch<OptionalExpr<ConcatExpr<MatchExpr<'a'>, MatchExpr<'b'>>>>(
            "ab")));
    assert((RegexMatch<OptionalExpr<ConcatExpr<MatchExpr<'a'>, MatchExpr<'b'>>>>(
            "")));
    assert((!RegexMatch<RepeatExpr<MatchExpr<'a'>>>("aaaaab")));
    assert((RegexMatch<RepeatExpr<OptionalExpr<MatchExpr<'a'>>>>("")));


    return 0;
}
