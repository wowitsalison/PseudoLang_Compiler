#include "lexer.h"
#include "token_scanner.h"
#include "keyword_manager.h"
#include <cctype>

// Constructor
Lexer::Lexer(const std::string& sourceCode) 
    : sourceCode(sourceCode), currentPosition(0), line(1), column(1) {
        scanner = std::make_unique<TokenScanner>(sourceCode, currentPosition, line, column);
        keywordManager = std::make_unique<KeywordManager>();
}

// Tokenize the PseudoLang source code
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        if (peek() == '/') {
            if (peekNext() == '/') {
                skipComment();
                continue;
            } else if (peekNext() == '*') {
                skipMultilineComment();
                continue;
            }
        }
        Token token = scanner->scanToken();

        // Handle potential multi-word keywords
        if (token.type == TokenType::IDENTIFIER) {
            if (keywordManager->isMultiWordKeyword(token.lexeme)) {
                size_t savedPos = currentPosition;
                int savedLine = line;
                int savedCol = column;

                skipWhitespace();
                if (!isAtEnd() && isalpha(peek())) {
                    std::string secondWord;
                    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
                        secondWord += advance();
                    }

                    std::string combined = token.lexeme + " " + secondWord;
                    TokenType multiWordType = keywordManager->getMultiWordKeywordType(combined);

                    if (multiWordType != TokenType::UNKNOWN) {
                        token = Token(multiWordType, combined, token.line, token.column);
                    } else {
                        currentPosition = savedPos;
                        line = savedLine;
                        column = savedCol;
                    }
                }
            }

            // Check for single-word keywords
            if (token.type == TokenType::IDENTIFIER) {
                TokenType keywordType = keywordManager->getKeywordType(token.lexeme);
                if (keywordType != TokenType::UNKNOWN) {
                    token = Token(keywordType, token.lexeme, token.line, token.column);
                }
            }
        }

        tokens.push_back(token);
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}

// Skip whitespace characters
void Lexer::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

// Skip single-line comments
void Lexer::skipComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

// Skip multi-line comments
void Lexer::skipMultilineComment() {
    advance(); // Consume '*'
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance(); // Consume '*'
            advance(); // Consume '/'
            break;
        }
        advance();
    }
}

// Peek at the current character
char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return sourceCode[currentPosition];
}

// Peek at the next character
char Lexer::peekNext() const {
    if (currentPosition + 1 >= sourceCode.size()) return '\0';
    return sourceCode[currentPosition + 1];
}

// Advance to the next character
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

// Check if at the end of the source code
bool Lexer::isAtEnd() const {
    return currentPosition >= sourceCode.size();
}
