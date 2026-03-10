#pragma once

#include <type_traits>

#include "../core/char_traits.hpp"
#include "../core/list.hpp"
#include "../core/string.hpp"
#include "token.hpp"

namespace temppy {

template <typename Str, size_t Pos, bool InBounds = (Pos < Str::length)>
struct GetCharImpl {
  static constexpr char value = Str::template at<Pos>();
};

template <typename Str, size_t Pos> struct GetCharImpl<Str, Pos, false> {
  static constexpr char value = '\0';
};

template <typename Str, size_t Pos> using GetChar = GetCharImpl<Str, Pos>;

template <typename Str, size_t Pos, bool Done = (Pos >= Str::length)>
struct SkipWhitespaceImpl;

template <typename Str, size_t Pos> struct SkipWhitespaceImpl<Str, Pos, false> {
  static constexpr char C = GetChar<Str, Pos>::value;
  static constexpr bool is_ws = CharTraits<C>::is_whitespace;

  static constexpr size_t value =
      is_ws ? SkipWhitespaceImpl<Str, Pos + 1>::value : Pos;
};

template <typename Str, size_t Pos> struct SkipWhitespaceImpl<Str, Pos, true> {
  static constexpr size_t value = Pos;
};

template <typename Str, size_t Pos>
using SkipWhitespace = SkipWhitespaceImpl<Str, Pos>;

template <typename Str, size_t Pos, bool Done = (Pos >= Str::length)>
struct SkipCommentImpl;

template <typename Str, size_t Pos> struct SkipCommentImpl<Str, Pos, false> {
  static constexpr char C = GetChar<Str, Pos>::value;
  static constexpr size_t value =
      (C == '\n') ? Pos : SkipCommentImpl<Str, Pos + 1>::value;
};

template <typename Str, size_t Pos> struct SkipCommentImpl<Str, Pos, true> {
  static constexpr size_t value = Pos;
};

template <typename Str, size_t Pos>
using SkipComment = SkipCommentImpl<Str, Pos>;

template <typename Str, size_t Pos, int Acc = 0,
          bool Done = (Pos >= Str::length)>
struct ParseNumberImpl;

template <typename Str, size_t Pos, int Acc>
struct ParseNumberImpl<Str, Pos, Acc, false> {
  static constexpr char C = GetChar<Str, Pos>::value;

  static constexpr int value = []() constexpr {
    if constexpr (CharTraits<C>::is_digit) {
      return ParseNumberImpl<Str, Pos + 1,
                             Acc * 10 + CharTraits<C>::to_digit>::value;
    } else {
      return Acc;
    }
  }();

  static constexpr size_t next_pos = []() constexpr {
    if constexpr (CharTraits<C>::is_digit) {
      return ParseNumberImpl<Str, Pos + 1,
                             Acc * 10 + CharTraits<C>::to_digit>::next_pos;
    } else {
      return Pos;
    }
  }();
};

template <typename Str, size_t Pos, int Acc>
struct ParseNumberImpl<Str, Pos, Acc, true> {
  static constexpr int value = Acc;
  static constexpr size_t next_pos = Pos;
};

template <typename Str, size_t Pos, int Acc = 0>
using ParseNumber = ParseNumberImpl<Str, Pos, Acc>;

template <typename Str, size_t Pos, typename Acc = EmptyString,
          bool Done = (Pos >= Str::length)>
struct ParseIdentifierImpl;

template <typename Str, size_t Pos, typename Acc>
struct ParseIdentifierImpl<Str, Pos, Acc, false> {
  static constexpr char C = GetChar<Str, Pos>::value;
  static constexpr bool is_alnum = CharTraits<C>::is_alnum;

  using result = typename std::conditional<
      is_alnum, ParseIdentifierImpl<Str, Pos + 1, StringAppend_t<Acc, C>>,
      ParseIdentifierImpl<Str, Pos, Acc, true>>::type;

  using identifier = typename result::identifier;
  static constexpr size_t next_pos = result::next_pos;
};

template <typename Str, size_t Pos, typename Acc>
struct ParseIdentifierImpl<Str, Pos, Acc, true> {
  using identifier = Acc;
  static constexpr size_t next_pos = Pos;
};

template <typename Str, size_t Pos, typename Acc = EmptyString>
using ParseIdentifier = ParseIdentifierImpl<Str, Pos, Acc>;

template <typename Str, size_t Pos, size_t Indent = 0,
          bool Done = (Pos >= Str::length)>
struct CountIndentImpl;

template <typename Str, size_t Pos, size_t Indent>
struct CountIndentImpl<Str, Pos, Indent, false> {
  static constexpr char C = GetChar<Str, Pos>::value;

  static constexpr size_t value = []() constexpr {
    if constexpr (C == ' ' || C == '\t') {
      return CountIndentImpl<Str, Pos + 1, Indent + 1>::value;
    } else if constexpr (C == '\r') {
      return CountIndentImpl<Str, Pos + 1, Indent>::value;
    } else {
      return Indent;
    }
  }();

