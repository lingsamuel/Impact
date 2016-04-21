#include <iostream>
#include <utility>
#include <tuple>
#include <vector>
#include <deque>
#include <type_traits>

using std::cout;
using std::endl;


namespace meta_programming {

    template<class T, class R>
    struct eq {
        static constexpr bool value = false;
    };;

    template<class T>
    struct eq<T, T> {
        static constexpr bool value = true;
    };


    template<class T>
    struct value {
        using result = T;
    };


    template<bool condition, class T, class R>
    struct cond {
        using result = R;
    };

    template<class T, class R>
    struct cond<true, T, R> {
        using result = T;
    };

    struct nil {
        static constexpr bool value = false;
    };

/**
 * This is the core structure, in correspondance with the lists in Lisp.
 * This is a structure.
 */
    template<class First, class ...Rest>
    struct list {
        using first = First;
        using rest = list<Rest...>;
        static constexpr int length = 1 + rest::length;
    };

    template<class First>
    struct list<First> {
        using first = First;
        using rest = nil;
        static constexpr int length = 1;
    };

    template<int index, class Typelist>
    struct get {
        using result = typename get<index - 1, typename Typelist::rest>::result;
    };

    template<class Typelist>
    struct get<0, Typelist> {
        using result = typename Typelist::first;
    };


    template<class First, class Second>
    struct cons {
        using result = list<First, Second>;
    };

    template<class T>
    struct cdr {
        using result = typename T::rest;
    };

    template<class T>
    struct car {
        using result = typename T::first;
    };

/**
 * Is syntactic sugar for `cons`.
 */
    template<class ...Types>
    struct brace {
        using result = list<Types...>;
    };

    template<template<typename...> class VTempl, class ...Args>
    struct apply_var {
        using result = typename VTempl<Args...>::result;
    };

//template<template<typename...> class VFunc, class TypeList, size_t... Indices>
//struct apply_explode {
//  using result = VFunc<get<Indices, TypeList>...>;
//};

    template<template<typename...> class VFunc, class TypeList, size_t... Indices>
    VFunc<typename get<Indices, TypeList>::result...> help_explode(std::index_sequence<Indices...>) {
        VFunc<typename get<Indices, TypeList>::result...> nonsense;
        return nonsense;
    };

/**
 * This is the true apply. Apply a variadic template to a typelist.
 *
 * This is the core part.
 */
    template<template<typename...> class VFunc, class TypeList>
    struct apply {
//  using result = typename VTempl<Args...>::result;
//  using result = cons
        using result = decltype(help_explode<VFunc, TypeList>(std::make_index_sequence<TypeList::length>{}));

    };

    template<template<typename...> class VFunc, class TypeList>
    struct three_apply {
        using result = VFunc<typename TypeList::first, typename TypeList::rest::first, typename TypeList::rest::rest::first>;
    };

    template<template<typename> class Func, class ...Args>
    struct map_list_var {
//  using result = typename VTempl<Args...>::result;
    };

    template<template<typename> class Func, class TypeList>
    struct map {
        using result = typename cons<
                typename Func<
                        typename car<TypeList>::result
                >::result,
                typename map<
                        Func,
                        typename cdr<TypeList>::result
                >::result
        >::result;

        using result2 = list<
                typename Func<typename car<TypeList>::result>::result,
                typename map<Func, typename cdr<TypeList>::result>::result
        >;
    };

    template<template<typename> class Func>
    struct map<Func, nil> {
//  using result = typename VTempl<Args...>::result;
        using result = nil;
    };


    template<template<typename> class VFunc, class TypeList, size_t... Indices>
    list<typename VFunc<typename get<Indices, TypeList>::result>::result...> help_explode2(std::index_sequence<Indices...>) {
        list<typename VFunc<typename get<Indices, TypeList>::result>::result...> nonsense;
        return nonsense;
    };

/**
 * This is the true apply. Apply a variadic template to a typelist.
 *
 * This is the core part.
 */
    template<template<typename> class VFunc, class TypeList>
    struct map2 {
//  using result = typename VTempl<Args...>::result;
//  using result = cons
        using result = decltype(help_explode2<VFunc, TypeList>(std::make_index_sequence<TypeList::length>{}));
    };

}

struct IntWrapper {
    using value = int;
};

struct DoubleWrapper {
    using value = double;
};

template<class T>
struct GetValue {
    using result = typename T::value;
};

template<class T>
struct GetValueType {
    using result = typename T::value_type;
};

int main() {
    using namespace meta_programming;
    cout << "Hello, World!" << endl;

    meta_programming::list<int, double> a;
    meta_programming::brace<int, double, char>::result b;

    cout << eq<
            car<
                    list<
                            int,
                            double
                    >
            >::result, int
    >::value << '\n';

    cout << eq<
            car<
                    cons<
                            int,
                            double
                    >::result
            >::result, int
    >::value << '\n';

    cout
    << eq<
            int,
            car<
                    map<
                            GetValue,
                            list<
                                    IntWrapper,
                                    DoubleWrapper
                            >
                    >::result
            >::result
    >::value
    << '\n';

    car<
            map<
                    GetValue,
                    list<
                            IntWrapper,
                            DoubleWrapper
                    >
            >::result
    >::result
            c = 3;

    get<0, list<int, double>>::result d = 4;

    cout << "The length of the list is " << list<int, double>::length << '\n';

    cout << eq<get<0, list<int, double> >::result, int>::value << '\n';
    cout << eq<get<0, list<int, double> >::result, double>::value << '\n';
    cout << eq<get<1, list<int, double> >::result, int>::value << '\n';
    cout << eq<get<1, list<int, double> >::result, double>::value << '\n';


    cout << eq<cond<true, int, double>::result, int>::value << '\n';
    cout << eq<cond<true, int, double>::result, double>::value << '\n';
    cout << eq<cond<false, int, double>::result, int>::value << '\n';
    cout << eq<cond<false, int, double>::result, double>::value << '\n';

    apply<std::tuple, list<int, double>>::result e = std::make_tuple(1, 1.5);
    cout << std::get<0>(e) << ',' << std::get<1>(e) << '\n';
    cout << eq<
            apply<
                    std::tuple,
                    list<
                            int,
                            double
                    >
            >::result,
            std::tuple<int, double>
    >::value << '\n';

    std::vector<int> intVector;
//    std::vector<int>::value_type;

    using types = map2<
            GetValueType,
            list<
                    std::vector<int>, std::deque<double>, std::vector<char>
            >
    >::result;
//
//    std::cout << std::is_same< get<0, types>::result , int>::value << '\n';
//    std::cout << std::is_same< get<0, types>::result , double>::value << '\n';
//    std::cout << eq< get<0, types>::result , int>::value << '\n';
//    std::cout << eq< get<0, types>::result , double>::value << '\n';

    std::cout << eq<get<0, types>::result, int>::value << '\n';
    std::cout << eq<get<1, types>::result, double>::value << '\n';
//    std::cout << eq< get<2, types>::result , char>::value << '\n';

//    types thing_ = 3;
//    list<int, double, char> thing_ = 3;

    using tuple_type = apply<std::tuple, types>::result;
    tuple_type thing = std::make_tuple(1, 2.5, 'c');

    using tuple_type2 = apply<std::tuple, list<int, double, char> >::result;
    tuple_type2 thing2 = std::make_tuple(1, 2.5, 'c');;

    std::cout << eq<types, list<int, double, char> >::value << '\n';

    return 0;
}