#pragma once

namespace temppy {


template<char C>
struct CharTraits {
    static constexpr bool is_digit = (C >= '0' && C <= '9');
    static constexpr bool is_alpha = (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') || (C == '_');
    static constexpr bool is_alnum = is_digit || is_alpha;
    static constexpr bool is_whitespace = (C == ' ' || C == '\t' || C == '\r');
    static constexpr bool is_newline = (C == '\n');
    static constexpr int to_digit = is_digit ? (C - '0') : -1;
};

} 
