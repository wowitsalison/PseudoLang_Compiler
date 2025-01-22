#include "statement_parser.h"
#include "expression_parser.h"
#include <iostream>

// Constructor for StatementParser
std::shared_ptr<ASTNode> StatementParser::parseDeclaration() {
    // Store declare token
    auto declareToken = parser.previous();

    // Assume identifier is next
    auto identifierToken = parser.advance();

    std::shared_ptr<ASTNode> valueNode = nullptr;
    if (parser.match(TokenType::ASSIGN)) {
        valueNode = parser.expressionParser->parseExpression();
    }

    // Check for semicolon
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after declaration at line " << parser.peek().line
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Add variable to symbol table
    parser.getSymbolTable().declareVariable(identifierToken.lexeme, VariableType::INTEGER);

    // Create the declaration AST node
    auto declarationNode = std::make_shared<ASTNode>(ASTNodeType::DECLARATION, declareToken);
    declarationNode->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, identifierToken));
    if (valueNode) { // Add value node if it exists
        declarationNode->children.push_back(valueNode);
    }

    return declarationNode;
}
 // Parse an assignment statement
std::shared_ptr<ASTNode> StatementParser::parseAssignment() {
    // Store identifier token
    auto identifierToken = parser.advance();

    // Check to make sure assignment operator is next
    if (!parser.match(TokenType::ASSIGN)) {
        std::cerr << "Expected '<-' after identifier at line " 
                  << parser.peek().line << ", column " 
                  << parser.peek().column << std::endl;
        return nullptr;
    }

    // Parse the variable's assigned value
    auto valueNode = parser.expressionParser->parseExpression();
    if (!valueNode) {
        return nullptr;
    }

    // Check for semicolon
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after assignment at line " << parser.peek().line
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Check if variable is declared
    if (!parser.getSymbolTable().isVariableDeclared(identifierToken.lexeme)) {
        std::cerr << "Undeclared variable: " << identifierToken.lexeme 
                  << " at line " << identifierToken.line 
                  << ", column " << identifierToken.column << std::endl;
        return nullptr;
    }

    // Create the assignment AST node
    auto assignmentNode = std::make_shared<ASTNode>(ASTNodeType::ASSIGNMENT, identifierToken);
    assignmentNode->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, identifierToken));
    assignmentNode->children.push_back(valueNode);

    return assignmentNode;
}

