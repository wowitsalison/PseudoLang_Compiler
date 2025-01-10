#pragma once
#include "token.h"
#include <unordered_map>
#include <string>

class KeywordManager {
public:
    KeywordManager();
    TokenType getKeywordType(const std::string& word) const;
    bool isMultiWordKeyword(const std::string& firstWord) const;
    TokenType getMultiWordKeywordType(const std::string& fullKeyword) const;

private:
    std::unordered_map<std::string, TokenType> keywordMap;
    std::unordered_map<std::string, TokenType> multiWordKeywordMap;
    void initializeKeywordMaps();
};