#pragma once

#include <cstddef>
#include <utility>
#include "char_traits.hpp"

namespace temppy {


template<char... Cs>
struct String {
    static constexpr size_t length = sizeof...(Cs);
    static constexpr char data[sizeof...(Cs) + 1] = {Cs..., '\0'};

    template<size_t Index>
    static constexpr char at() {
        static_assert(Index < length, "String index out of bounds");
        constexpr char arr[] = {Cs...};
        return arr[Index];
    }
};

template<char... Cs>
constexpr char String<Cs...>::data[];


using EmptyString = String<>;


template<typename S1, typename S2>
struct StringConcat;

template<char... C1s, char... C2s>
struct StringConcat<String<C1s...>, String<C2s...>> {
    using type = String<C1s..., C2s...>;
};

template<typename S1, typename S2>
using StringConcat_t = typename StringConcat<S1, S2>::type;


template<typename S, char C>
struct StringAppend;

template<char... Cs, char C>
struct StringAppend<String<Cs...>, C> {
    using type = String<Cs..., C>;
};

template<typename S, char C>
using StringAppend_t = typename StringAppend<S, C>::type;

template <std::size_t N>
struct fixed_string {
    char value[N];
    static constexpr std::size_t size = N;
    constexpr fixed_string(const char (&str)[N]) : value{} {
        for (std::size_t i = 0; i < N; ++i) value[i] = str[i];
    }
    constexpr char operator[](std::size_t i) const { return value[i]; }
};

template <auto S, typename Seq>
struct ToString;

template <auto S, std::size_t... I>
struct ToString<S, std::index_sequence<I...>> {
    using type = temppy::String<S[I]...>;
};

template <auto S>
using ToString_t = typename ToString<S, std::make_index_sequence<S.size - 1>>::type;

} // namespace temppy

template <char... Cs>
consteval temppy::fixed_string<sizeof...(Cs) + 1> operator""_fs() {
    return temppy::fixed_string<sizeof...(Cs) + 1>{ { Cs..., '\0' } };
}
