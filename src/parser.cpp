#include "parser.h"
#include <iostream>
#include <algorithm>

// Initialize parser with tokens
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentPosition(0) {}

// Parse entire program
std::shared_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

// Parse complete program and represent as a sequence of statements
std::shared_ptr<ASTNode> Parser::parseProgram() {
    auto programNode = std::make_shared<ASTNode>(ASTNodeType::PROGRAM, Token(TokenType::UNKNOWN, "", 0, 0));
    
    while (!isAtEnd()) {
        // Skip whitespace and semicolons
        while (!isAtEnd() && (isWhitespace(peek()) || peek().type == TokenType::SEMICOLON)) {
            if (peek().type == TokenType::SEMICOLON) {
                std::cerr << "Warning: Extra semicolon at line " << peek().line 
                          << ", column " << peek().column << std::endl;
            }
            advance();
        }
        
        if (isAtEnd()) break;
        
        if (match(TokenType::DECLARE)) {
            auto node = parseDeclaration();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::IDENTIFIER)) {
            auto node = parseAssignment();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::IF)) {
            auto node = parseIfStatement();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::WHILE)) {
            auto node = parseWhileStatement();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::PUT)) {
            auto node = parsePutStatement();
            if (node) programNode->children.push_back(node);
        } else if (!isWhitespace(peek())) {
            std::cerr << "Unexpected token: " << peek().lexeme 
                      << " at line " << peek().line 
                      << ", column " << peek().column << std::endl;
            advance();
        } else {
            advance();
        }
    }
    
    return programNode;
}

// Parse declarations
std::shared_ptr<ASTNode> Parser::parseDeclaration() {
    // Store declare token
    auto declareToken = previous();

    // Assume identifier is next
    auto identifierToken = advance();

    std::shared_ptr<ASTNode> valueNode = nullptr;
    if (match(TokenType::ASSIGN)) {
        valueNode = parseExpression();
    }

    // Check for semicolon
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after declaration at line " << peek().line
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    // Create the declaration AST node
    auto declarationNode = std::make_shared<ASTNode>(ASTNodeType::DECLARATION, declareToken);
    declarationNode->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, identifierToken));
    if (valueNode) {
        declarationNode->children.push_back(valueNode);
    }

    return declarationNode;
}

// Parses assignments
std::shared_ptr<ASTNode> Parser::parseAssignment() {
    // Store identifier token
    auto identifierToken = previous();

    // Check to make sure assignment operator is next
    if (!match(TokenType::ASSIGN)) {
        std::cerr << "Expected '<-' after identifier at line " 
                  << peek().line << ", column " 
                  << peek().column << std::endl;
        return nullptr;
    }

    // Parse the variable's assigned value
    auto valueNode = parseExpression();

    // Check for semicolon
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after assignment at line " << peek().line
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    // Create the assignment AST node
    auto assignmentNode = std::make_shared<ASTNode>(ASTNodeType::ASSIGNMENT, identifierToken);
    assignmentNode->children.push_back(valueNode);

    return assignmentNode;
}

// Parse expressions
std::shared_ptr<ASTNode> Parser::parseExpression() {
    if (match(TokenType::OPEN_PAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::CLOSE_PAREN)) {
            std::cerr << "Expected ')' at line " << peek().line << ", column " << peek().column << std::endl;
            return nullptr;
        }
        return expr;
    }
    
    auto left = parsePrimary();
    while (match(TokenType::PLUS) || match(TokenType::MINUS) || 
           match(TokenType::STAR) || match(TokenType::SLASH) || 
           match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL) || 
           match(TokenType::LESS) || match(TokenType::GREATER) || 
           match(TokenType::LESS_EQUAL) || match(TokenType::GREATER_EQUAL)) {
        auto operatorToken = previous();
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


std::shared_ptr<ASTNode> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        return std::make_shared<ASTNode>(ASTNodeType::NUMBER, previous());
    } else if (match(TokenType::IDENTIFIER)) {
        return std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, previous());
    } else if (match(TokenType::STRING)) {
        return std::make_shared<ASTNode>(ASTNodeType::STRING, previous());
    } else if (match(TokenType::OPEN_PAREN)) {
        auto expr = parseExpression();
        if (!match(TokenType::CLOSE_PAREN)) {
            std::cerr << "Expected ')' at line " << peek().line << ", column " << peek().column << std::endl;
        }
        return expr;
    }

    std::cerr << "Unexpected token in expression: " << peek().lexeme 
              << " at line " << peek().line 
              << ", column " << peek().column << std::endl;
    return std::make_shared<ASTNode>(ASTNodeType::UNKNOWN, Token(TokenType::UNKNOWN, "", 0, 0));
}

