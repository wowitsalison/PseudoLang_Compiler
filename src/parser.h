#pragma once
#define PARSER_H

#include <vector>
#include <memory>
#include "lexer.h"
#include "symbol_table.h"

enum class ASTNodeType {
    PROGRAM,
    DECLARATION,
    ASSIGNMENT,
    BINARY_OP,
    IDENTIFIER,
    NUMBER,
    STRING,
    IF_STATEMENT,
    ELSEIF_STATEMENT,
    ELSE_STATEMENT,
    WHILE_STATEMENT,
    BLOCK,
    PUT_STATEMENT,
    PROCEDURE,
    PROCEDURE_CALL,
    PARAMETER,
    RETURN_STATEMENT,
    UNKNOWN
};

class ASTNode {
public:
    ASTNodeType type;
    std::vector<std::shared_ptr<ASTNode>> children;
    Token token;

    ASTNode(ASTNodeType type, const Token& token) : type(type), token(token) {}
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ASTNode> parse(); 

private:
    std::vector<Token> tokens;
    size_t currentPosition;
    SymbolTable symbolTable;

    // Parsing methods
    std::shared_ptr<ASTNode> parseProgram();
    std::shared_ptr<ASTNode> parseDeclaration();
    std::shared_ptr<ASTNode> parseAssignment();
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parseBlock();
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parsePutStatement();
    std::shared_ptr<ASTNode> parseProcedure();
    std::shared_ptr<ASTNode> parseProcedureCall();
    std::shared_ptr<ASTNode> parseProcedureCallStatement();
    std::shared_ptr<ASTNode> parseReturnStatement();

    // Utility methods
    bool isAtEnd() const;
    const Token& advance();
    const Token& peek() const;
    const Token& previous() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    bool isWhitespace(const Token& token) const;
};
