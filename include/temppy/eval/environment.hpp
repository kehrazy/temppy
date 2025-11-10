#pragma once

#include "../core/list.hpp"
#include "../core/string.hpp"
#include "../core/value.hpp"

namespace temppy {


template<typename Name, typename Value>
struct Binding {
    using name = Name;
    using value = Value;
};


using EmptyEnv = TypeList<>;


template<typename Env, typename Name>
struct Lookup;

template<typename Name>
struct Lookup<EmptyEnv, Name> {
    
    using value = IntValue<0>;
};

template<typename BindingName, typename BindingValue, typename... Rest, typename Name>
struct Lookup<TypeList<Binding<BindingName, BindingValue>, Rest...>, Name> {
    
    static constexpr bool is_same = std::is_same_v<BindingName, Name>;

    using value = typename std::conditional<
        is_same,
        BindingValue,
        typename Lookup<TypeList<Rest...>, Name>::value
    >::type;
};

template<typename Env, typename Name>
using Lookup_t = typename Lookup<Env, Name>::value;


template<typename Env, typename Name, typename Value>
struct Update;

template<typename Name, typename Value>
struct Update<EmptyEnv, Name, Value> {
    using env = TypeList<Binding<Name, Value>>;
};

template<typename BindingName, typename BindingValue, typename... Rest, typename Name, typename Value>
struct Update<TypeList<Binding<BindingName, BindingValue>, Rest...>, Name, Value> {
    static constexpr bool is_same = std::is_same_v<BindingName, Name>;

    using env = typename std::conditional<
        is_same,
        
        TypeList<Binding<Name, Value>, Rest...>,
        
        Concat_t<
            TypeList<Binding<BindingName, BindingValue>>,
            typename Update<TypeList<Rest...>, Name, Value>::env
        >
    >::type;
};

template<typename Env, typename Name, typename Value>
using Update_t = typename Update<Env, Name, Value>::env;

} 
