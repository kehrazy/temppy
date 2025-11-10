#pragma once

#include <cstddef>

namespace temppy {


template<typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);
};


using EmptyList = TypeList<>;


template<typename List, typename T>
struct Append;

template<typename... Ts, typename T>
struct Append<TypeList<Ts...>, T> {
    using type = TypeList<Ts..., T>;
};

template<typename List, typename T>
using Append_t = typename Append<List, T>::type;


template<typename L1, typename L2>
struct Concat;

template<typename... T1s, typename... T2s>
struct Concat<TypeList<T1s...>, TypeList<T2s...>> {
    using type = TypeList<T1s..., T2s...>;
};

template<typename L1, typename L2>
using Concat_t = typename Concat<L1, L2>::type;


template<typename List>
struct Head;

template<typename T, typename... Ts>
struct Head<TypeList<T, Ts...>> {
    using type = T;
};

template<typename List>
using Head_t = typename Head<List>::type;


template<typename List>
struct Tail;

template<typename T, typename... Ts>
struct Tail<TypeList<T, Ts...>> {
    using type = TypeList<Ts...>;
};

template<typename List>
using Tail_t = typename Tail<List>::type;


template<typename List>
struct IsEmpty {
    static constexpr bool value = false;
};

template<>
struct IsEmpty<TypeList<>> {
    static constexpr bool value = true;
};

template<typename List>
inline constexpr bool IsEmpty_v = IsEmpty<List>::value;


template<typename List, size_t Index>
struct At;

template<typename T, typename... Ts>
struct At<TypeList<T, Ts...>, 0> {
    using type = T;
};

template<typename T, typename... Ts, size_t Index>
struct At<TypeList<T, Ts...>, Index> {
    using type = typename At<TypeList<Ts...>, Index - 1>::type;
};

template<typename List, size_t Index>
using At_t = typename At<List, Index>::type;

} 
