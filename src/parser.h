#pragma once
#include <vector>
#include <memory>
#include "lexer.h"
#include "symbol_table.h"

enum class ASTNodeType {
    PROGRAM,
    DECLARATION,
    ASSIGNMENT,
    IF_STATEMENT,
    ELSEIF_STATEMENT,
    ELSE_STATEMENT,
    WHILE_STATEMENT,
    PUT_STATEMENT,
    BLOCK,
    BINARY_OP,
    NUMBER,
    STRING,
    IDENTIFIER,
    PARAMETER,
    PROCEDURE,
    PROCEDURE_CALL,
    RETURN_STATEMENT,
    UNKNOWN
};

class ASTNode {
public:
    ASTNode(ASTNodeType type, const Token& token) : type(type), token(token) {}
    ASTNodeType type;
    Token token;
    std::vector<std::shared_ptr<ASTNode>> children;
};

class StatementParser;
class ExpressionParser;

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ASTNode> parse();
    
    // These need to be public for StatementParser and ExpressionParser
    bool isAtEnd() const;
    const Token& advance();
    const Token& peek() const;
    const Token& previous() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    bool isWhitespace(const Token& token) const;
    SymbolTable& getSymbolTable() { return symbolTable; }

    std::vector<Token> tokens;
    size_t currentPosition;
    SymbolTable symbolTable;
    std::unique_ptr<StatementParser> statementParser;
    std::unique_ptr<ExpressionParser> expressionParser;
    
    std::shared_ptr<ASTNode> parseProgram();
};