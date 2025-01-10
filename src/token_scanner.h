#pragma once
#include "token.h"

class TokenScanner {
public:
    TokenScanner(const std::string& sourceCode, size_t& pos, int& line, int& col);
    Token scanToken();
    
private:
    const std::string& sourceCode;
    size_t& currentPosition;
    int& line;
    int& column;
    
    Token handleIdentifierOrKeyword(char firstChar, int startColumn);
    Token handleNumber(char firstChar, int startColumn);
    Token handleString(int startColumn);
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
};