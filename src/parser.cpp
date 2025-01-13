#include "parser.h"
#include <iostream>
#include <algorithm>

// Initialize parser with tokens
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentPosition(0) {
    symbolTable.enterScope(); // Enter global scope
}

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
        } else if (match(TokenType::PROCEDURE)) {  
            auto node = parseProcedure();
            if (node) programNode->children.push_back(node);
        } else if (match(TokenType::IDENTIFIER)) {
            Token identToken = previous();
            if (peek().type == TokenType::OPEN_PAREN) {
                currentPosition--; // Back up so parseProcedureCall sees the identifier
                auto node = parseProcedureCall();
                if (node) programNode->children.push_back(node);
            } else if (peek().type == TokenType::ASSIGN) {
                currentPosition--; // Back up so parseAssignment sees the identifier
                auto node = parseAssignment();
                if (node) programNode->children.push_back(node);
            } else {
                std::cerr << "Expected '<-' or '(' after identifier at line " 
                        << identToken.line << ", column " << identToken.column << std::endl;
            }
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
    auto identifierToken = advance();

    // Check to make sure assignment operator is next
    if (!match(TokenType::ASSIGN)) {
        std::cerr << "Expected '<-' after identifier at line " 
                  << peek().line << ", column " 
                  << peek().column << std::endl;
        return nullptr;
    }

    // Parse the variable's assigned value
    auto valueNode = parseExpression();
    if (!valueNode) {
        return nullptr;
    }

    // Check for semicolon
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after assignment at line " << peek().line
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }

    // Check if variable is declared
    if (!symbolTable.isVariableDeclared(identifierToken.lexeme)) {
        std::cerr << "Undeclared variable: " << identifierToken.lexeme 
                  << " at line " << identifierToken.line 
                  << ", column " << identifierToken.column << std::endl;
        advance(); // Advance to avoid infinite loop
        return nullptr;
    }

    // Create the assignment AST node
    auto assignmentNode = std::make_shared<ASTNode>(ASTNodeType::ASSIGNMENT, identifierToken);
    assignmentNode->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, identifierToken));
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
    
    // Check for procedure call first
    if (check(TokenType::IDENTIFIER) && tokens[currentPosition + 1].type == TokenType::OPEN_PAREN) {
        return parseProcedureCall();
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
        if (!symbolTable.isVariableDeclared(previous().lexeme)) {
            std::cerr << "Undeclared variable: " << previous().lexeme 
                      << " at line " << previous().line 
                      << ", column " << previous().column << std::endl;
        }
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

    // Enter new scope for if block
    symbolTable.enterScope();

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

    // Exit scope for if block
    symbolTable.exitScope();

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

    // Enter new scope for block
    symbolTable.enterScope();

    while (!isAtEnd() && !check(TokenType::END_IF) && !check(TokenType::END_LOOP) && 
           !check(TokenType::ELSE) && !check(TokenType::ELSEIF) && 
           !check(TokenType::END_PROCEDURE)) {  
        
        // Skip whitespace
        while (!isAtEnd() && isWhitespace(peek())) {
            advance();
        }
        
        if (isAtEnd() || check(TokenType::END_IF) || check(TokenType::END_LOOP) || 
            check(TokenType::ELSE) || check(TokenType::ELSEIF) || 
            check(TokenType::END_PROCEDURE)) break;  
        
        if (match(TokenType::RETURN)) { 
            auto returnNode = parseReturnStatement();
            if (returnNode) blockNode->children.push_back(returnNode);
        } else if (match(TokenType::DECLARE)) {
            auto declNode = parseDeclaration();
            if (declNode) blockNode->children.push_back(declNode);
        } else if (match(TokenType::IDENTIFIER)) {
            Token identToken = previous();
            if (peek().type == TokenType::OPEN_PAREN) {
                // This is a procedure call
                currentPosition--; // Back up so parseProcedureCall sees the identifier
                auto node = parseProcedureCallStatement();
                if (node) blockNode->children.push_back(node);
                else advance();
            } else if (peek().type == TokenType::ASSIGN) {
                // This is an assignment
                currentPosition--; // Back up so parseAssignment sees the identifier
                auto node = parseAssignment();
                if (node) blockNode->children.push_back(node);
                else advance();
            } else {
                std::cerr << "Expected '<-' or '(' after identifier at line " 
                        << identToken.line << ", column " << identToken.column << std::endl;
                advance();
            }
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

    // Exit scope for block
    symbolTable.exitScope();

    return blockNode;
}

std::shared_ptr<ASTNode> Parser::parseProcedure() {
    auto procToken = previous();
    
    // Parse procedure name
    if (!match(TokenType::IDENTIFIER)) {
        std::cerr << "Expected procedure name at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }
    auto nameNode = std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, previous());
    
    // Parse parameters
    std::vector<std::shared_ptr<ASTNode>> params;
    if (!match(TokenType::OPEN_PAREN)) {
        std::cerr << "Expected '(' after procedure name at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    // Parse parameter list
    while (!check(TokenType::CLOSE_PAREN)) {
        if (!match(TokenType::IDENTIFIER)) {
            std::cerr << "Expected parameter name at line " << peek().line 
                      << ", column " << peek().column << std::endl;
            return nullptr;
        }
        params.push_back(std::make_shared<ASTNode>(ASTNodeType::PARAMETER, previous()));
        
        if (!check(TokenType::CLOSE_PAREN)) {
            if (!match(TokenType::COMMA)) {
                std::cerr << "Expected ',' between parameters at line " 
                          << peek().line << ", column " << peek().column << std::endl;
                return nullptr;
            }
        }
    }
    
    if (!match(TokenType::CLOSE_PAREN)) {
        std::cerr << "Expected ')' after parameters at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    // Parse procedure body
    if (!match(TokenType::BEGIN)) {
        std::cerr << "Expected 'begin' after procedure header at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    auto bodyNode = parseBlock();
    
    if (!match(TokenType::END_PROCEDURE)) {
        std::cerr << "Expected 'end procedure' at line " << peek().line 
                  << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after 'end procedure' at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    auto procNode = std::make_shared<ASTNode>(ASTNodeType::PROCEDURE, procToken);
    procNode->children.push_back(nameNode);
    for (auto& param : params) {
        procNode->children.push_back(param);
    }
    procNode->children.push_back(bodyNode);
    
    return procNode;
}

//
std::shared_ptr<ASTNode> Parser::parseProcedureCall() {
    if (!match(TokenType::IDENTIFIER)) {
        std::cerr << "Expected procedure name at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    auto procName = previous();
    auto callNode = std::make_shared<ASTNode>(ASTNodeType::PROCEDURE_CALL, procName);
    
    if (!match(TokenType::OPEN_PAREN)) {
        std::cerr << "Expected '(' after procedure name at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    // Parse arguments
    while (!check(TokenType::CLOSE_PAREN)) {
        auto arg = parseExpression();
        if (!arg) {
            return nullptr;
        }
        callNode->children.push_back(arg);
        
        if (!check(TokenType::CLOSE_PAREN)) {
            if (!match(TokenType::COMMA)) {
                std::cerr << "Expected ',' between arguments at line " 
                          << peek().line << ", column " << peek().column << std::endl;
                return nullptr;
            }
        }
    }
    
    if (!match(TokenType::CLOSE_PAREN)) {
        std::cerr << "Expected ')' after arguments at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    // Don't require semicolon here - let the calling context handle it
    return callNode;
}

std::shared_ptr<ASTNode> Parser::parseProcedureCallStatement() {
    auto callNode = parseProcedureCall();
    if (!callNode) {
        return nullptr;
    }
    
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after procedure call at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    return callNode;
}

//
std::shared_ptr<ASTNode> Parser::parseReturnStatement() {
    auto returnToken = previous();
    
    auto expressionNode = parseExpression();
    if (!expressionNode) {
        return nullptr;
    }
    
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after return statement at line " 
                  << peek().line << ", column " << peek().column << std::endl;
        return nullptr;
    }
    
    auto returnNode = std::make_shared<ASTNode>(ASTNodeType::RETURN_STATEMENT, returnToken);
    returnNode->children.push_back(expressionNode);
    return returnNode;
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