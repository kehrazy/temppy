#pragma once

#include "ast.hpp"
#include "../lexer/token.hpp"
#include "../core/list.hpp"

namespace temppy {

template<typename AST, typename RemainingTokens>
struct ParseResult {
    using ast = AST;
    using remaining = RemainingTokens;
};

template<typename Tokens>
struct ParseExpression;

template<typename Tokens>
struct ParseTerm;

template<typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseFactor;

template<typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseParenExpr;

template<typename Tokens>
struct ParseParenExpr<Tokens, true> {
    using result = ParseResult<NumberLiteral<0>, Tokens>;
};

template<typename Tokens>
struct ParseParenExpr<Tokens, false> {
    using expr_result = typename ParseExpression<Tokens>::result;
    using after_expr = typename expr_result::remaining;

    using result = typename std::conditional<
        IsEmpty_v<after_expr>,
        ParseResult<typename expr_result::ast, after_expr>,
        ParseResult<typename expr_result::ast, Tail_t<after_expr>>
    >::type;
};


template<typename Tokens, bool Empty>
struct ParseFactor;

template<typename Tokens>
struct ParseFactor<Tokens, true> {
    using result = ParseResult<NumberLiteral<0>, Tokens>;
};

template<typename Tokens>
struct ParseFactor<Tokens, false> {
    using current = Head_t<Tokens>;
    using rest = Tail_t<Tokens>;

    using result = typename std::conditional<
        current::type == TokenType::Number,
        ParseResult<NumberLiteral<current::int_data>, rest>,

        typename std::conditional<
            current::type == TokenType::Identifier,
            ParseResult<Variable<typename current::data>, rest>,

            typename std::conditional<
                current::type == TokenType::LParen,
                typename ParseParenExpr<rest>::result,
                ParseResult<NumberLiteral<0>, Tokens>
            >::type
        >::type
    >::type;
};


template<typename Tokens, typename LeftAST, bool Empty = IsEmpty_v<Tokens>>
struct ParseTermRest;

template<typename Tokens, typename LeftAST>
struct ParseTermRest<Tokens, LeftAST, true> {
    using result = ParseResult<LeftAST, Tokens>;
};

template<typename Tokens, typename LeftAST>
struct ParseTermRest<Tokens, LeftAST, false> {
    using current = Head_t<Tokens>;
    using rest = Tail_t<Tokens>;

    using result = typename std::conditional<
        current::type == TokenType::Star,
        typename ParseTermRest<
            typename ParseFactor<rest>::result::remaining,
            BinaryOp<LeftAST, '*', typename ParseFactor<rest>::result::ast>
        >::result,

        typename std::conditional<
            current::type == TokenType::Slash,
            typename ParseTermRest<
                typename ParseFactor<rest>::result::remaining,
                BinaryOp<LeftAST, '/', typename ParseFactor<rest>::result::ast>
            >::result,
            ParseResult<LeftAST, Tokens>
        >::type
    >::type;
};

template<typename Tokens>
struct ParseTerm {
    using factor_result = typename ParseFactor<Tokens>::result;
    using result = typename ParseTermRest<
        typename factor_result::remaining,
        typename factor_result::ast
    >::result;
};


template<typename Tokens, typename LeftAST, bool Empty = IsEmpty_v<Tokens>>
struct ParseExpressionRest;

template<typename Tokens, typename LeftAST>
struct ParseExpressionRest<Tokens, LeftAST, true> {
    using result = ParseResult<LeftAST, Tokens>;
};

template<typename Tokens, typename LeftAST>
struct ParseExpressionRest<Tokens, LeftAST, false> {
    using current = Head_t<Tokens>;
    using rest = Tail_t<Tokens>;

    using result = typename std::conditional<
        current::type == TokenType::Plus,
        typename ParseExpressionRest<
            typename ParseTerm<rest>::result::remaining,
            BinaryOp<LeftAST, '+', typename ParseTerm<rest>::result::ast>
        >::result,