// Parse an if statement
std::shared_ptr<ASTNode> StatementParser::parseIfStatement() {
    auto ifToken = parser.previous();
    
    // Parse condition
    auto conditionNode = parser.expressionParser->parseExpression();
    if (!conditionNode) {
        return nullptr;
    }

    // Check for 'then' keyword
    if (!parser.match(TokenType::THEN)) {
        std::cerr << "Expected 'then' after if condition at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Enter new scope for if block
    parser.getSymbolTable().enterScope();

    // Parse if block
    auto ifBlockNode = parseBlock();
    if (!ifBlockNode) {
        return nullptr;
    }

    // Create if node
    auto ifNode = std::make_shared<ASTNode>(ASTNodeType::IF_STATEMENT, ifToken);
    ifNode->children.push_back(conditionNode);
    ifNode->children.push_back(ifBlockNode);

    // Parse elseif and else blocks
    while (parser.peek().type == TokenType::ELSEIF) {
        parser.advance(); // Consume ELSEIF
        auto elseifCondition = parser.expressionParser->parseExpression();
        if (!elseifCondition) {
            return nullptr;
        }

    // Check for 'then' keyword after elseif condition
        if (!parser.match(TokenType::THEN)) {
            std::cerr << "Expected 'then' after elseif condition at line " 
                      << parser.peek().line << ", column " << parser.peek().column << std::endl;
            return nullptr;
        }

        // Parse elseif block
        auto elseifBlock = parseBlock();
        if (!elseifBlock) {
            return nullptr;
        }

        // Create elseif node
        auto elseifNode = std::make_shared<ASTNode>(ASTNodeType::ELSEIF_STATEMENT, parser.previous());
        elseifNode->children.push_back(elseifCondition);
        elseifNode->children.push_back(elseifBlock);
        ifNode->children.push_back(elseifNode);
    }

    // Parse else block if it exists
    if (parser.peek().type == TokenType::ELSE) {
        parser.advance(); // Consume ELSE
        auto elseBlock = parseBlock();
        if (!elseBlock) {
            return nullptr;
        }
        auto elseNode = std::make_shared<ASTNode>(ASTNodeType::ELSE_STATEMENT, parser.previous());
        elseNode->children.push_back(elseBlock);
        ifNode->children.push_back(elseNode);
    }

    // Check for 'end if' 
    if (!parser.match(TokenType::END_IF)) {
        std::cerr << "Expected 'end if' at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Exit scope for if block
    parser.getSymbolTable().exitScope();

    // Require semicolon after end if
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after 'end if' at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    return ifNode;
}

// Parse a while statement
std::shared_ptr<ASTNode> StatementParser::parseWhileStatement() {
    auto whileToken = parser.previous();
    
    // Parse condition expression 
    auto conditionNode = parser.expressionParser->parseExpression();
    if (!conditionNode) {
        return nullptr;
    }

    // Check for loop keyword after condition
    if (!parser.match(TokenType::LOOP)) {
        std::cerr << "Expected 'loop' after while condition at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Enter new scope for while block
    auto blockNode = parseBlock();
    if (!blockNode) {
        return nullptr;
    }

    // Check for end loop keyword after block
    if (!parser.match(TokenType::END_LOOP)) {
        std::cerr << "Expected 'end loop' at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Require semicolon after end loop
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after 'end loop' at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Exit scope for while block
    auto whileNode = std::make_shared<ASTNode>(ASTNodeType::WHILE_STATEMENT, whileToken);
    whileNode->children.push_back(conditionNode);
    whileNode->children.push_back(blockNode);
    return whileNode;
}

// Parse a put statement
std::shared_ptr<ASTNode> StatementParser::parsePutStatement() {
    auto putToken = parser.previous();
    
    // Handle opening parenthesis
    if (!parser.match(TokenType::OPEN_PAREN)) {
        std::cerr << "Expected '(' after put at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Parse expression
    auto expressionNode = parser.expressionParser->parseExpression();
    if (!expressionNode) {
        return nullptr;
    }
    
    // Handle closing parenthesis
    if (!parser.match(TokenType::CLOSE_PAREN)) {
        std::cerr << "Expected ')' after put expression at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Handle semicolon
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after put statement at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }

    // Create put node
    auto putNode = std::make_shared<ASTNode>(ASTNodeType::PUT_STATEMENT, putToken);
    putNode->children.push_back(expressionNode);
    return putNode;
}

// Parse a procedure definition
std::shared_ptr<ASTNode> StatementParser::parseProcedure() {
    auto procToken = parser.previous();
    
    // Parse procedure name
    if (!parser.match(TokenType::IDENTIFIER)) {
        std::cerr << "Expected procedure name at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    auto nameNode = std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, parser.previous());
    
    // Parse parameters
    std::vector<std::shared_ptr<ASTNode>> params;
    if (!parser.match(TokenType::OPEN_PAREN)) {
        std::cerr << "Expected '(' after procedure name at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Parse parameter list
    while (!parser.check(TokenType::CLOSE_PAREN)) {
        if (!parser.match(TokenType::IDENTIFIER)) {
            std::cerr << "Expected parameter name at line " << parser.peek().line 
                      << ", column " << parser.peek().column << std::endl;
            return nullptr;
        }
        params.push_back(std::make_shared<ASTNode>(ASTNodeType::PARAMETER, parser.previous()));
        
        if (!parser.check(TokenType::CLOSE_PAREN)) {
            if (!parser.match(TokenType::COMMA)) {
                std::cerr << "Expected ',' between parameters at line " 
                          << parser.peek().line << ", column " << parser.peek().column << std::endl;
                return nullptr;
            }
        }
    }
    
    // Handle closing parenthesis
    if (!parser.match(TokenType::CLOSE_PAREN)) {
        std::cerr << "Expected ')' after parameters at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Parse procedure body
    if (!parser.match(TokenType::BEGIN)) {
        std::cerr << "Expected 'begin' after procedure header at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    auto bodyNode = parseBlock();
    
    // Handle end procedure
    if (!parser.match(TokenType::END_PROCEDURE)) {
        std::cerr << "Expected 'end procedure' at line " << parser.peek().line 
                  << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Require semicolon after end procedure
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after 'end procedure' at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Create procedure node
    auto procNode = std::make_shared<ASTNode>(ASTNodeType::PROCEDURE, procToken);
    procNode->children.push_back(nameNode);
    for (auto& param : params) {
        procNode->children.push_back(param);
    }
    procNode->children.push_back(bodyNode);
    
    return procNode;
}

// Parse a procedure call
std::shared_ptr<ASTNode> StatementParser::parseProcedureCall() {
    if (!parser.match(TokenType::IDENTIFIER)) {
        std::cerr << "Expected procedure name at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Create procedure call node
    auto procName = parser.previous();
    auto callNode = std::make_shared<ASTNode>(ASTNodeType::PROCEDURE_CALL, procName);
    
    // Check for opening parenthesis
    if (!parser.match(TokenType::OPEN_PAREN)) {
        std::cerr << "Expected '(' after procedure name at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Parse arguments
    while (!parser.check(TokenType::CLOSE_PAREN)) {
        auto arg = parser.expressionParser->parseExpression();
        if (!arg) {
            return nullptr;
        }
        callNode->children.push_back(arg);
        
        if (!parser.check(TokenType::CLOSE_PAREN)) {
            if (!parser.match(TokenType::COMMA)) {
                std::cerr << "Expected ',' between arguments at line " 
                          << parser.peek().line << ", column " << parser.peek().column << std::endl;
                return nullptr;
            }
        }
    }
    
    // Handle closing parenthesis
    if (!parser.match(TokenType::CLOSE_PAREN)) {
        std::cerr << "Expected ')' after arguments at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    return callNode;
}

// Parse a procedure call statement
std::shared_ptr<ASTNode> StatementParser::parseProcedureCallStatement() {
    auto callNode = parseProcedureCall();
    if (!callNode) {
        return nullptr;
    }
    
    // Handle semicolon after procedure call
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after procedure call at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    return callNode;
}

// Parse a return statement
std::shared_ptr<ASTNode> StatementParser::parseReturnStatement() {
    auto returnToken = parser.previous();
    
    // Parse expression
    auto expressionNode = parser.expressionParser->parseExpression();
    if (!expressionNode) {
        return nullptr;
    }
    
    // Handle semicolon after return expression
    if (!parser.match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after return statement at line " 
                  << parser.peek().line << ", column " << parser.peek().column << std::endl;
        return nullptr;
    }
    
    // Create return node
    auto returnNode = std::make_shared<ASTNode>(ASTNodeType::RETURN_STATEMENT, returnToken);
    returnNode->children.push_back(expressionNode);
    return returnNode;
}

// Parse a block of statements
std::shared_ptr<ASTNode> StatementParser::parseBlock() {
    auto blockNode = std::make_shared<ASTNode>(ASTNodeType::BLOCK, Token(TokenType::UNKNOWN, "", 0, 0));

    // Enter new scope for block
    parser.getSymbolTable().enterScope();

    while (!parser.isAtEnd() && !parser.check(TokenType::END_IF) && !parser.check(TokenType::END_LOOP) && 
           !parser.check(TokenType::ELSE) && !parser.check(TokenType::ELSEIF) && 
           !parser.check(TokenType::END_PROCEDURE)) {  
        
        // Skip whitespace
        while (!parser.isAtEnd() && parser.isWhitespace(parser.peek())) {
            parser.advance();
        }
        
        // Break if at end of block
        if (parser.isAtEnd() || parser.check(TokenType::END_IF) || parser.check(TokenType::END_LOOP) || 
            parser.check(TokenType::ELSE) || parser.check(TokenType::ELSEIF) || 
            parser.check(TokenType::END_PROCEDURE)) break;  
        
        // Parse statement based on first token in block
        if (parser.match(TokenType::RETURN)) { 
            auto returnNode = parseReturnStatement();
            if (returnNode) blockNode->children.push_back(returnNode);
        } else if (parser.match(TokenType::DECLARE)) {
            auto declNode = parseDeclaration();
            if (declNode) blockNode->children.push_back(declNode);
        } else if (parser.match(TokenType::IDENTIFIER)) {
            Token identToken = parser.previous();
            if (parser.peek().type == TokenType::OPEN_PAREN) {
                parser.currentPosition--; // Back up so parseProcedureCall sees the identifier
                auto node = parseProcedureCallStatement();
                if (node) blockNode->children.push_back(node);
            } else if (parser.peek().type == TokenType::ASSIGN) {
                parser.currentPosition--; // Back up so parseAssignment sees the identifier
                auto node = parseAssignment();
                if (node) blockNode->children.push_back(node);
            } else {
                std::cerr << "Expected '<-' or '(' after identifier at line " 
                        << identToken.line << ", column " << identToken.column << std::endl;
            }
        } else if (parser.match(TokenType::IF)) {
            auto ifNode = parseIfStatement();
            if (ifNode) blockNode->children.push_back(ifNode);
        } else if (parser.match(TokenType::WHILE)) {
            auto whileNode = parseWhileStatement();
            if (whileNode) blockNode->children.push_back(whileNode);
        } else if (parser.match(TokenType::PUT)) {
            auto putNode = parsePutStatement();
            if (putNode) blockNode->children.push_back(putNode);
        } else if (!parser.isWhitespace(parser.peek())) {
            std::cerr << "Unexpected token in block: " << parser.peek().lexeme 
                      << " at line " << parser.peek().line 
                      << ", column " << parser.peek().column << std::endl;
            parser.advance();
        } else {
            parser.advance();
        }
    }

    // Exit scope for block
    parser.getSymbolTable().exitScope();

    return blockNode;
}