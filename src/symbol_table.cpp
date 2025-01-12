#include "symbol_table.h"

void SymbolTable::enterScope() {
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

void SymbolTable::declareVariable(const std::string& name, VariableType type) {
    if (!scopes.empty()) {
        scopes.back()[name] = type;
    }
}

VariableType SymbolTable::getVariableType(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    return VariableType::UNKNOWN;
}

bool SymbolTable::isVariableDeclared(const std::string& name) const {
    for (const auto& scope : scopes) {
        if (scope.find(name) != scope.end()) {
            return true;
        }
    }
    return false;
}