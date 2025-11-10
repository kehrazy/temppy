#pragma once

#include "environment.hpp"
#include "../parser/ast.hpp"
#include "../core/value.hpp"

namespace temppy {


template<typename Value, typename Env>
struct EvalResult {
    using value = Value;
    using env = Env;
};


template<typename Expr, typename Env>
struct EvalExpr;


template<int N, typename Env>
struct EvalExpr<NumberLiteral<N>, Env> {
    using result = EvalResult<IntValue<N>, Env>;
};


template<typename Name, typename Env>
struct EvalExpr<Variable<Name>, Env> {
    using result = EvalResult<Lookup_t<Env, Name>, Env>;
};


template<typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '+', Right>, Env> {
    using left_result = typename EvalExpr<Left, Env>::result;
    using right_result = typename EvalExpr<Right, typename left_result::env>::result;
    using value = typename left_result::value::template Add<typename right_result::value>;

    using result = EvalResult<value, typename right_result::env>;
};

template<typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '-', Right>, Env> {
    using left_result = typename EvalExpr<Left, Env>::result;
    using right_result = typename EvalExpr<Right, typename left_result::env>::result;
    using value = typename left_result::value::template Sub<typename right_result::value>;

    using result = EvalResult<value, typename right_result::env>;
};

template<typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '*', Right>, Env> {
    using left_result = typename EvalExpr<Left, Env>::result;
    using right_result = typename EvalExpr<Right, typename left_result::env>::result;
    using value = typename left_result::value::template Mul<typename right_result::value>;

    using result = EvalResult<value, typename right_result::env>;
};

template<typename Left, typename Right, typename Env>
struct EvalExpr<BinaryOp<Left, '/', Right>, Env> {
    using left_result = typename EvalExpr<Left, Env>::result;
    using right_result = typename EvalExpr<Right, typename left_result::env>::result;
    using value = typename left_result::value::template Div<typename right_result::value>;

    using result = EvalResult<value, typename right_result::env>;
};


template<typename Stmt, typename Env>
struct EvalStmt;


template<typename Name, typename Expr, typename Env>
struct EvalStmt<Assignment<Name, Expr>, Env> {
    using expr_result = typename EvalExpr<Expr, Env>::result;
    using new_env = Update_t<typename expr_result::env, Name, typename expr_result::value>;

    using result = EvalResult<NoneValue, new_env>;
};


template<typename Expr, typename Env>
struct EvalStmt<ExpressionStatement<Expr>, Env> {
    using expr_result = typename EvalExpr<Expr, Env>::result;

    using result = EvalResult<typename expr_result::value, typename expr_result::env>;
};


template<typename Stmts, typename Env>
struct EvalStmts;

template<typename Env>
struct EvalStmts<EmptyList, Env> {
    using result = EvalResult<NoneValue, Env>;
};

template<typename Stmt, typename... Rest, typename Env>
struct EvalStmts<TypeList<Stmt, Rest...>, Env> {
    using stmt_result = typename EvalStmt<Stmt, Env>::result;

    
    using result = std::conditional_t<
        IsEmpty_v<TypeList<Rest...>>,
        typename EvalStmt<Stmt, Env>::result,
        
        typename EvalStmts<TypeList<Rest...>, typename stmt_result::env>::result
    >;
};


template<typename Stmts, typename Env = EmptyEnv>
using Eval = typename EvalStmts<Stmts, Env>::result;


template<typename Result>
using GetValue = typename Result::value;


template<typename Result>
using GetEnv = typename Result::env;

} 
