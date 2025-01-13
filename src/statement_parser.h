#pragma once
#include "parser.h"

class StatementParser {
public:
    StatementParser(Parser& parser) : parser(parser) {}

    std::shared_ptr<ASTNode> parseDeclaration();
    std::shared_ptr<ASTNode> parseAssignment();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parsePutStatement();
    std::shared_ptr<ASTNode> parseProcedure();
    std::shared_ptr<ASTNode> parseProcedureCall();
    std::shared_ptr<ASTNode> parseProcedureCallStatement();
    std::shared_ptr<ASTNode> parseReturnStatement();
    std::shared_ptr<ASTNode> parseBlock();

private:
    Parser& parser;
};