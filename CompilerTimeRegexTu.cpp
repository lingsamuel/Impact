template<typename Left, typename Right>
struct ConcatExpr;

template<char ch>
struct MatchExpr;

template<typename RegExpr>
struct MatchImpl;

template<typename Left, typename Right>
struct MatchImpl<ConcatExpr<Left, Right>> {
    template<typename Continuation>
    static bool Apply(const char *target, Continuation cont) {
        return MatchImpl<Left>::Apply(target, [cont](const char *rest) -> bool {
            return MatchImpl<Right>::Apply(rest, cont);
        });
    }
};

template<char ch>
struct MatchImpl<MatchExpr<ch>> {
    template<typename Continuation>
    static bool Apply(const char *target, Continuation cont) {
        return *target && *target == ch && cont(target + 1);
    }
};

template<typename RegExpr>
bool RegexMatch(const char *target) {
    return MatchImpl<RegExpr>::Apply(
            target, [](const char *rest) -> bool { return *rest == '\0'; });
}

#include <cassert>

int main() {
    assert((RegexMatch<ConcatExpr<MatchExpr<'a'>, MatchExpr<'b'> > >("ab")));


    assert((RegexMatch<MatchExpr<'b'> >("b")));

    assert((RegexMatch<
            ConcatExpr<
                    MatchExpr<'a'>, ConcatExpr<MatchExpr<'b'>,
                            ConcatExpr<MatchExpr<'a'>, MatchExpr<'b'>
                            > > > >("abab")));
    return 0;
}

