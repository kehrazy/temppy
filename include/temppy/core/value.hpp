#pragma once

#include <type_traits>
#include "string.hpp"

namespace temppy {


enum class ValueType {
    Integer,
    None
};


template<int N>
struct IntValue {
    static constexpr ValueType type = ValueType::Integer;
    static constexpr int value = N;

    template<typename Other>
    using Add = IntValue<N + Other::value>;

    template<typename Other>
    using Sub = IntValue<N - Other::value>;

    template<typename Other>
    using Mul = IntValue<N * Other::value>;

    template<typename Other>
    using Div = IntValue<N / Other::value>;

    template<typename Other>
    using Eq = std::bool_constant<N == Other::value>;

    template<typename Other>
    using Lt = std::bool_constant<N < Other::value>;

    template<typename Other>
    using Gt = std::bool_constant<(N > Other::value)>;
};


struct NoneValue {
    static constexpr ValueType type = ValueType::None;
};


template<typename T>
struct IsValue : std::false_type {};

template<int N>
struct IsValue<IntValue<N>> : std::true_type {};

template<>
struct IsValue<NoneValue> : std::true_type {};

template<typename T>
inline constexpr bool IsValue_v = IsValue<T>::value;

} 
