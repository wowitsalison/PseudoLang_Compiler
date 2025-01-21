#pragma once
#include <string>
#include <vector>
#include <memory>
#include "token.h"
#include "token_scanner.h"
#include "keyword_manager.h"

class Lexer {
public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();

private:
    std::string sourceCode;
    size_t currentPosition;
    int line;
    int column;
    std::unique_ptr<TokenScanner> scanner;
    std::unique_ptr<KeywordManager> keywordManager;

    void skipWhitespace();
    void skipComment();
    void skipMultilineComment();
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    void savePosition(size_t& pos, int& l, int& col) const;
    void restorePosition(size_t pos, int l, int col);
};
