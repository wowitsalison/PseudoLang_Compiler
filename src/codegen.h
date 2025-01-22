#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "parser.h"
#include "expression_parser.h"
#include "statement_parser.h"
#include "symbol_table.h"

class CodeGenerator {
private:
    int indentLevel = 0;
    std::string getIndent() const;
    std::unordered_map<std::string, bool> declaredVariables;
public:
    CodeGenerator() = default;
    
    // Main code generation method
    std::string generateCode(const std::shared_ptr<ASTNode>& node);
    
    // Helper methods for specific node types
    std::string generateProgramCode(const std::shared_ptr<ASTNode>& node);
    std::string generateDeclarationCode(const std::shared_ptr<ASTNode>& node);
    std::string generateAssignmentCode(const std::shared_ptr<ASTNode>& node);
    std::string generateIfStatementCode(const std::shared_ptr<ASTNode>& node);
    std::string generateWhileStatementCode(const std::shared_ptr<ASTNode>& node);
    std::string generatePutStatementCode(const std::shared_ptr<ASTNode>& node);
    std::string generateBinaryOpCode(const std::shared_ptr<ASTNode>& node);
    std::string generateProcedureCode(const std::shared_ptr<ASTNode>& node);
    std::string generateProcedureCallCode(const std::shared_ptr<ASTNode>& node);
    std::string generateBlockCode(const std::shared_ptr<ASTNode>& node);
    std::string generateReturnStatementCode(const std::shared_ptr<ASTNode>& node);
};