#include "symbol_table.h"

// Constructor for SymbolTable class
SymbolTable::SymbolTable() {
    enterScope();
}

// Add empty map to the scopes stack to enter a new scope
void SymbolTable::enterScope() {
    scopes.push_back({});
}

// Remove top map from scopes stack to exit current scope
void SymbolTable::exitScope() {
    if (scopes.size() > 1) { // Keeps it from removing the global scope
        scopes.pop_back(); 
    }
}

// Declare new variable in current scope
void SymbolTable::declareVariable(const std::string& name, VariableType type) {
    if (!scopes.empty()) { // Make sure there is a scope to declare the variables in
        scopes.back()[name] = type; // Add the variable to the current scope
    }
}

// Retrieve the type of a variable
VariableType SymbolTable::getVariableType(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) { // Iterate through all scopes
        auto found = it->find(name); 
        if (found != it->end()) { 
            return found->second; // Return the type of the variable
        }
    }
    return VariableType::UNKNOWN; // Return UNKNOWN if variable is not found
}

// Make sure variable is declared
bool SymbolTable::isVariableDeclared(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) { // Iterate through all scopes
        if (it->find(name) != it->end()) { 
            return true; // Return true if variable is found
        }
    }
    return false; // Return false if variable is not found
}

// Return all variables in the symbol table
std::set<std::string> SymbolTable::getAllVariables() const {
    std::set<std::string> allVariables; // Create a set to store all variables
    for (auto scopeIt = scopes.rbegin(); scopeIt != scopes.rend(); ++scopeIt) { // Iterate through all scopes
        for (const auto& pair : *scopeIt) {
            allVariables.insert(pair.first); // Add the variable to the set
        }
    }
    return allVariables;
}