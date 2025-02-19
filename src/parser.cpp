#include "parser.h"
#include "statement_parser.h"
#include "expression_parser.h"
#include <iostream>
#include <algorithm>

// Constructor for the Parser class
Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), currentPosition(0) {
    symbolTable.enterScope(); // Enter global scope
    statementParser = std::make_unique<StatementParser>(*this); // Initialize statement parser
    expressionParser = std::make_unique<ExpressionParser>(*this); // Initialize expression parser
}

// Parse entire program and return AST root node
std::shared_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

// Parse the entire program
std::shared_ptr<ASTNode> Parser::parseProgram() {
    auto programNode = std::make_shared<ASTNode>(ASTNodeType::PROGRAM, Token(TokenType::UNKNOWN, "", 0, 0));
    
    while (!isAtEnd()) {
        // Skip whitespaces and extra semicolons
        while (!isAtEnd() && (isWhitespace(peek()) || peek().type == TokenType::SEMICOLON)) {
            if (peek().type == TokenType::SEMICOLON) {
                // Warn about extra semicolons
                std::cerr << "Warning: Extra semicolon at line " << peek().line 
                          << ", column " << peek().column << std::endl;
            }
            advance();
        }
        
        if (isAtEnd()) break; // Check for end of file
        
        // Parse statements based on current token type
        if (match(TokenType::DECLARE)) {
            auto node = statementParser->parseDeclaration();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::PROCEDURE)) {
            auto node = statementParser->parseProcedure();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::IDENTIFIER)) { // Can be procedure calls or assignments
            Token identToken = previous();
            if (peek().type == TokenType::OPEN_PAREN) {
                currentPosition--; // Back up so parseProcedureCall sees the identifier
                auto node = statementParser->parseProcedureCall();
                if (node) programNode->children.push_back(node);
            } else if (peek().type == TokenType::ASSIGN) {
                currentPosition--; // Back up so parseAssignment sees the identifier
                auto node = statementParser->parseAssignment();
                if (node) programNode->children.push_back(node);
            } else {
                // Report unexpected token
                std::cerr << "Expected '<-' or '(' after identifier at line " 
                        << identToken.line << ", column " << identToken.column << std::endl;
            }
        } else if (match(TokenType::IF)) {
            auto node = statementParser->parseIfStatement();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::WHILE)) {
            auto node = statementParser->parseWhileStatement();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::PUT)) {
            auto node = statementParser->parsePutStatement();
            if (node) programNode->children.push_back(node);
        } else if (!isWhitespace(peek())) {
            std::cerr << "Unexpected token: " << peek().lexeme 
                      << " at line " << peek().line 
                      << ", column " << peek().column << std::endl;
            advance(); // Skip the unexpected token
        } else {
            advance();
        }
    }
    
    return programNode; // Return the root node of the AST
}

// Utility method
bool Parser::isAtEnd() const {
    return currentPosition >= tokens.size();
}

// Advances the current position and returns previous token
const Token& Parser::advance() {
    if (!isAtEnd()) {
        currentPosition++;
    }
    return previous();
}

// Returns the current token
const Token& Parser::peek() const {
    return tokens[currentPosition];
}

// Returns the previous token
const Token& Parser::previous() const {
    return tokens[currentPosition - 1];
}

// Matches token types and advances if matched
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// Checks if the current token matches the given type
bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

// Checks if the token is whitespace
bool Parser::isWhitespace(const Token& token) const {
    return token.type == TokenType::UNKNOWN && 
           (token.lexeme.empty() || 
            std::all_of(token.lexeme.begin(), token.lexeme.end(), ::isspace));
}