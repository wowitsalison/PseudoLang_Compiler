#include "token_scanner.h"
#include <cctype>
#include <stdexcept>

// Constructor for TokenScanner
TokenScanner::TokenScanner(const std::string& sourceCode, size_t& pos, int& line, int& col)
    : sourceCode(sourceCode), currentPosition(pos), line(line), column(col) {}

// Scans the next token in the source code
Token TokenScanner::scanToken() {
    char c = advance();
    
    if (isspace(c)) return scanToken();
    
    int startCol = column - 1;  // Adjust for the advance() call
    int startLine = line;
    
    // Handle identifiers
    if (isalpha(c) || c == '_') {
        currentPosition--;  // Move back to re-read the first character
        column--;
        return handleIdentifier();
    }
    
    // Handle numbers
    if (isdigit(c)) {
        currentPosition--;  // Move back to re-read the first digit
        column--;
        return handleNumber();
    }
    
    // Handle strings
    if (c == '"') {
        currentPosition--;  // Move back to include the opening quote
        column--;
        return handleString();
    }
    
    // Handle operators and other characters
    return handleOperator(c);
}

// Helper functions for scanning identifier tokens 
Token TokenScanner::handleIdentifier() {
    int startCol = column;
    int startLine = line;
    std::string identifier;
    
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        identifier += advance();
    }
    
    return Token(TokenType::IDENTIFIER, identifier, startLine, startCol);
}

// Helper functions for scanning number tokens
Token TokenScanner::handleNumber() {
    int startCol = column;
    int startLine = line;
    std::string number;
    
    while (!isAtEnd() && isdigit(peek())) {
        number += advance();
    }
    
    return Token(TokenType::NUMBER, number, startLine, startCol);
}

// Helper functions for scanning string tokens
Token TokenScanner::handleString() {
    int startCol = column;
    int startLine = line;
    std::string str;
    
    advance();  // Consume opening quote
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            throw std::runtime_error("Unterminated string literal");
        }
        str += advance();
    }
    
    if (isAtEnd()) {
        return Token(TokenType::UNTERMINATED_STRING, str, startLine, startCol);
    }
    
    advance();  // Consume closing quote
    return Token(TokenType::STRING, str, startLine, startCol);
}

// Helper functions for scanning operator tokens
Token TokenScanner::handleOperator(char c) {
    int startCol = column - 1;  // Adjust for the advance() call
    int startLine = line;
    
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", startLine, startCol);
        case '-': return Token(TokenType::MINUS, "-", startLine, startCol);
        case '*': return Token(TokenType::STAR, "*", startLine, startCol);
        case '/': return Token(TokenType::SLASH, "/", startLine, startCol);
        case '=': return Token(TokenType::EQUAL, "=", startLine, startCol);
        case '!':
            if (match('=')) return Token(TokenType::NOT_EQUAL, "!=", startLine, startCol);
            break;
        case '<':
            if (match('=')) return Token(TokenType::LESS_EQUAL, "<=", startLine, startCol);
            if (match('-')) return Token(TokenType::ASSIGN, "<-", startLine, startCol);
            return Token(TokenType::LESS, "<", startLine, startCol);
        case '>':
            if (match('=')) return Token(TokenType::GREATER_EQUAL, ">=", startLine, startCol);
            return Token(TokenType::GREATER, ">", startLine, startCol);
        case '(': return Token(TokenType::OPEN_PAREN, "(", startLine, startCol);
        case ')': return Token(TokenType::CLOSE_PAREN, ")", startLine, startCol);
        case '{': return Token(TokenType::OPEN_BRACE, "{", startLine, startCol);
        case '}': return Token(TokenType::CLOSE_BRACE, "}", startLine, startCol);
        case ',': return Token(TokenType::COMMA, ",", startLine, startCol);
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startCol);
    }
    
    return Token(TokenType::UNKNOWN, std::string(1, c), startLine, startCol);
}

// Helper functions for scanning characters
char TokenScanner::peek() const {
    if (isAtEnd()) return '\0';
    return sourceCode[currentPosition];
}

char TokenScanner::peekNext() const {
    if (currentPosition + 1 >= sourceCode.size()) return '\0';
    return sourceCode[currentPosition + 1];
}

char TokenScanner::advance() {
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

bool TokenScanner::match(char expected) {
    if (isAtEnd() || peek() != expected) return false;
    advance();
    return true;
}

// Check for end of source code
bool TokenScanner::isAtEnd() const {
    return currentPosition >= sourceCode.size();
}