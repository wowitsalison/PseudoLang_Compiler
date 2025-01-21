#pragma once
#include "token.h"
#include <string>

class TokenScanner {
public:
    TokenScanner(const std::string& sourceCode, size_t& pos, int& line, int& col);
    Token scanToken();
    
private:
    const std::string& sourceCode;
    size_t& currentPosition;
    int& line;
    int& column;
    
    Token handleIdentifier();
    Token handleNumber();
    Token handleString();
    Token handleOperator(char c);
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
};