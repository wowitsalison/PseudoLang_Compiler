#pragma once
#define CODEGENERATOR_H

#include <string>
#include "parser.h"

class CodeGenerator {
public:
    CodeGenerator(const ASTNode& ast);
    std::string generate();
private:
    ASTNode ast;
};