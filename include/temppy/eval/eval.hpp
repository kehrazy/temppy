#pragma once

#include "../core/value.hpp"
#include "../parser/ast.hpp"
#include "environment.hpp"

namespace temppy {

template <typename Value, typename Env> struct EvalResult {
  using value = Value;
  using env = Env;
};

template <typename Expr, typename Env> struct EvalExpr;

template <int N, typename Env> struct EvalExpr<NumberLiteral<N>, Env> {
  using result = EvalResult<IntValue<N>, Env>;
};

template <typename Name, typename Env> struct EvalExpr<Variable<Name>, Env> {
  using result = EvalResult<Lookup_t<Env, Name>, Env>;
};

template <typename Expr, typename Env>
struct EvalExpr<UnaryOp<'+', Expr>, Env> {
  using result = typename EvalExpr<Expr, Env>::result;
};

template <typename Expr, typename Env>
struct EvalExpr<UnaryOp<'-', Expr>, Env> {
  using expr_result = typename EvalExpr<Expr, Env>::result;
  using value = typename IntValue<0>::template Sub<typename expr_result::value>;

  using result = EvalResult<value, typename expr_result::env>;
};

template <typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '+', Right>, Env> {
  using left_result = typename EvalExpr<Left, Env>::result;
  using right_result =
      typename EvalExpr<Right, typename left_result::env>::result;
  using value =
      typename left_result::value::template Add<typename right_result::value>;

  using result = EvalResult<value, typename right_result::env>;
};

template <typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '-', Right>, Env> {
  using left_result = typename EvalExpr<Left, Env>::result;
  using right_result =
      typename EvalExpr<Right, typename left_result::env>::result;
  using value =
      typename left_result::value::template Sub<typename right_result::value>;

  using result = EvalResult<value, typename right_result::env>;
};

template <typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '*', Right>, Env> {
  using left_result = typename EvalExpr<Left, Env>::result;
  using right_result =
      typename EvalExpr<Right, typename left_result::env>::result;
  using value =
      typename left_result::value::template Mul<typename right_result::value>;

  using result = EvalResult<value, typename right_result::env>;
};

template <typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '/', Right>, Env> {
  using left_result = typename EvalExpr<Left, Env>::result;
  using right_result =
      typename EvalExpr<Right, typename left_result::env>::result;
  using value =
      typename left_result::value::template Div<typename right_result::value>;

  using result = EvalResult<value, typename right_result::env>;
};

template <typename Stmt, typename Env> struct EvalStmt;

template <typename Stmts, typename Env> struct EvalStmts;

template <bool RunBody, typename Body, typename Env> struct EvalIfBody;

template <typename Body, typename Env> struct EvalIfBody<false, Body, Env> {
  using result = EvalResult<NoneValue, Env>;
};

template <typename Body, typename Env> struct EvalIfBody<true, Body, Env> {
  using body_result = typename EvalStmts<Body, Env>::result;
  using result = EvalResult<NoneValue, typename body_result::env>;
};

template <typename Name, typename Body, int I, int End, typename Env,
          bool Done = (I >= End)>
struct EvalForRangeLoop;

template <typename Name, typename Body, int I, int End, typename Env>
struct EvalForRangeLoop<Name, Body, I, End, Env, true> {
  using result = EvalResult<NoneValue, Env>;
};

template <typename Name, typename Body, int I, int End, typename Env>
struct EvalForRangeLoop<Name, Body, I, End, Env, false> {
  using loop_env = Update_t<Env, Name, IntValue<I>>;
  using body_result = typename EvalStmts<Body, loop_env>::result;
  using result = typename EvalForRangeLoop<Name, Body, I + 1, End,
                                           typename body_result::env>::result;
};

template <typename Name, typename Expr, typename Env>
struct EvalStmt<Assignment<Name, Expr>, Env> {
  using expr_result = typename EvalExpr<Expr, Env>::result;
  using new_env =
      Update_t<typename expr_result::env, Name, typename expr_result::value>;

  using result = EvalResult<NoneValue, new_env>;
};

template <typename Condition, typename Body, typename Env>
struct EvalStmt<IfStatement<Condition, Body>, Env> {
  using cond_result = typename EvalExpr<Condition, Env>::result;
  static constexpr bool should_run = (cond_result::value::value != 0);

  using result =
      typename EvalIfBody<should_run, Body, typename cond_result::env>::result;
};

template <typename Name, typename LimitExpr, typename Body, typename Env>
struct EvalStmt<ForRangeStatement<Name, LimitExpr, Body>, Env> {
  using limit_result = typename EvalExpr<LimitExpr, Env>::result;
  using loop_result =
      typename EvalForRangeLoop<Name, Body, 0, limit_result::value::value,
                                typename limit_result::env>::result;

  using result = EvalResult<NoneValue, typename loop_result::env>;
};

template <typename Expr, typename Env>
struct EvalStmt<ExpressionStatement<Expr>, Env> {
  using expr_result = typename EvalExpr<Expr, Env>::result;

  using result =
      EvalResult<typename expr_result::value, typename expr_result::env>;
};

template <typename Env> struct EvalStmts<EmptyList, Env> {
  using result = EvalResult<NoneValue, Env>;
};

template <typename Stmt, typename... Rest, typename Env>
struct EvalStmts<TypeList<Stmt, Rest...>, Env> {
  using stmt_result = typename EvalStmt<Stmt, Env>::result;

  using result = std::conditional_t<
      IsEmpty_v<TypeList<Rest...>>, typename EvalStmt<Stmt, Env>::result,

      typename EvalStmts<TypeList<Rest...>, typename stmt_result::env>::result>;
};

template <typename Stmts, typename Env = EmptyEnv>
using Eval = typename EvalStmts<Stmts, Env>::result;

template <typename Result> using GetValue = typename Result::value;

template <typename Result> using GetEnv = typename Result::env;

} // namespace temppy