  static constexpr size_t next_pos = []() constexpr {
    if constexpr (C == ' ' || C == '\t' || C == '\r') {
      return CountIndentImpl<Str, Pos + 1,
                             (C == '\r' ? Indent : Indent + 1)>::next_pos;
    } else {
      return Pos;
    }
  }();
};

template <typename Str, size_t Pos, size_t Indent>
struct CountIndentImpl<Str, Pos, Indent, true> {
  static constexpr size_t value = Indent;
  static constexpr size_t next_pos = Pos;
};

template <typename Str, size_t Pos, size_t Indent = 0>
using CountIndent = CountIndentImpl<Str, Pos, Indent>;

template <size_t N> struct IndentLevel {
  static constexpr size_t value = N;
};

template <typename List, typename T> struct Prepend;

template <typename... Ts, typename T> struct Prepend<TypeList<Ts...>, T> {
  using type = TypeList<T, Ts...>;
};

template <typename List, typename T>
using Prepend_t = typename Prepend<List, T>::type;

template <typename Stack> struct IndentTop;

template <size_t N, typename... Rest>
struct IndentTop<TypeList<IndentLevel<N>, Rest...>> {
  static constexpr size_t value = N;
};

template <typename Tokens, typename Stack> struct AdjustIndentResult {
  using tokens = Tokens;
  using stack = Stack;
};

template <typename Tokens, typename Stack, size_t Target> struct AdjustIndent;

template <typename Tokens, typename Stack, size_t Target>
struct AdjustIndentDecrease {
  static_assert(Stack::size > 1, "Invalid indentation: dedent below zero");

