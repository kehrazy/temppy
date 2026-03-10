#pragma once

#include <type_traits>

#include "../core/list.hpp"
#include "../lexer/token.hpp"
#include "ast.hpp"

namespace temppy {

template <typename AST, typename RemainingTokens> struct ParseResult {
  using ast = AST;
  using remaining = RemainingTokens;
};

template <typename Tokens> struct ParseExpression;

template <typename Tokens> struct ParseTerm;

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>> struct ParseFactor;

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseParenExpr;

template <typename Tokens> struct ParseParenExpr<Tokens, true> {
  using result = ParseResult<NumberLiteral<0>, Tokens>;
};

template <typename Tokens> struct ParseParenExpr<Tokens, false> {
  using expr_result = typename ParseExpression<Tokens>::result;
  using after_expr = typename expr_result::remaining;

  using result = typename std::conditional<
      IsEmpty_v<after_expr>, ParseResult<typename expr_result::ast, after_expr>,
      ParseResult<typename expr_result::ast, Tail_t<after_expr>>>::type;
};

template <typename Tokens, bool Empty> struct ParseFactor;

template <typename Tokens> struct ParseUnaryPlus {
  using factor_result = typename ParseFactor<Tokens>::result;
  using result = ParseResult<typename factor_result::ast,
                             typename factor_result::remaining>;
};

template <typename Tokens> struct ParseUnaryMinus {
  using factor_result = typename ParseFactor<Tokens>::result;
  using result = ParseResult<UnaryOp<'-', typename factor_result::ast>,
                             typename factor_result::remaining>;
};

template <typename Tokens> struct ParseFactor<Tokens, true> {
  using result = ParseResult<NumberLiteral<0>, Tokens>;
};

template <typename Tokens> struct ParseFactor<Tokens, false> {
  using current = Head_t<Tokens>;
  using rest = Tail_t<Tokens>;

  using result = typename std::conditional<
      current::type == TokenType::Number,
      ParseResult<NumberLiteral<current::int_data>, rest>,
      typename std::conditional<
          current::type == TokenType::Identifier,
          ParseResult<Variable<typename current::data>, rest>,
          typename std::conditional<
              current::type == TokenType::Plus,
              typename ParseUnaryPlus<rest>::result,
              typename std::conditional<
                  current::type == TokenType::Minus,
                  typename ParseUnaryMinus<rest>::result,
                  typename std::conditional<
                      current::type == TokenType::LParen,
                      typename ParseParenExpr<rest>::result,
                      ParseResult<NumberLiteral<0>, Tokens>>::type>::type>::
              type>::type>::type;
};

template <typename Tokens, typename LeftAST, bool Empty = IsEmpty_v<Tokens>>
struct ParseTermRest;

template <typename Tokens, typename LeftAST>
struct ParseTermRest<Tokens, LeftAST, true> {
  using result = ParseResult<LeftAST, Tokens>;
};

template <typename Tokens, typename LeftAST>
struct ParseTermRest<Tokens, LeftAST, false> {
  using current = Head_t<Tokens>;
  using rest = Tail_t<Tokens>;

  using result = typename std::conditional<
      current::type == TokenType::Star,
      typename ParseTermRest<
          typename ParseFactor<rest>::result::remaining,
          BinaryOp<LeftAST, '*',
                   typename ParseFactor<rest>::result::ast>>::result,
      typename std::conditional<
          current::type == TokenType::Slash,
          typename ParseTermRest<
              typename ParseFactor<rest>::result::remaining,
              BinaryOp<LeftAST, '/',
                       typename ParseFactor<rest>::result::ast>>::result,
          ParseResult<LeftAST, Tokens>>::type>::type;
};

template <typename Tokens> struct ParseTerm {
  using factor_result = typename ParseFactor<Tokens>::result;
  using result = typename ParseTermRest<typename factor_result::remaining,
                                        typename factor_result::ast>::result;
};

template <typename Tokens, typename LeftAST, bool Empty = IsEmpty_v<Tokens>>
struct ParseExpressionRest;

template <typename Tokens, typename LeftAST>
struct ParseExpressionRest<Tokens, LeftAST, true> {
  using result = ParseResult<LeftAST, Tokens>;
};

template <typename Tokens, typename LeftAST>
struct ParseExpressionRest<Tokens, LeftAST, false> {
  using current = Head_t<Tokens>;
  using rest = Tail_t<Tokens>;

