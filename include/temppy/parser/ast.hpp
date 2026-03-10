#pragma once

#include "../core/list.hpp"
#include "../core/string.hpp"
#include "../core/value.hpp"

namespace temppy {

template <int N> struct NumberLiteral {
  static constexpr int value = N;
};

template <typename Name> struct Variable {
  using name = Name;
};

template <char Op, typename Expr> struct UnaryOp {
  static constexpr char op = Op;
  using expr = Expr;
};

template <typename Left, char Op, typename Right> struct BinaryOp {
  using left = Left;
  static constexpr char op = Op;
  using right = Right;
};

template <typename Name, typename Expr> struct Assignment {
  using name = Name;
  using expr = Expr;
};

template <typename Expr> struct ExpressionStatement {
  using expr = Expr;
};

template <typename Condition, typename Body> struct IfStatement {
  using condition = Condition;
  using body = Body;
};

template <typename Name, typename LimitExpr, typename Body>
struct ForRangeStatement {
  using name = Name;
  using limit = LimitExpr;
  using body = Body;
};

template <typename... Stmts> struct Block {
  using statements = TypeList<Stmts...>;
};

template <typename T> struct IsExpression : std::false_type {};

template <int N> struct IsExpression<NumberLiteral<N>> : std::true_type {};

template <typename Name>
struct IsExpression<Variable<Name>> : std::true_type {};

template <char Op, typename Expr>
struct IsExpression<UnaryOp<Op, Expr>> : std::true_type {};

template <typename L, char Op, typename R>
struct IsExpression<BinaryOp<L, Op, R>> : std::true_type {};

template <typename T>
inline constexpr bool IsExpression_v = IsExpression<T>::value;

template <typename T> struct IsStatement : std::false_type {};

template <typename Name, typename Expr>
struct IsStatement<Assignment<Name, Expr>> : std::true_type {};

template <typename Expr>
struct IsStatement<ExpressionStatement<Expr>> : std::true_type {};

template <typename Condition, typename Body>
struct IsStatement<IfStatement<Condition, Body>> : std::true_type {};

template <typename Name, typename LimitExpr, typename Body>
struct IsStatement<ForRangeStatement<Name, LimitExpr, Body>> : std::true_type {
};

template <typename... Stmts>
struct IsStatement<Block<Stmts...>> : std::true_type {};

template <typename T>
inline constexpr bool IsStatement_v = IsStatement<T>::value;

} // namespace temppy
