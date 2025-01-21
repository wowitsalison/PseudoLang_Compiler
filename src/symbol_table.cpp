#include "symbol_table.h"

SymbolTable::SymbolTable() {
    enterScope();
}

void SymbolTable::enterScope() {
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    if (scopes.size() > 1) { 
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
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

std::set<std::string> SymbolTable::getAllVariables() const {
    std::set<std::string> allVariables;
    // Iterate through all scopes from innermost to outermost
    for (auto scopeIt = scopes.rbegin(); scopeIt != scopes.rend(); ++scopeIt) {
        // Add all variables from current scope
        for (const auto& pair : *scopeIt) {
            allVariables.insert(pair.first);
        }
    }
    return allVariables;
}