#include "expression_parser.h"
#include <iostream>
#include "statement_parser.h"

// Parse an expression
std::shared_ptr<ASTNode> ExpressionParser::parseExpression() {
    if (parser.match(TokenType::OPEN_PAREN)) {
        auto expr = parseExpression();
        if (!parser.match(TokenType::CLOSE_PAREN)) {
            std::cerr << "Expected ')' at line " << parser.peek().line 
                      << ", column " << parser.peek().column << std::endl;
            return nullptr;
        }
        return expr;
    }
    
    // Check for procedure call first
    if (parser.check(TokenType::IDENTIFIER) && 
        parser.tokens[parser.currentPosition + 1].type == TokenType::OPEN_PAREN) {
        return parser.statementParser->parseProcedureCall();
    }
    
    // Check for assignment
    auto left = parsePrimary();
    while (parser.match(TokenType::PLUS) || parser.match(TokenType::MINUS) || 
           parser.match(TokenType::STAR) || parser.match(TokenType::SLASH) || 
           parser.match(TokenType::EQUAL) || parser.match(TokenType::NOT_EQUAL) || 
           parser.match(TokenType::LESS) || parser.match(TokenType::GREATER) || 
           parser.match(TokenType::LESS_EQUAL) || parser.match(TokenType::GREATER_EQUAL)) {
        auto operatorToken = parser.previous();
        auto right = parsePrimary();
        if (!right) {
            std::cerr << "Expected right operand after operator at line " 
                      << operatorToken.line << ", column " << operatorToken.column << std::endl;
            return nullptr;
        }
        auto binaryOpNode = std::make_shared<ASTNode>(ASTNodeType::BINARY_OP, operatorToken);
        binaryOpNode->children.push_back(left);
        binaryOpNode->children.push_back(right);
        left = binaryOpNode;
    }
    return left;
}

// Parse a primary expression
std::shared_ptr<ASTNode> ExpressionParser::parsePrimary() {
    if (parser.match(TokenType::NUMBER)) {
        return std::make_shared<ASTNode>(ASTNodeType::NUMBER, parser.previous());
    } else if (parser.match(TokenType::IDENTIFIER)) {
        if (!parser.getSymbolTable().isVariableDeclared(parser.previous().lexeme)) {
            std::cerr << "Undeclared variable: " << parser.previous().lexeme 
                      << " at line " << parser.previous().line 
                      << ", column " << parser.previous().column << std::endl;
        }
        return std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, parser.previous());
    } else if (parser.match(TokenType::STRING)) {
        return std::make_shared<ASTNode>(ASTNodeType::STRING, parser.previous());
    } else if (parser.match(TokenType::OPEN_PAREN)) {
        auto expr = parseExpression();
        if (!parser.match(TokenType::CLOSE_PAREN)) {
            std::cerr << "Expected ')' at line " << parser.peek().line 
                      << ", column " << parser.peek().column << std::endl;
        }
        return expr;
    }

    // Warn about unexpected token
    std::cerr << "Unexpected token in expression: " << parser.peek().lexeme 
              << " at line " << parser.peek().line 
              << ", column " << parser.peek().column << std::endl;
    return std::make_shared<ASTNode>(ASTNodeType::UNKNOWN, Token(TokenType::UNKNOWN, "", 0, 0));
}