#include "lexer.h"
#include <cctype>
#include <unordered_map>

// Constructor with keyword map initialization
Lexer::Lexer(const std::string& sourceCode) 
    : sourceCode(sourceCode), currentPosition(0), line(1), column(1) {
    initializeKeywordMap(); // Populate the keyword map
}

// Initialize keyword map
void Lexer::initializeKeywordMap() {
    keywordMap = {
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"declare", TokenType::DECLARE}
    };
}

// Tokenize the PseudoLang source code
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        tokens.push_back(scanToken());
    }
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}

// Peek at the current character
char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return sourceCode[currentPosition];
}

// Go to next character
char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = sourceCode[currentPosition++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

// Check for end of file
bool Lexer::isAtEnd() const {
    return currentPosition >= sourceCode.size();
}

// Ignore white space
void Lexer::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

// Scan the next token
Token Lexer::scanToken() {
    char c = advance(); // grabs the next character
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", line, column);
        case '-': return Token(TokenType::MINUS, "-", line, column);
        case '*': return Token(TokenType::STAR, "*", line, column);
        case '/':
            if (peek() == '/') {
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                return Token(TokenType::COMMENT, "", line, column);
            } else {
                return Token(TokenType::SLASH, "/", line, column);
            }
        case '=': return Token(TokenType::EQUAL, "=", line, column);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", line, column);
            }
            return Token(TokenType::UNKNOWN, "!", line, column);
        case '<':
            if (peek() == '-') {
                advance();
                return Token(TokenType::ASSIGN, "<-", line, column);
            }
            if (peek() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", line, column);
            }
            return Token(TokenType::LESS, "<", line, column);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", line, column);
            }
            return Token(TokenType::GREATER, ">", line, column);
        case '(': return Token(TokenType::OPEN_PAREN, "(", line, column);
        case ')': return Token(TokenType::CLOSE_PAREN, ")", line, column);
        case '{': return Token(TokenType::OPEN_BRACE, "{", line, column);
        case '}': return Token(TokenType::CLOSE_BRACE, "}", line, column);
        case ',': return Token(TokenType::COMMA, ",", line, column);
        case ';': return Token(TokenType::SEMICOLON, ";", line, column);
        default:
            if (isalpha(c)) {
                return handleIdentifierOrKeyword(c);
            } else if (isdigit(c)) {
                return handleNumber(c);
            } else if (c == '"') {
                return handleString();
            } else {
                return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
            }
    }
    return Token(TokenType::UNKNOWN, "", line, column);
}

// Handle identifiers and keywords
Token Lexer::handleIdentifierOrKeyword(char firstChar) {
    std::string lexeme(1, firstChar);
    while (isalnum(peek())) {
        lexeme += advance();
    }

    // Check if the lexeme is a keyword
    auto it = keywordMap.find(lexeme);
    if (it != keywordMap.end()) {
        return Token(it->second, lexeme, line, column);
    }
    return Token(TokenType::IDENTIFIER, lexeme, line, column);
}

// Handle numeric literals
Token Lexer::handleNumber(char firstChar) {
    std::string number(1, firstChar);
    while (isdigit(peek())) {
        number += advance();
    }
    return Token(TokenType::NUMBER, number, line, column);
}

// Handle string literals
Token Lexer::handleString() {
    std::string str;
    while (peek() != '"' && !isAtEnd()) {
        str += advance();
    }
    if (isAtEnd()) {
        return Token(TokenType::UNKNOWN, str, line, column);
    }
    advance(); // Consume closing "
    return Token(TokenType::STRING, str, line, column);
}