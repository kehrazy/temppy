#pragma once

#include "../core/string.hpp"

namespace temppy {


enum class TokenType {
    Number,
    Identifier,
    Plus,
    Minus,
    Star,
    Slash,
    LParen,
    RParen,
    Assign,
    Newline,
    Eof
};


template<TokenType Type, typename Data = EmptyString, int IntData = 0>
struct Token {
    static constexpr TokenType type = Type;
    using data = Data;
    static constexpr int int_data = IntData;
};


template<int N>
using NumberToken = Token<TokenType::Number, EmptyString, N>;

template<typename Name>
using IdentifierToken = Token<TokenType::Identifier, Name>;

using PlusToken = Token<TokenType::Plus>;
using MinusToken = Token<TokenType::Minus>;
using StarToken = Token<TokenType::Star>;
using SlashToken = Token<TokenType::Slash>;
using LParenToken = Token<TokenType::LParen>;
using RParenToken = Token<TokenType::RParen>;
using AssignToken = Token<TokenType::Assign>;
using NewlineToken = Token<TokenType::Newline>;
using EofToken = Token<TokenType::Eof>;

} 