  using next =
      AdjustIndent<Append_t<Tokens, DedentToken>, Tail_t<Stack>, Target>;
  using tokens = typename next::tokens;
  using stack = typename next::stack;
};

template <typename Tokens, typename Stack, size_t Target> struct AdjustIndent {
  static constexpr size_t current = IndentTop<Stack>::value;

  using branch = std::conditional_t<
      (Target == current), AdjustIndentResult<Tokens, Stack>,
      std::conditional_t<
          (Target > current),
          AdjustIndentResult<Append_t<Tokens, IndentToken>,
                             Prepend_t<Stack, IndentLevel<Target>>>,
          AdjustIndentDecrease<Tokens, Stack, Target>>>;

  using tokens = typename branch::tokens;
  using stack = typename branch::stack;
};

template <typename Tokens, typename Stack,
          bool Done = (IndentTop<Stack>::value == 0)>
struct FlushDedents;

template <typename Tokens, typename Stack>
struct FlushDedents<Tokens, Stack, true> {
  using tokens = Tokens;
};

template <typename Tokens, typename Stack>
struct FlushDedents<Tokens, Stack, false> {
  using next = FlushDedents<Append_t<Tokens, DedentToken>, Tail_t<Stack>>;
  using tokens = typename next::tokens;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack,
          bool AtLineStart, bool AtEnd = (Pos >= Str::length)>
struct LexerImpl;

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack,
          bool AtLineStart>
struct LexerImpl<Str, Pos, Accumulator, IndentStack, AtLineStart, true> {
  using flushed = FlushDedents<Accumulator, IndentStack>;
  using result = Append_t<typename flushed::tokens, EofToken>;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerImplLineStartDispatch;

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerImpl<Str, Pos, Accumulator, IndentStack, true, false> {
  using result = typename LexerImplLineStartDispatch<Str, Pos, Accumulator,
                                                     IndentStack>::result;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerLineStartBlankOrComment;

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack,
          size_t Indent>
struct LexerLineStartContent {
  using adjusted = AdjustIndent<Accumulator, IndentStack, Indent>;

  using result = typename LexerImpl<Str, Pos, typename adjusted::tokens,
                                    typename adjusted::stack, false>::result;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerLineStartBlankOrComment {
  static constexpr char current = GetChar<Str, Pos>::value;

  using result = typename std::conditional_t<
      current == '\n',
      LexerImpl<Str, Pos + 1, Append_t<Accumulator, NewlineToken>, IndentStack,
                true>,
      std::conditional_t<current == '#',
                         LexerImpl<Str, SkipComment<Str, Pos>::value,
                                   Accumulator, IndentStack, true>,
                         LexerImpl<Str, Pos, Accumulator, IndentStack, true>>>::
      result;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerImplLineStartDispatch {
  using indent_info = CountIndent<Str, Pos>;
  static constexpr size_t line_pos = indent_info::next_pos;
  static constexpr bool at_end = (line_pos >= Str::length);
  static constexpr char current = GetChar<Str, line_pos>::value;

  using result = typename std::conditional_t<
      at_end, LexerImpl<Str, line_pos, Accumulator, IndentStack, true>,
      std::conditional_t<
          (current == '\n' || current == '#'),
          LexerLineStartBlankOrComment<Str, line_pos, Accumulator, IndentStack>,
          LexerLineStartContent<Str, line_pos, Accumulator, IndentStack,
                                indent_info::value>>>::result;
};

template <typename Str, size_t Pos, typename Acc, typename IndentStack>
struct LexNumber {
  using result = typename LexerImpl<
      Str, ParseNumber<Str, Pos>::next_pos,
      Append_t<Acc, NumberToken<ParseNumber<Str, Pos>::value>>, IndentStack,
      false>::result;
};

template <typename Str, size_t Pos, typename Acc, typename IndentStack>
struct LexIdentifier {
  using result = typename LexerImpl<
      Str, ParseIdentifier<Str, Pos>::next_pos,
      Append_t<Acc,
               IdentifierToken<typename ParseIdentifier<Str, Pos>::identifier>>,
      IndentStack, false>::result;
};

template <typename Str, size_t Pos, typename Acc, typename IndentStack,
          typename TokenT>
struct LexSimpleToken {
  using result = typename LexerImpl<Str, Pos + 1, Append_t<Acc, TokenT>,
                                    IndentStack, false>::result;
};

template <typename Str, size_t Pos, typename Acc, typename IndentStack>
struct LexNewline {
  using result = typename LexerImpl<Str, Pos + 1, Append_t<Acc, NewlineToken>,
                                    IndentStack, true>::result;
};

template <typename Str, size_t Pos, typename Acc, typename IndentStack>
struct LexComment {
  using result = typename LexerImpl<Str, SkipComment<Str, Pos>::value, Acc,
                                    IndentStack, false>::result;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerImplNormalDispatch;

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerImpl<Str, Pos, Accumulator, IndentStack, false, false> {
  static constexpr size_t pos_after_ws = SkipWhitespace<Str, Pos>::value;

  using result = typename std::conditional_t<
      (pos_after_ws >= Str::length),
      LexerImpl<Str, pos_after_ws, Accumulator, IndentStack, false>,
      LexerImplNormalDispatch<Str, pos_after_ws, Accumulator,
                              IndentStack>>::result;
};

template <typename Str, size_t Pos, typename Accumulator, typename IndentStack>
struct LexerImplNormalDispatch {
  static constexpr char current = GetChar<Str, Pos>::value;

  using result = typename std::conditional_t<
      CharTraits<current>::is_digit,
      LexNumber<Str, Pos, Accumulator, IndentStack>,
      std::conditional_t<
          CharTraits<current>::is_alpha,
          LexIdentifier<Str, Pos, Accumulator, IndentStack>,
          std::conditional_t<
              current == '+',
              LexSimpleToken<Str, Pos, Accumulator, IndentStack, PlusToken>,
              std::conditional_t<
                  current == '-',
                  LexSimpleToken<Str, Pos, Accumulator, IndentStack,
                                 MinusToken>,
                  std::conditional_t<
                      current == '*',
                      LexSimpleToken<Str, Pos, Accumulator, IndentStack,
                                     StarToken>,
                      std::conditional_t<
                          current == '/',
                          LexSimpleToken<Str, Pos, Accumulator, IndentStack,
                                         SlashToken>,
                          std::conditional_t<
                              current == ':',
                              LexSimpleToken<Str, Pos, Accumulator, IndentStack,
                                             ColonToken>,
                              std::conditional_t<
                                  current == ',',
                                  LexSimpleToken<Str, Pos, Accumulator,
                                                 IndentStack, CommaToken>,
                                  std::conditional_t<
                                      current == '(',
                                      LexSimpleToken<Str, Pos, Accumulator,
                                                     IndentStack, LParenToken>,
                                      std::conditional_t<
                                          current == ')',
                                          LexSimpleToken<Str, Pos, Accumulator,
                                                         IndentStack,
                                                         RParenToken>,
                                          std::conditional_t<
                                              current == '=',
                                              LexSimpleToken<
                                                  Str, Pos, Accumulator,
                                                  IndentStack, AssignToken>,
                                              std::conditional_t<
                                                  current == '#',
                                                  LexComment<Str, Pos,
                                                             Accumulator,
                                                             IndentStack>,
                                                  std::conditional_t<
                                                      current == '\n',
                                                      LexNewline<Str, Pos,
                                                                 Accumulator,
                                                                 IndentStack>,
                                                      LexerImpl<
                                                          Str, Pos + 1,
                                                          Accumulator,
                                                          IndentStack,
                                                          false>>>>>>>>>>>>>>::
      result;
};

template <typename Str>
using Tokenize = typename LexerImpl<Str, 0, EmptyList, TypeList<IndentLevel<0>>,
                                    true>::result;

} // namespace temppy