// Parse if statements
std::shared_ptr<ASTNode> Parser::parseIfStatement() {
    auto ifToken = previous();
    
    auto conditionNode = parseExpression();
    if (!conditionNode) {
        return nullptr;
    }

    if (!match(TokenType::THEN)) {
        std::cerr << "Expected 'then' after if condition at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }

    auto ifBlockNode = parseBlock();
    if (!ifBlockNode) {
        return nullptr;
    }

    auto ifNode = std::make_shared<ASTNode>(ASTNodeType::IF_STATEMENT, ifToken);
    ifNode->children.push_back(conditionNode);
    ifNode->children.push_back(ifBlockNode);

    while (peek().type == TokenType::ELSEIF) {
        advance(); // Consume ELSEIF
        auto elseifCondition = parseExpression();
        if (!elseifCondition) {
            return nullptr;
        }

        if (!match(TokenType::THEN)) {
            std::cerr << "Expected 'then' after elseif condition at line " 
                      << peek().line << ", column " << peek().column << std::endl;
            return nullptr;
        }

        auto elseifBlock = parseBlock();
        if (!elseifBlock) {
            return nullptr;
        }

        auto elseifNode = std::make_shared<ASTNode>(ASTNodeType::ELSEIF_STATEMENT, previous());
        elseifNode->children.push_back(elseifCondition);
        elseifNode->children.push_back(elseifBlock);
        ifNode->children.push_back(elseifNode);
    }

    if (peek().type == TokenType::ELSE) {
        advance(); // Consume ELSE
        auto elseBlock = parseBlock();
        if (!elseBlock) {
            return nullptr;
        }
        auto elseNode = std::make_shared<ASTNode>(ASTNodeType::ELSE_STATEMENT, previous());
        elseNode->children.push_back(elseBlock);
        ifNode->children.push_back(elseNode);
    }

    if (!match(TokenType::END_IF)) {
        std::cerr << "Expected 'end if' at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    // Require semicolon after end if
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after 'end if' at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    return ifNode;
}

// Parse while loops
std::shared_ptr<ASTNode> Parser::parseWhileStatement() {
    auto whileToken = previous();
    
    auto conditionNode = parseExpression();
    if (!conditionNode) {
        return nullptr;
    }

    if (!match(TokenType::LOOP)) {
        std::cerr << "Expected 'loop' after while condition at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }

    auto blockNode = parseBlock();
    if (!blockNode) {
        return nullptr;
    }

    if (!match(TokenType::END_LOOP)) {
        std::cerr << "Expected 'end loop' at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    // Require semicolon after end loop
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after 'end loop' at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    auto whileNode = std::make_shared<ASTNode>(ASTNodeType::WHILE_STATEMENT, whileToken);
    whileNode->children.push_back(conditionNode);
    whileNode->children.push_back(blockNode);
    return whileNode;
}

// Parse blocks of code in braces
std::shared_ptr<ASTNode> Parser::parseBlock() {
    auto blockNode = std::make_shared<ASTNode>(ASTNodeType::BLOCK, Token(TokenType::UNKNOWN, "", 0, 0));

    while (!isAtEnd() && !check(TokenType::END_IF) && !check(TokenType::END_LOOP) && 
           !check(TokenType::ELSE) && !check(TokenType::ELSEIF)) {
        
        // Skip whitespace
        while (!isAtEnd() && isWhitespace(peek())) {
            advance();
        }
        
        if (isAtEnd() || check(TokenType::END_IF) || check(TokenType::END_LOOP) || 
            check(TokenType::ELSE) || check(TokenType::ELSEIF)) break;
        
        // Only warn about truly extra semicolons
        if (match(TokenType::SEMICOLON)) {
            if (blockNode->children.empty()) {
                std::cerr << "Warning: Extra semicolon at line " << previous().line 
                          << ", column " << previous().column << std::endl;
            }
            continue;
        }
        
        if (match(TokenType::DECLARE)) {
            auto declNode = parseDeclaration();
            if (declNode) blockNode->children.push_back(declNode);
        } else if (match(TokenType::IDENTIFIER)) {
            auto assignNode = parseAssignment();
            if (assignNode) blockNode->children.push_back(assignNode);
        } else if (match(TokenType::IF)) {
            auto ifNode = parseIfStatement();
            if (ifNode) blockNode->children.push_back(ifNode);
        } else if (match(TokenType::WHILE)) {
            auto whileNode = parseWhileStatement();
            if (whileNode) blockNode->children.push_back(whileNode);
        } else if (match(TokenType::PUT)) {
            auto putNode = parsePutStatement();
            if (putNode) blockNode->children.push_back(putNode);
        } else if (!isWhitespace(peek())) {
            std::cerr << "Unexpected token in block: " << peek().lexeme 
                      << " at line " << peek().line 
                      << ", column " << peek().column << std::endl;
            advance();
        } else {
            advance();
        }
    }

    return blockNode;
}

// Parse put statements
std::shared_ptr<ASTNode> Parser::parsePutStatement() {
    auto putToken = previous();
    
    // Handle opening parenthesis
    if (!match(TokenType::OPEN_PAREN)) {
        std::cerr << "Expected '(' after put at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    auto expressionNode = parseExpression();
    if (!expressionNode) {
        return nullptr;
    }
    
    // Handle closing parenthesis
    if (!match(TokenType::CLOSE_PAREN)) {
        std::cerr << "Expected ')' after put expression at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    // Handle semicolon
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after put statement at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    auto putNode = std::make_shared<ASTNode>(ASTNodeType::PUT_STATEMENT, putToken);
    putNode->children.push_back(expressionNode);
    return putNode;
}

// Check if at end of tokens
bool Parser::isAtEnd() const {
    return currentPosition >= tokens.size();
}

// Advance to next token and return previous token
const Token& Parser::advance() {
    if (!isAtEnd()) {
        currentPosition++;
    }
    return previous();
}

// Peek at current token
const Token& Parser::peek() const {
    return tokens[currentPosition];
}

// Return previous token
const Token& Parser::previous() const {
    return tokens[currentPosition - 1];
}

// Matches a token if it is of the given type
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// Check if current token matches given type
bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::isWhitespace(const Token& token) const {
    return token.type == TokenType::UNKNOWN && 
           (token.lexeme.empty() || 
            std::all_of(token.lexeme.begin(), token.lexeme.end(), ::isspace));
}