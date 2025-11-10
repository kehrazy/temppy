#pragma once

#include "token.hpp"
#include "../core/string.hpp"
#include "../core/list.hpp"
#include "../core/char_traits.hpp"

namespace temppy {

template<typename Str, size_t Pos, bool InBounds = (Pos < Str::length)>
struct GetCharImpl {
    static constexpr char value = Str::template at<Pos>();
};

template<typename Str, size_t Pos>
struct GetCharImpl<Str, Pos, false> {
    static constexpr char value = '\0';
};

template<typename Str, size_t Pos>
using GetChar = GetCharImpl<Str, Pos>;

template<typename Str, size_t Pos, bool Done = (Pos >= Str::length)>
struct SkipWhitespaceImpl;

template<typename Str, size_t Pos>
struct SkipWhitespaceImpl<Str, Pos, false> {
    static constexpr char C = GetChar<Str, Pos>::value;
    static constexpr bool is_ws = CharTraits<C>::is_whitespace;

    static constexpr size_t value = is_ws ?
        SkipWhitespaceImpl<Str, Pos + 1>::value : Pos;
};

template<typename Str, size_t Pos>
struct SkipWhitespaceImpl<Str, Pos, true> {
    static constexpr size_t value = Pos;
};

template<typename Str, size_t Pos>
using SkipWhitespace = SkipWhitespaceImpl<Str, Pos>;

template<typename Str, size_t Pos, int Acc = 0, bool Done = (Pos >= Str::length)>
struct ParseNumberImpl;

template<typename Str, size_t Pos, int Acc>
struct ParseNumberImpl<Str, Pos, Acc, false> {
    static constexpr char C = GetChar<Str, Pos>::value;

    static constexpr int value = []() constexpr {
        if constexpr (CharTraits<C>::is_digit) {
            return ParseNumberImpl<Str, Pos + 1, Acc * 10 + CharTraits<C>::to_digit>::value;
        } else {
            return Acc;
        }
    }();

    static constexpr size_t next_pos = []() constexpr {
        if constexpr (CharTraits<C>::is_digit) {
            return ParseNumberImpl<Str, Pos + 1, Acc * 10 + CharTraits<C>::to_digit>::next_pos;
        } else {
            return Pos;
        }
    }();
};

template<typename Str, size_t Pos, int Acc>
struct ParseNumberImpl<Str, Pos, Acc, true> {
    static constexpr int value = Acc;
    static constexpr size_t next_pos = Pos;
};

template<typename Str, size_t Pos, int Acc = 0>
using ParseNumber = ParseNumberImpl<Str, Pos, Acc>;

template<typename Str, size_t Pos, typename Acc = EmptyString, bool Done = (Pos >= Str::length)>
struct ParseIdentifierImpl;

template<typename Str, size_t Pos, typename Acc>
struct ParseIdentifierImpl<Str, Pos, Acc, false> {
    static constexpr char C = GetChar<Str, Pos>::value;
    static constexpr bool is_alnum = CharTraits<C>::is_alnum;

    using result = typename std::conditional<
        is_alnum,
        ParseIdentifierImpl<Str, Pos + 1, StringAppend_t<Acc, C>>,
        ParseIdentifierImpl<Str, Pos, Acc, true>
    >::type;

    using identifier = typename result::identifier;
    static constexpr size_t next_pos = result::next_pos;
};

template<typename Str, size_t Pos, typename Acc>
struct ParseIdentifierImpl<Str, Pos, Acc, true> {
    using identifier = Acc;
    static constexpr size_t next_pos = Pos;
};

template<typename Str, size_t Pos, typename Acc = EmptyString>
using ParseIdentifier = ParseIdentifierImpl<Str, Pos, Acc>;

template<typename Str, size_t Pos, typename Accumulator, bool AtEnd = (Pos >= Str::length)>
struct LexerImpl;

template<typename Str, size_t Pos, typename Accumulator>
struct LexerImplDispatch;

template<typename Str, size_t Pos, typename Acc>
struct LexNumber {
    using result = typename LexerImpl<
        Str,
        ParseNumber<Str, Pos>::next_pos,
        Append_t<Acc, NumberToken<ParseNumber<Str, Pos>::value>>
    >::result;
};

template<typename Str, size_t Pos, typename Acc>
struct LexIdentifier {
    using result = typename LexerImpl<
        Str,
        ParseIdentifier<Str, Pos>::next_pos,
        Append_t<Acc, IdentifierToken<typename ParseIdentifier<Str, Pos>::identifier>>
    >::result;
};

template<typename Str, size_t Pos, typename Acc, typename TokenType>
struct LexSimpleToken {
    using result = typename LexerImpl<Str, Pos + 1, Append_t<Acc, TokenType>>::result;
};

template<typename Str, size_t Pos, typename Accumulator>
struct LexerImpl<Str, Pos, Accumulator, true> {
    using result = Append_t<Accumulator, EofToken>;
};


template<typename Str, size_t Pos, typename Accumulator>
struct LexerImpl<Str, Pos, Accumulator, false> {
    static constexpr size_t pos_after_ws = SkipWhitespace<Str, Pos>::value;
    static constexpr bool at_end_after_ws = (pos_after_ws >= Str::length);

    using result = typename std::conditional<
        at_end_after_ws,
        Append_t<Accumulator, EofToken>,
        typename LexerImplDispatch<Str, pos_after_ws, Accumulator>::result
    >::type;
};


template<typename Str, size_t Pos, typename Accumulator>
struct LexerImplDispatch {
    static constexpr char current = GetChar<Str, Pos>::value;

    using result = typename std::conditional<
        CharTraits<current>::is_digit,
        LexNumber<Str, Pos, Accumulator>,

        typename std::conditional<
            CharTraits<current>::is_alpha,
            LexIdentifier<Str, Pos, Accumulator>,

            typename std::conditional<
                current == '+',
                LexSimpleToken<Str, Pos, Accumulator, PlusToken>,

                typename std::conditional<
                    current == '-',
                    LexSimpleToken<Str, Pos, Accumulator, MinusToken>,

                    typename std::conditional<
                        current == '*',
                        LexSimpleToken<Str, Pos, Accumulator, StarToken>,

                        typename std::conditional<
                            current == '/',
                            LexSimpleToken<Str, Pos, Accumulator, SlashToken>,

                            typename std::conditional<
                                current == '(',
                                LexSimpleToken<Str, Pos, Accumulator, LParenToken>,

                                typename std::conditional<
                                    current == ')',
                                    LexSimpleToken<Str, Pos, Accumulator, RParenToken>,

                                    typename std::conditional<
                                        current == '=',
                                        LexSimpleToken<Str, Pos, Accumulator, AssignToken>,

                                        typename std::conditional<
                                            current == '\n',
                                            LexSimpleToken<Str, Pos, Accumulator, NewlineToken>,

                                            
                                            LexerImpl<Str, Pos + 1, Accumulator>
                                        >::type
                                    >::type
                                >::type
                            >::type
                        >::type
                    >::type
                >::type
            >::type
        >::type
    >::type::result;
};


template<typename Str>
using Tokenize = typename LexerImpl<Str, 0, EmptyList>::result;

} 
