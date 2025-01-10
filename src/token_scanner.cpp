#include "token_scanner.h"
#include <cctype>
#include <iostream> // For debug prints

TokenScanner::TokenScanner(const std::string& sourceCode, size_t& pos, int& line, int& col)
    : sourceCode(sourceCode), currentPosition(pos), line(line), column(col) {}

Token TokenScanner::scanToken() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }

    char c = advance();

    switch (c) {
        case '+':
            return Token(TokenType::PLUS, "+", line, column);
        case '-':
            return Token(TokenType::MINUS, "-", line, column);
        case '*':
            return Token(TokenType::STAR, "*", line, column);
        case '/':
            return Token(TokenType::SLASH, "/", line, column);
        case '=':
            return Token(TokenType::EQUAL, "=", line, column);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", line, column);
            }
            break;
        case '<':
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
        case '(':
            return Token(TokenType::OPEN_PAREN, "(", line, column);
        case ')':
            return Token(TokenType::CLOSE_PAREN, ")", line, column);
        case '{':
            return Token(TokenType::OPEN_BRACE, "{", line, column);
        case '}':
            return Token(TokenType::CLOSE_BRACE, "}", line, column);
        case ',':
            return Token(TokenType::COMMA, ",", line, column);
        case ';':
            return Token(TokenType::SEMICOLON, ";", line, column);
        default:
            if (isalpha(c) || c == '_') {
                std::string identifier(1, c);
                while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
                    identifier += advance();
                }
                std::cout << "Identifier found: " << identifier << std::endl; // Debug print
                return Token(TokenType::IDENTIFIER, identifier, line, column);
            } else if (isdigit(c)) {
                std::string number(1, c);
                while (!isAtEnd() && isdigit(peek())) {
                    number += advance();
                }
                std::cout << "Number found: " << number << std::endl; // Debug print
                return Token(TokenType::NUMBER, number, line, column);
            } else if (c == '"') {
                std::string str;
                while (!isAtEnd() && peek() != '"') {
                    str += advance();
                }
                if (isAtEnd()) {
                    return Token(TokenType::UNTERMINATED_STRING, str, line, column);
                }
                advance(); // Consume closing quote
                return Token(TokenType::STRING, str, line, column);
            }
            std::cout << "Unknown token: " << c << std::endl; // Debug print
            break;
    }

    return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
}

char TokenScanner::peek() const {
    if (isAtEnd()) return '\0';
    return sourceCode[currentPosition];
}

char TokenScanner::peekNext() const {
    if (currentPosition + 1 >= sourceCode.size()) return '\0';
    return sourceCode[currentPosition + 1];
}

char TokenScanner::advance() {
    currentPosition++;
    column++;
    return sourceCode[currentPosition - 1];
}

bool TokenScanner::isAtEnd() const {
    return currentPosition >= sourceCode.size();
}