  using result = typename std::conditional<
      current::type == TokenType::Plus,
      typename ParseExpressionRest<
          typename ParseTerm<rest>::result::remaining,
          BinaryOp<LeftAST, '+',
                   typename ParseTerm<rest>::result::ast>>::result,
      typename std::conditional<
          current::type == TokenType::Minus,
          typename ParseExpressionRest<
              typename ParseTerm<rest>::result::remaining,
              BinaryOp<LeftAST, '-',
                       typename ParseTerm<rest>::result::ast>>::result,
          ParseResult<LeftAST, Tokens>>::type>::type;
};

template <typename Tokens> struct ParseExpression {
  using term_result = typename ParseTerm<Tokens>::result;
  using result =
      typename ParseExpressionRest<typename term_result::remaining,
                                   typename term_result::ast>::result;
};

template <typename Tokens, TokenType Expected, bool Empty = IsEmpty_v<Tokens>>
struct ConsumeTokenIf;

template <typename Tokens, TokenType Expected>
struct ConsumeTokenIf<Tokens, Expected, true> {
  using result = Tokens;
};

template <typename Tokens, TokenType Expected>
struct ConsumeTokenIf<Tokens, Expected, false> {
  using current = Head_t<Tokens>;
  using result = typename std::conditional<current::type == Expected,
                                           Tail_t<Tokens>, Tokens>::type;
};

template <typename Tokens, typename Name, bool Empty = IsEmpty_v<Tokens>>
struct ConsumeIdentifierNamed;

template <typename Tokens, typename Name>
struct ConsumeIdentifierNamed<Tokens, Name, true> {
  using result = Tokens;
};

template <typename Tokens, typename Name>
struct ConsumeIdentifierNamed<Tokens, Name, false> {
  using current = Head_t<Tokens>;
  static constexpr bool matches = current::type == TokenType::Identifier &&
                                  std::is_same_v<typename current::data, Name>;

