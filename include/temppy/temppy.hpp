#pragma once

#include "core/char_traits.hpp"
#include "core/list.hpp"
#include "core/string.hpp"
#include "core/value.hpp"
#include "eval/environment.hpp"
#include "eval/eval.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"

namespace temppy {

template <typename Str> struct Run {
  using tokens = Tokenize<Str>;
  using ast = Parse<tokens>;
  using result = Eval<ast>;
  using value = GetValue<result>;
  using env = GetEnv<result>;
};

} // namespace temppy
