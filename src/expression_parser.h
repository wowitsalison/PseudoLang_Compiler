#pragma once
#include "parser.h"

class ExpressionParser {
public:
    ExpressionParser(Parser& parser) : parser(parser) {}
    
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parsePrimary();

private:
    Parser& parser;
};