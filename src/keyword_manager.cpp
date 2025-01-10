#include "keyword_manager.h"

KeywordManager::KeywordManager() {
    initializeKeywordMaps();
}

void KeywordManager::initializeKeywordMaps() {
    keywordMap = {
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"elseif", TokenType::ELSEIF},
        {"while", TokenType::WHILE},
        {"declare", TokenType::DECLARE},
        {"put", TokenType::PUT},
        {"then", TokenType::THEN},
        {"loop", TokenType::LOOP},
        {"procedure", TokenType::PROCEDURE},
        {"begin", TokenType::BEGIN},
        {"end", TokenType::END},
        {"return", TokenType::RETURN}
    };

    multiWordKeywordMap = {
        {"end if", TokenType::END_IF},
        {"end loop", TokenType::END_LOOP},
        {"end procedure", TokenType::END_PROCEDURE}
    };
}

TokenType KeywordManager::getKeywordType(const std::string& word) const {
    auto it = keywordMap.find(word);
    if (it != keywordMap.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

TokenType KeywordManager::getMultiWordKeywordType(const std::string& phrase) const {
    auto it = multiWordKeywordMap.find(phrase);
    if (it != multiWordKeywordMap.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

bool KeywordManager::isMultiWordKeyword(const std::string& firstWord) const {
    for (const auto& pair : multiWordKeywordMap) {
        if (pair.first.find(firstWord) == 0) {
            return true;
        }
    }
    return false;
}