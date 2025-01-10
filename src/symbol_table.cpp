#include "symbol_table.h"

void SymbolTable::declareVariable(const std::string& name, VariableType type) {
    table[name] = type;
}

VariableType SymbolTable::getVariableType(const std::string& name) const {
    auto it = table.find(name);
    if (it != table.end()) {
        return it->second;
    }
    return VariableType::UNKNOWN;
}

bool SymbolTable::isVariableDeclared(const std::string& name) const {
    return table.find(name) != table.end();
}