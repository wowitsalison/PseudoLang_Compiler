#include "codegen.h"
#include "parser.h"
#include "statement_parser.h"
#include "expression_parser.h"
#include <sstream>

// Get the current indentation level
std::string CodeGenerator::getIndent() const {
    return std::string(indentLevel * 4, ' ');
}

// Main code generation method
std::string CodeGenerator::generateCode(const std::shared_ptr<ASTNode>& node) {
    if (!node) return "";
    
    switch (node->type) {
        case ASTNodeType::PROGRAM:
            return generateProgramCode(node);
        case ASTNodeType::DECLARATION:
            return generateDeclarationCode(node);
        case ASTNodeType::ASSIGNMENT:
            return generateAssignmentCode(node);
        case ASTNodeType::IF_STATEMENT:
            return generateIfStatementCode(node);
        case ASTNodeType::WHILE_STATEMENT:
            return generateWhileStatementCode(node);
        case ASTNodeType::PUT_STATEMENT:
            return generatePutStatementCode(node);
        case ASTNodeType::BINARY_OP:
            return generateBinaryOpCode(node);
        case ASTNodeType::PROCEDURE:
            return generateProcedureCode(node);
        case ASTNodeType::PROCEDURE_CALL:
            return generateProcedureCallCode(node);
        case ASTNodeType::BLOCK:
            return generateBlockCode(node);
        case ASTNodeType::RETURN_STATEMENT:
            return generateReturnStatementCode(node);
        case ASTNodeType::NUMBER:
            return node->token.lexeme;
        case ASTNodeType::STRING:
            return "\"" + node->token.lexeme + "\"";
        case ASTNodeType::IDENTIFIER:
            return node->token.lexeme;
        default:
            return "";
    }
}

// Helper methods for specific node types in whole program
std::string CodeGenerator::generateProgramCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    code << "#include <iostream>\n\n";
    
    // Forward declarations and global variables
    for (const auto& child : node->children) {
        if (child->type == ASTNodeType::DECLARATION) {
            code << "int " << child->children[0]->token.lexeme << ";\n";
        }
    }
    code << "\n";
    
    // Function declarations
    for (const auto& child : node->children) {
        if (child->type == ASTNodeType::PROCEDURE) {
            code << generateProcedureCode(child);
        }
    }
    
    code << "int main() {\n";
    indentLevel++;
    
    // Main program statements (excluding declarations and procedures)
    for (const auto& child : node->children) {
        if (child->type != ASTNodeType::DECLARATION && 
            child->type != ASTNodeType::PROCEDURE) {
            code << getIndent() << generateCode(child);
        }
    }
    
    indentLevel--;
    code << "    return 0;\n}\n";
    return code.str();
}

// Helper methods for specific node types in declarations 
std::string CodeGenerator::generateDeclarationCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (node->children.size() >= 1) {
        std::string varName = node->children[0]->token.lexeme;
        code << "int " << varName;
        if (node->children.size() >= 2) {
            code << " = " << generateCode(node->children[1]);
        } else {
            code << " = 0";
        }
        code << ";\n";
        declaredVariables[varName] = true;
    }
    return code.str();
}

// Helper methods for specific node types in assignments
std::string CodeGenerator::generateAssignmentCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (node->children.size() >= 2) {
        code << node->children[0]->token.lexeme << " = "
             << generateCode(node->children[1]) << ";\n";
    }
    return code.str();
}

// Helper methods for specific node types in if statements
std::string CodeGenerator::generateIfStatementCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (node->children.size() >= 2) {
        code << "if (" << generateCode(node->children[0]) << ") {\n";
        indentLevel++;
        code << getIndent() << generateCode(node->children[1]);
        indentLevel--;
        code << getIndent() << "}\n";
        
        // Handle ELSEIF blocks
        size_t i = 2;
        while (i < node->children.size() && 
               node->children[i]->type == ASTNodeType::ELSEIF_STATEMENT) {
            code << getIndent() << "else if (" 
                 << generateCode(node->children[i]->children[0]) << ") {\n";
            indentLevel++;
            code << getIndent() << generateCode(node->children[i]->children[1]);
            indentLevel--;
            code << getIndent() << "}\n";
            i++;
        }
        
        // Handle ELSE block
        if (i < node->children.size() && 
            node->children[i]->type == ASTNodeType::ELSE_STATEMENT) {
            code << getIndent() << "else {\n";
            indentLevel++;
            code << getIndent() << generateCode(node->children[i]->children[0]);
            indentLevel--;
            code << getIndent() << "}\n";
        }
    }
    return code.str();
}

// Helper methods for specific node types in while statements
std::string CodeGenerator::generateWhileStatementCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (node->children.size() >= 2) {
        code << "while (" << generateCode(node->children[0]) << ") {\n";
        indentLevel++;
        code << getIndent() << generateCode(node->children[1]);
        indentLevel--;
        code << getIndent() << "}\n";
    }
    return code.str();
}

// Helper methods for specific node types in put statements
std::string CodeGenerator::generatePutStatementCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (!node->children.empty()) {
        code << "std::cout << " << generateCode(node->children[0]) << " << std::endl;\n";
    }
    return code.str();
}

// Helper methods for specific node types in binary operations
std::string CodeGenerator::generateBinaryOpCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (node->children.size() >= 2) {
        code << "(" << generateCode(node->children[0]);
        
        switch (node->token.type) {
            case TokenType::PLUS: code << " + "; break;
            case TokenType::MINUS: code << " - "; break;
            case TokenType::STAR: code << " * "; break;
            case TokenType::SLASH: code << " / "; break;
            case TokenType::EQUAL: code << " == "; break;
            case TokenType::NOT_EQUAL: code << " != "; break;
            case TokenType::LESS: code << " < "; break;
            case TokenType::GREATER: code << " > "; break;
            case TokenType::LESS_EQUAL: code << " <= "; break;
            case TokenType::GREATER_EQUAL: code << " >= "; break;
            default: code << " ? "; break;
        }
        
        code << generateCode(node->children[1]) << ")";
    }
    return code.str();
}

// Helper methods for specific node types in procedures
std::string CodeGenerator::generateProcedureCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (node->children.size() >= 2) {
        std::string procName = node->children[0]->token.lexeme;
        code << "int " << procName << "(";
        
        // Parameters
        for (size_t i = 1; i < node->children.size() - 1; i++) {
            if (node->children[i]->type == ASTNodeType::PARAMETER) {
                if (i > 1) code << ", ";
                code << "int " << node->children[i]->token.lexeme;
            }
        }
        
        code << ") {\n";
        indentLevel++;
        code << getIndent() << generateCode(node->children.back());
        indentLevel--;
        code << "}\n\n";
    }
    return code.str();
}

// Helper methods for specific node types in procedure calls
std::string CodeGenerator::generateProcedureCallCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (!node->children.empty()) {
        code << node->token.lexeme << "(";
        for (size_t i = 0; i < node->children.size(); i++) {
            if (i > 0) code << ", ";
            code << generateCode(node->children[i]);
        }
        code << ")";
    }
    return code.str();
}

// Helper methods for specific node types in blocks
std::string CodeGenerator::generateBlockCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    for (const auto& child : node->children) {
        code << getIndent() << generateCode(child);
    }
    return code.str();
}

// Helper methods for specific node types in return statements
std::string CodeGenerator::generateReturnStatementCode(const std::shared_ptr<ASTNode>& node) {
    std::stringstream code;
    if (!node->children.empty()) {
        code << "return " << generateCode(node->children[0]) << ";\n";
    }
    return code.str();
}