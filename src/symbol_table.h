#pragma once
#include <unordered_map>
#include <string>
#include <vector>

enum class VariableType {
    INTEGER,
    STRING,
    BOOLEAN,
    UNKNOWN
};

class SymbolTable {
public:
    void enterScope();
    void exitScope();
    void declareVariable(const std::string& name, VariableType type);
    VariableType getVariableType(const std::string& name) const;
    bool isVariableDeclared(const std::string& name) const;

private:
    std::vector<std::unordered_map<std::string, VariableType>> scopes;
};