  using result =
      typename std::conditional<matches, Tail_t<Tokens>, Tokens>::type;
};

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>> struct SkipNewlines;

template <typename Tokens> struct SkipNewlines<Tokens, true> {
  using result = Tokens;
};

template <typename Tokens> struct SkipNewlines<Tokens, false> {
  using current = Head_t<Tokens>;

  using result =
      typename std::conditional<current::type == TokenType::Newline,
                                typename SkipNewlines<Tail_t<Tokens>>::result,
                                Tokens>::type;
};

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseStatement;

template <typename Tokens, typename StmtList = EmptyList,
          bool Empty = IsEmpty_v<Tokens>>
struct ParseBlockStatements;

template <typename Tokens, typename StmtList, bool Empty = IsEmpty_v<Tokens>>
struct ParseBlockStatementsAfterSkip;

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseIndentedSuite;

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseIndentedSuiteAfterSkip;

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseIfStatement;

template <typename Tokens, bool Empty = IsEmpty_v<Tokens>>
struct ParseForStatement;

template <typename LoopVar, typename Tokens> struct ParseForStatementImpl;

template <typename Name, typename Rest, bool Empty = IsEmpty_v<Rest>>
struct ParseAssignmentHelper;

template <typename Name, typename Rest> struct ParseIdentifierStatement;

template <typename Tokens, bool HasLoopVar> struct ParseForStatementDispatch;

template <typename Name, typename Rest, bool HasAssign>
struct ParseAssignmentDispatch;

template <typename Name, typename Rest, bool IsIf, bool IsFor>
struct ParseIdentifierStatementDispatch;

template <typename Tokens, bool IsEof, bool IsDedent, bool IsIdentifier>
struct ParseStatementDispatch;

template <typename Tokens, typename StmtList, bool EmptyAfterSkip>
struct ParseProgramDispatch;

template <typename Tokens, typename StmtList, bool Stop>
struct ParseProgramHelper;

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsRecurse {
  using stmt_result = typename ParseStatement<Tokens>::result;
  using result = typename ParseBlockStatements<
      typename stmt_result::remaining,
      Append_t<StmtList, typename stmt_result::ast>>::result;
};

template <typename Tokens, typename StmtList, bool IsDedent, bool IsEof>
struct ParseBlockStatementsDispatch;

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsDispatch<Tokens, StmtList, true, false> {
  using result = ParseResult<StmtList, Tail_t<Tokens>>;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsDispatch<Tokens, StmtList, false, true> {
  using result = ParseResult<StmtList, Tokens>;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsDispatch<Tokens, StmtList, false, false> {
  using result = typename ParseBlockStatementsRecurse<Tokens, StmtList>::result;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsDispatch<Tokens, StmtList, true, true> {
  using result = ParseResult<StmtList, Tail_t<Tokens>>;
};

template <typename Tokens, bool HasIndent>
struct ParseIndentedSuiteAfterSkipDispatch;

template <typename Tokens>
struct ParseIndentedSuiteAfterSkipDispatch<Tokens, true> {
  using result = typename ParseBlockStatements<Tail_t<Tokens>>::result;
};

template <typename Tokens>
struct ParseIndentedSuiteAfterSkipDispatch<Tokens, false> {
  using result = ParseResult<EmptyList, Tokens>;
};

template <typename Tokens> struct ParseForStatementDispatch<Tokens, true> {
  using current = Head_t<Tokens>;
  using result = typename ParseForStatementImpl<typename current::data,
                                                Tail_t<Tokens>>::result;
};

template <typename Tokens> struct ParseForStatementDispatch<Tokens, false> {
  using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template <typename Name, typename Rest>
struct ParseAssignmentDispatch<Name, Rest, true> {
  using expr_result = typename ParseExpression<Tail_t<Rest>>::result;
  using result = ParseResult<Assignment<Name, typename expr_result::ast>,
                             typename expr_result::remaining>;
};

template <typename Name, typename Rest>
struct ParseAssignmentDispatch<Name, Rest, false> {
  using tokens_with_id = Concat_t<TypeList<IdentifierToken<Name>>, Rest>;
  using expr_result = typename ParseExpression<tokens_with_id>::result;
  using result = ParseResult<ExpressionStatement<typename expr_result::ast>,
                             typename expr_result::remaining>;
};

template <typename Name, typename Rest>
struct ParseIdentifierStatementDispatch<Name, Rest, true, false> {
  using result = typename ParseIfStatement<Rest>::result;
};

template <typename Name, typename Rest>
struct ParseIdentifierStatementDispatch<Name, Rest, false, true> {
  using result = typename ParseForStatement<Rest>::result;
};

template <typename Name, typename Rest>
struct ParseIdentifierStatementDispatch<Name, Rest, false, false> {
  using result = typename ParseAssignmentHelper<Name, Rest>::result;
};

template <typename Name, typename Rest>
struct ParseIdentifierStatementDispatch<Name, Rest, true, true> {
  using result = typename ParseIfStatement<Rest>::result;
};

template <typename Tokens>
struct ParseStatementDispatch<Tokens, true, false, false> {
  using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template <typename Tokens>
struct ParseStatementDispatch<Tokens, false, true, false> {
  using result =
      ParseResult<ExpressionStatement<NumberLiteral<0>>, Tail_t<Tokens>>;
};

template <typename Tokens>
struct ParseStatementDispatch<Tokens, false, false, true> {
  using current = Head_t<Tokens>;
  using result = typename ParseIdentifierStatement<typename current::data,
                                                   Tail_t<Tokens>>::result;
};

template <typename Tokens>
struct ParseStatementDispatch<Tokens, false, false, false> {
  using expr_result = typename ParseExpression<Tokens>::result;
  using result = ParseResult<ExpressionStatement<typename expr_result::ast>,
                             typename expr_result::remaining>;
};

template <typename Tokens>
struct ParseStatementDispatch<Tokens, true, true, false> {
  using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template <typename Tokens, typename StmtList>
struct ParseProgramDispatch<Tokens, StmtList, true> {
  using result = typename ParseProgramHelper<Tokens, StmtList, true>::result;
};

template <typename Tokens, typename StmtList>
struct ParseProgramDispatch<Tokens, StmtList, false> {
  using current = Head_t<Tokens>;
  using result =
      typename ParseProgramHelper<Tokens, StmtList,
                                  (current::type == TokenType::Eof) ||
                                      (current::type ==
                                       TokenType::Dedent)>::result;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatements<Tokens, StmtList, true> {
  using result = ParseResult<StmtList, Tokens>;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatements<Tokens, StmtList, false> {
  using tokens_after_newlines = typename SkipNewlines<Tokens>::result;
  using result = typename ParseBlockStatementsAfterSkip<tokens_after_newlines,
                                                        StmtList>::result;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsAfterSkip<Tokens, StmtList, true> {
  using result = ParseResult<StmtList, Tokens>;
};

template <typename Tokens, typename StmtList>
struct ParseBlockStatementsAfterSkip<Tokens, StmtList, false> {
  using current = Head_t<Tokens>;
  using result = typename ParseBlockStatementsDispatch<
      Tokens, StmtList, current::type == TokenType::Dedent,
      current::type == TokenType::Eof>::result;
};

template <typename Tokens> struct ParseIndentedSuite<Tokens, true> {
  using result = ParseResult<EmptyList, Tokens>;
};

template <typename Tokens> struct ParseIndentedSuite<Tokens, false> {
  using after_newlines = typename SkipNewlines<Tokens>::result;
  using result = typename ParseIndentedSuiteAfterSkip<after_newlines>::result;
};

template <typename Tokens> struct ParseIndentedSuiteAfterSkip<Tokens, true> {
  using result = ParseResult<EmptyList, Tokens>;
};

template <typename Tokens> struct ParseIndentedSuiteAfterSkip<Tokens, false> {
  using current = Head_t<Tokens>;
  using result = typename ParseIndentedSuiteAfterSkipDispatch<
      Tokens, current::type == TokenType::Indent>::result;
};

template <typename Tokens> struct ParseIfStatement<Tokens, true> {
  using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template <typename Tokens> struct ParseIfStatement<Tokens, false> {
  using cond_result = typename ParseExpression<Tokens>::result;
  using after_colon = typename ConsumeTokenIf<typename cond_result::remaining,
                                              TokenType::Colon>::result;
  using suite_result = typename ParseIndentedSuite<after_colon>::result;

  using result = ParseResult<
      IfStatement<typename cond_result::ast, typename suite_result::ast>,
      typename suite_result::remaining>;
};

template <typename Tokens> struct ParseForStatement<Tokens, true> {
  using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template <typename Tokens> struct ParseForStatement<Tokens, false> {
  using current = Head_t<Tokens>;
  using result = typename ParseForStatementDispatch<
      Tokens, current::type == TokenType::Identifier>::result;
};

template <typename LoopVar, typename Tokens> struct ParseForStatementImpl {
  using after_in =
      typename ConsumeIdentifierNamed<Tokens, String<'i', 'n'>>::result;
  using after_range =
      typename ConsumeIdentifierNamed<after_in,
                                      String<'r', 'a', 'n', 'g', 'e'>>::result;
  using after_lparen =
      typename ConsumeTokenIf<after_range, TokenType::LParen>::result;
  using limit_result = typename ParseExpression<after_lparen>::result;
  using after_rparen = typename ConsumeTokenIf<typename limit_result::remaining,
                                               TokenType::RParen>::result;
  using after_colon =
      typename ConsumeTokenIf<after_rparen, TokenType::Colon>::result;
  using suite_result = typename ParseIndentedSuite<after_colon>::result;

  using result =
      ParseResult<ForRangeStatement<LoopVar, typename limit_result::ast,
                                    typename suite_result::ast>,
                  typename suite_result::remaining>;
};

template <typename Name, typename Rest>
struct ParseAssignmentHelper<Name, Rest, true> {
  using result = ParseResult<ExpressionStatement<Variable<Name>>, Rest>;
};

template <typename Name, typename Rest>
struct ParseAssignmentHelper<Name, Rest, false> {
  using second = Head_t<Rest>;
  using result = typename ParseAssignmentDispatch<
      Name, Rest, second::type == TokenType::Assign>::result;
};

template <typename Name, typename Rest> struct ParseIdentifierStatement {
  using result = typename ParseIdentifierStatementDispatch<
      Name, Rest, std::is_same_v<Name, String<'i', 'f'>>,
      std::is_same_v<Name, String<'f', 'o', 'r'>>>::result;
};

template <typename Tokens> struct ParseStatement<Tokens, true> {
  using result = ParseResult<ExpressionStatement<NumberLiteral<0>>, Tokens>;
};

template <typename Tokens> struct ParseStatement<Tokens, false> {
  using current = Head_t<Tokens>;
  using result =
      typename ParseStatementDispatch<Tokens, current::type == TokenType::Eof,
                                      current::type == TokenType::Dedent,
                                      current::type ==
                                          TokenType::Identifier>::result;
};

template <typename Tokens, typename StmtList = EmptyList,
          bool Empty = IsEmpty_v<Tokens>>
struct ParseProgram;

template <typename Tokens, typename StmtList>
struct ParseProgram<Tokens, StmtList, true> {
  using result = StmtList;
};

template <typename Tokens, typename StmtList, bool Stop>
struct ParseProgramHelper;

template <typename Tokens, typename StmtList>
struct ParseProgramHelper<Tokens, StmtList, true> {
  using result = StmtList;
};

template <typename Tokens, typename StmtList>
struct ParseProgramHelper<Tokens, StmtList, false> {
  using stmt_result = typename ParseStatement<Tokens>::result;
  using result = typename ParseProgram<
      typename stmt_result::remaining,
      Append_t<StmtList, typename stmt_result::ast>>::result;
};

template <typename Tokens, typename StmtList>
struct ParseProgram<Tokens, StmtList, false> {
  using tokens_after_newlines = typename SkipNewlines<Tokens>::result;
  using result =
      typename ParseProgramDispatch<tokens_after_newlines, StmtList,
                                    IsEmpty_v<tokens_after_newlines>>::result;
};

template <typename Tokens> using Parse = typename ParseProgram<Tokens>::result;

} // namespace temppy