        typename std::conditional<
            current::type == TokenType::Minus,
            typename ParseExpressionRest<
                typename ParseTerm<rest>::result::remaining,
                BinaryOp<LeftAST, '-', typename ParseTerm<rest>::result::ast>
            >::result,
            ParseResult<LeftAST, Tokens>
        >::type
    >::type;
};

template<typename Tokens>
struct ParseExpression {
    using term_result = typename ParseTerm<Tokens>::result;
    using result = typename ParseExpressionRest<
        typename term_result::remaining,
        typename term_result::ast
    >::result;
};

template<typename Name, typename Rest, bool Empty = IsEmpty_v<Rest>>
struct ParseAssignmentHelper;

template<typename Name, typename Rest>
struct ParseAssignmentHelper<Name, Rest, true> {
    using result = ParseResult<
        ExpressionStatement<Variable<Name>>,
        Rest
    >;
};

template<typename Name, typename Rest>
struct ParseAssignmentHelper<Name, Rest, false> {
    using second = Head_t<Rest>;
    using tokens_with_id = Concat_t<TypeList<IdentifierToken<Name>>, Rest>;

    using result = typename std::conditional<
        second::type == TokenType::Assign,
        ParseResult<
            Assignment<Name, typename ParseExpression<Tail_t<Rest>>::result::ast>,
            typename ParseExpression<Tail_t<Rest>>::result::remaining
        >,
        ParseResult<
            ExpressionStatement<typename ParseExpression<tokens_with_id>::result::ast>,
            typename ParseExpression<tokens_with_id>::result::remaining
        >
    >::type;
};

template<typename Name, typename Rest>
struct ReconstructIdentifierTokens {
    using type = TypeList<IdentifierToken<Name>, Rest>;
};


template<typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseStatement;

template<typename Tokens>
struct ParseStatement<Tokens, true> {
    using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template<typename Tokens>
struct ParseStatement<Tokens, false> {
    using current = Head_t<Tokens>;
    using rest = Tail_t<Tokens>;

    using result = typename std::conditional<
        current::type == TokenType::Eof,
        ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>,
        typename std::conditional<
            current::type == TokenType::Identifier,
            typename ParseAssignmentHelper<typename current::data, rest>::result,
            ParseResult<
                ExpressionStatement<typename ParseExpression<Tokens>::result::ast>,
                typename ParseExpression<Tokens>::result::remaining
            >
        >::type
    >::type;
};

template<typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct SkipNewlines;

template<typename Tokens>
struct SkipNewlines<Tokens, true> {
    using result = Tokens;
};

template<typename Tokens>
struct SkipNewlines<Tokens, false> {
    using current = Head_t<Tokens>;

    using result = typename std::conditional<
        current::type == TokenType::Newline,
        typename SkipNewlines<Tail_t<Tokens>>::result,
        Tokens
    >::type;
};


template<typename Tokens, typename StmtList = EmptyList, bool Empty = IsEmpty_v<Tokens>>
struct ParseProgram;

template<typename Tokens, typename StmtList>
struct ParseProgram<Tokens, StmtList, true> {
    using result = StmtList;
};

template<typename Tokens, typename StmtList, bool IsEof>
struct ParseProgramHelper;

template<typename Tokens, typename StmtList>
struct ParseProgramHelper<Tokens, StmtList, true> {
    using result = StmtList;
};

template<typename Tokens, typename StmtList>
struct ParseProgramHelper<Tokens, StmtList, false> {
    using stmt_result = typename ParseStatement<Tokens>::result;
    using result = typename ParseProgram<
        typename stmt_result::remaining,
        Append_t<StmtList, typename stmt_result::ast>
    >::result;
};

template<typename Tokens, typename StmtList>
struct ParseProgram<Tokens, StmtList, false> {
    using tokens_after_newlines = typename SkipNewlines<Tokens>::result;
    using current = Head_t<tokens_after_newlines>;

    using result = typename ParseProgramHelper<
        tokens_after_newlines,
        StmtList,
        current::type == TokenType::Eof
    >::result;
};

template<typename Tokens>
using Parse = typename ParseProgram<Tokens>::result;

} 
